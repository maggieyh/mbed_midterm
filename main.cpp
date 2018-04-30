#include "mbed.h"
#include "glibr.h"
#include "parallax.h"
#include "uLCD_4DGL.h"
#include <cmath>      
#include <cstdlib>
#define PI 3.14159265

uLCD_4DGL uLCD(D1, D0, D2);
glibr GSensor(D14,D15);
Serial pc(USBTX, USBRX);
Serial xbee(D9, D7);
PwmOut pin11(D11), pin12(D12);
DigitalIn pin3(D3), pin2(D2);
DigitalOut p(LED1);
InterruptIn sw2btn(SW2);
int isr_flag = 0;
int mode = -1; // remote control or local demo
Ticker gesture_ticker;
float car_x, car_y, car_theta;
float points[5000][2] = {0};
int point_idx = 0;
parallax_servo *servo0_ptr, *servo1_ptr;
parallax_encoder *encoder3_ptr, *encoder2_ptr;

void sys_init();
void gesture_handler();
void xbeeConnect();
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
void mode_switch();
void car_init(PwmOut &pin_servo0, PwmOut &pin_servo1,  DigitalIn &d3, DigitalIn &d2);
void ServoCtrl( int speed );
void ServoStop();
void ServoDistaqnce(float distance);
void ServoTurn(float deg);
void PointToPoint(float x, float y);
void communicate_mode();
void default_sketch(int num);
int main(void)
{
    mode = 0;
    sys_init();

    
    if (mode == 0) {
        uLCD.cls();
        uLCD.printf("Remote\nTrying connect....\n");
        xbeeConnect();
        communicate_mode();

    } else if(mode == 1) {
        uLCD.printf("Menu\n^1.Circle \n 2.Square\n 3.Triangle");
        int demo_num = 0;
        while(sw2btn == 1) {
            
            if ( GSensor.isGestureAvailable() ) {    
                switch ( GSensor.readGesture() ) {        // gesture differentiate
                    case DIR_UP:
                        demo_num =  abs((demo_num - 1) % 3);
                        pc.printf("UP\r\n");
                        break;
                    case DIR_DOWN:
                        demo_num = (demo_num + 1) % 3;
                        pc.printf("DOWN\r\n");
                        break;
                    case DIR_RIGHT:
                    case DIR_LEFT:
                        pc.printf("RIGHT\r\n");
                        uLCD.cls(); 
                        uLCD.printf("running: %d\n", demo_num);
                        wait(3);
                        default_sketch(demo_num);
                        uLCD.cls();
                        uLCD.printf("Menu\n 1.Circle \n 2.Square\n 3.Triangle");
                        break;
                    default:
                        pc.printf("NONE\r\n");
                        break;
                }
                for(int i = 1; i < 4; i++) { uLCD.locate(0, i); uLCD.printf(" ");} 
                uLCD.locate(0, demo_num+1);
                uLCD.printf("^");
             }
        }
    }

    


    // ServoTurn(-160);
    // PointToPoint(10, 0);

    // PointToPoint(6.4, 4.8);
    // PointToPoint(0,0);
    // PointToPoint(10, 10);

    // PointToPoint(0, 0);
  
    // ServoCtrl(100);
    // wait(1.0);
  
    
    // ServoStop();
    // wait(1000);
}


void ServoDistaqnce(float distance) 
{
    encoder3_ptr->reset();
    ServoCtrl(90);
    while(encoder3_ptr->get_cm() < distance) wait_ms(10);
    ServoStop();
}


void ServoTurn(float deg){
    int speed = -60;
    encoder3_ptr->reset();
    if (deg < 0) { speed = -speed; deg = -deg;}
    //25 encoder 3
    servo0_ptr->set_speed(speed*0.5);
    servo1_ptr->set_speed(speed*0.7);
    // float del = abs(encoder3_ptr->get_cm() - deg * 31.5 / 360.0);
    while(encoder3_ptr->get_cm() < deg * 31.5 / 360.0) {
        // if(abs(encoder3_ptr->get_cm() - deg * 31 / 360.0) < 0.1 * del) {
        //     speed = speed / 3;
        //     servo0_ptr->set_speed(speed*0.4);
        //     servo1_ptr->set_speed(speed*0.7);
        // }
        wait_ms(3);
    } 
    ServoStop();
}

void car_init(PwmOut &pin_servo0, PwmOut &pin_servo1,  DigitalIn &d3, DigitalIn &d2) {
    static parallax_servo servo0(pin_servo0);
    static parallax_servo servo1(pin_servo1);
    static parallax_encoder encoder3(d3);
    static parallax_encoder encoder2(d2);
    servo0_ptr = &servo0;
    servo1_ptr = &servo1;
    encoder3_ptr = &encoder3;
    encoder2_ptr = &encoder2;

    servo_ticker.attach(&servo_control, .5);
    servo0 = 0; servo1 = 0;
    servo0_ptr->set_ramp(100);
    servo1_ptr->set_ramp(1500);
    
    encoder_ticker.attach(&encoder_control, .01);
    
    car_x = 0;
    car_y = 0;
    car_theta = 0;
}

void ServoStop(){
    servo0_ptr->set_speed(0);
    servo1_ptr->set_speed(0);
    servo0_ptr->set_factor(1);
    servo1_ptr->set_factor(1);
    return;
}

void ServoCtrl( int speed ){
    servo0_ptr->set_speed(speed*0.3);
    servo1_ptr->set_speed(-speed*0.7);
    return;
}

void PointToPoint(float x, float y) {
    float deltax = x - car_x, deltay = y - car_y;
    
    float target = 0;
    if (deltax >= 0 && deltay >= 0) {
        target = atan (deltay/deltax) * 180.0 / PI;
    } else if (deltax >= 0 && deltay <= 0) {
        target = 360+atan (deltay/deltax) * 180.0 / PI;
    } else {
        target = 180+atan (deltay/deltax) * 180.0 / PI;
    }
     
    float turnDeg = target - car_theta ;
    float distance = sqrt(pow(deltax, 2.0)+pow(deltay,2.0)); 
    
    ServoTurn(turnDeg);
    wait(0.5);
    ServoDistaqnce(distance);
    car_x = x;
    car_y = y;
    car_theta = target;
    pc.printf("%f | %f  | car_theta : %f \r\n", turnDeg, distance, car_theta);
}

void mode_switch() 
{
    pc.printf("jjj");
    ServoStop();
    mode = mode? 0:1;
    if (mode) {
        // in local control
        uLCD.cls();
        uLCD.printf("Menu^\n1.Happy face\n2.Sad face\n3.Square");
    }  else {
        // remote control
        xbeeConnect();
    }
}
void sys_init() {
    car_init(pin11, pin12, pin3, pin2);
    // Initialize Sensor with I2C
    
    int succ = 0;
    if ( GSensor.ginit() ) {
        uLCD.printf("APDS-9960 init\n");
        succ = 1;
    } else {
        uLCD.printf("APDS-9960 fail\n");
    }
    // Start running the APDS-9960 gesture sensor engine
    if ( GSensor.enableGestureSensor(true) ) {
        succ = 1;
        uLCD.printf("Gesture enable\r\n");
    } else {
        uLCD.printf("Gesture fail?!\r\n");
    }
    // gesture_ticker.attach(&gesture_handler, .01);

    if (succ && mode < 0) {
        uLCD.cls();
        uLCD.set_font(FONT_7X8);
        uLCD.printf("Welcome\nChoose Mode(Up/Down to move cursor, Right to confirm)\n\n0. Remote\n1. Local\n");
        uLCD.locate(0,9);
        uLCD.printf("choose mode: %d", mode);
        int r = 0;
        while(r == 0) {
            if ( GSensor.isGestureAvailable() ) {    
                
                switch ( GSensor.readGesture() ) {        // gesture differentiate
                    case DIR_UP:
                        // uLCD.printf("UP\n");
                        pc.printf("UP\r\n");
                        mode = (mode + 1) % 2;
                        break;
                    case DIR_DOWN:
                        // uLCD.printf("DOWN\n");
                        pc.printf("DOWN\r\n");
                        mode = abs((mode - 1) % 2);
                        break;
                    case DIR_RIGHT:
                    case DIR_LEFT:
                        uLCD.printf("CONFIRM\n");
                        r=1;
                        break;
                    default:
                        pc.printf("NONE\r\n");
                        break;
                }
                uLCD.locate(0,9);
                uLCD.printf("choose mode: %d", mode);
             }
        }
        uLCD.cls();
        
    }
    
    
    
}
void default_sketch(int num) {
    switch(num) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;

    }
}



void reply_messange(char *xbee_reply, char *messange){
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    xbee_reply[2] = xbee.getc();
    if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    uLCD.printf("%s\n", messange);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
    }
}

void check_addr(char *xbee_reply, char *messenger)
{
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    xbee_reply[2] = xbee.getc();
    uLCD.printf("%s = %c%c\r\n", messenger, xbee_reply[1], xbee_reply[2]);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
}

void communicate_mode(){
    uLCD.printf("Receiving...");
    int i;
    char buf[100] = {0};
    while(1){
      i = 0;
      while (i < 50){
        buf[i] = xbee.getc();
        i++;
        buf[i] = '\0';
        if (buf[i-1] == 'j') { 
            if (buf[0] == 'e') { break;}
            char *pEnd;
            points[point_idx][0] = strtof(buf, &pEnd);
            points[point_idx][1] = strtof(pEnd, NULL);
            point_idx++;
            break;  
        }
        
      }

      xbee.printf("ACKN");
      if (buf[0] == 'e') { break;}
      wait(0.1);
      while(i >= 0 ){
        buf[i] = 0;
        i--;
      }
    }
    for(int j = 0; j < point_idx; j++) {
        pc.printf("%3f, %3f\r\n", points[j][0], points[j][1]);
    }



}
void xbeeConnect() {
    char xbee_reply[3];

    xbee.getc(); //remove the first redundant char
    int i;
    char buf[20] = {0};
    i = 0;
    while (i < 10) {
        buf[i] = xbee.getc();
        i++;
        buf[i] = '\0';
        if (buf[i-1] == 'j') { 
            break;  
        }
    
    }
    xbee.printf("ACKN");
    uLCD.printf("Connected\n");
    /*
    // XBee setting
    xbee.baud(9600);
    xbee.printf("+++");
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
        pc.printf("enter AT mode.\r\n");
        xbee_reply[0] = '\0';
        xbee_reply[1] = '\0';
    }
    xbee.printf("ATMY 0x80\r\n");
    reply_messange(xbee_reply, "setting MY : 0x80");

    xbee.printf("ATDL 0x81\r\n");
    reply_messange(xbee_reply, "setting DL : 0x81");

    xbee.printf("ATWR\r\n");
    reply_messange(xbee_reply, "write config");

    xbee.printf("ATMY\r\n");
    check_addr(xbee_reply, "MY");

    xbee.printf("ATDL\r\n");
    check_addr(xbee_reply, "DL");

    xbee.printf("ATCN\r\n");
    reply_messange(xbee_reply, "exit AT mode");
    */
}


void gesture_handler()
{
    if ( GSensor.isGestureAvailable() ) {         // gesture detect
            switch ( GSensor.readGesture() ) {        // gesture differentiate
                case DIR_UP:
                    pc.printf("UP\r\n");
                    break;
                case DIR_DOWN:
                    pc.printf("DOWN\r\n");
                    break;
                case DIR_LEFT:
                    pc.printf("LEFT\r\n");
                    break;
                case DIR_RIGHT:
                    pc.printf("RIGHT\r\n");
                    break;
                case DIR_NEAR:
                    pc.printf("NEAR\r\n");
                    break;
                case DIR_FAR:
                    pc.printf("FAR\r\n");
                    break;
                default:
                    pc.printf("NONE\r\n");
                    break;
            }
        }
}
