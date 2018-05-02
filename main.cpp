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
PwmOut pin11(D11), pin12(D12), pin5(D5);
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
parallax_stdservo *stdservo_ptr;
void sys_init();
void processPoints();
void xbeeConnect();
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
void mode_switch();
void car_init(PwmOut &pin_servo0, PwmOut &pin_servo1,  DigitalIn &d3, DigitalIn &d2);
void ServoCtrl( int speed );
void ServoStop();
void ServoDistance(float distance);
void ServoTurn(float deg);
void PointToPoint(float x, float y);
void communicate_mode();
void default_sketch(int num);
void TurnPen(float deg);
int deg_ser = 60;
int main(void)
{
    // mode = 3;
    
    sys_init();
    // *stdservo_ptr = deg_ser;
    // deg_ser = 100;
    // PointToPoint(0.0, 6.35);
    // deg_ser = 60;

    // PointToPoint(16.5669, 7.6);
    // deg_ser = 100;
    // PointToPoint(21.3011,  8.4703);
    // deg_ser = 60;
    // PointToPoint(10.4633 , 13.5790 );
    // deg_ser = 100;
    // PointToPoint(11.5848, 0);
    // deg_ser = 60;
    // PointToPoint(21.6761,6.9764);
    // ServoCtrl(100);
    // wait(1);
    // ServoStop();
    // *stdservo_ptr = 100;
    // TurnPen(60);
    // ServoTurn(45);
    // *stdservo_ptr = deg_ser;
    // p = 0;
    // ServoStop();
    // wait(1);
    // ServoTurn(-180);
    // wait(100);
    // *stdservo_ptr = deg_ser;
    // TurnPen(120);

    if (mode == 0) {
        uLCD.cls();
        uLCD.printf("Remote\nTrying connect....\n");
        xbeeConnect();
        communicate_mode();
        processPoints();

    } else if(mode == 1) {
        uLCD.printf("Menu\n^1.Square \n 2.Tirangle\n 3.Circle");
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
                        uLCD.printf("running: %d\n", demo_num+1);
                        wait(3);
                        *stdservo_ptr = deg_ser;
                        default_sketch(demo_num);
                        uLCD.cls();
                        uLCD.printf("Menu\n 1.Square \n 2.Triangle\n 3.Circle");
                        demo_num = 0;
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

}


void TurnPen(float deg) {
    // if (deg < 5 && deg > -5) return;

    *stdservo_ptr = 100;
    int speed = 90, jt = 800;
    if (abs(deg) <= 90) { jt += 6 * abs(deg-90); }
    else { jt = 1200; }
    ServoCtrl(speed);
    wait_ms(jt);
    ServoStop();
    servo0_ptr->servo_control();
    servo1_ptr->servo_control();
    wait(1);
    
    ServoTurn(deg);
    wait(1);
    if(abs(deg) <= 90) 
        ServoDistance(-4);
    else 
        ServoDistance(-2.5);
    *stdservo_ptr = deg_ser;
    wait(0.1);
    ServoStop();
}
void ServoDistance(float distance) 
{
    encoder3_ptr->reset();
    int speed = 90;
    if (distance < 0) { speed = -1 * speed; distance = -1 * distance;}
    ServoCtrl(speed);
    while(encoder3_ptr->get_cm() < distance) wait_ms(3);
    ServoStop();
    servo0_ptr->servo_control();
    servo1_ptr->servo_control();
    
    encoder3_ptr->reset();
}

void ServoTurn(float deg){
    *stdservo_ptr = 100;
    

    int speed = -75;
    encoder3_ptr->reset();
    if (deg < 0) { speed = -speed; deg = -deg;}
    //25 encoder 3
    servo0_ptr->set_speed(speed*0.5);
    servo1_ptr->set_speed(speed*0.7);
    // wait_ms(1250.0*deg/90.0);
    // servo0_ptr->set_speed(0);
    // servo1_ptr->set_speed(0);
    // servo0_ptr->servo_control();
    // servo1_ptr->servo_control();
    // wait(0.96);
    while(encoder3_ptr->get_cm() < deg * 31 / 360.0) {
        // if(abs(encoder3_ptr->get_cm() - deg * 31 / 360.0) < 0.1 * del) {
        //     speed = speed / 3;
        //     servo0_ptr->set_speed(speed*0.4);
        //     servo1_ptr->set_speed(speed*0.7);
        // }
        wait_ms(3);
    } 
    servo0_ptr->set_speed(0);
    servo1_ptr->set_speed(0);
    servo0_ptr->servo_control();
    servo1_ptr->servo_control();
    // ServoStop();
    
    // *stdservo_ptr = 75;
}

void car_init(PwmOut &pin_servo0, PwmOut &pin_servo1,  DigitalIn &d3, DigitalIn &d2, PwmOut &pin_stdservo) {
    static parallax_servo servo0(pin_servo0);
    static parallax_servo servo1(pin_servo1);
    static parallax_encoder encoder3(d3);
    static parallax_encoder encoder2(d2);
    static parallax_stdservo stdservo(pin_stdservo);
    servo0_ptr = &servo0;
    servo1_ptr = &servo1;
    encoder3_ptr = &encoder3;
    encoder2_ptr = &encoder2;
    stdservo_ptr = &stdservo;

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
int transit = 0;
void PointToPoint(float x, float y) {
    if (x < 0 || y < 0) return;
    float deltax = x - car_x, deltay = y - car_y;
    
    float target = 0;
    if (deltax >= 0 && deltay >= 0) {
        target = atan (deltay/deltax) * 180.0 / PI;
    } else if (deltax >= 0 && deltay <= 0) {
        target = 360+atan (deltay/deltax) * 180.0 / PI;
    } else {
        target = 180+atan (deltay/deltax) * 180.0 / PI;
    }
     
    float turnDeg = abs(target - car_theta) > 180 ? (360 - abs(target-car_theta)) * ((target - car_theta) > 0 ? -1 : 1) : target - car_theta ;
    float distance = sqrt(pow(deltax, 2.0)+pow(deltay,2.0)); 
    // if (transit) { 
        *stdservo_ptr = 100;
    //     ServoTurn(turnDeg);
    //     *stdservo_ptr = deg_ser;
    // } else {
        TurnPen(turnDeg);
    // }
    
    // ServoTurn(turnDeg);
    wait(0.5);
    ServoDistance(distance);
    car_x = x;
    car_y = y;
    if (turnDeg > 5 || turnDeg < -5)
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
    car_init(pin11, pin12, pin3, pin2, pin5);
    // Initialize Sensor with I2C
    if (mode >= 0) return;
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
        mode = 0;
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
    float turnDeg;
    switch(num) {
        case 0:
            PointToPoint(10, 0);
            wait(1.0);
            PointToPoint(10, 10);
            wait(1);
            PointToPoint(0, 10);
            wait(1);
            PointToPoint(0, 0);
            *stdservo_ptr = 100;
            
            break;
        case 1:
            PointToPoint(15, 0);
            wait(1.0);
            PointToPoint(7.5, 13);
            wait(1);
            PointToPoint(0, 0);
            *stdservo_ptr=100;
            turnDeg = abs(0 - car_theta) > 180 ? (360 - abs(0-car_theta)) * ((0 - car_theta) > 0 ? -1 : 1) : 0 - car_theta ;
            TurnPen(turnDeg);
            break;

        case 2:
            servo1_ptr->set_ramp(1500);
            servo0_ptr->set_ramp(1500);
            servo0_ptr->set_speed(10);
            servo1_ptr->set_speed(-100);
            
            wait(10);
            ServoStop();
            break;
        default:
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

void processPoints()
{
    int i;
    if (point_idx < 1) return;
    *stdservo_ptr = 100;
    // for(i = 0; i < point_idx; i++) {
    //     xbee.printf("%5.0f%5.0f|", points[i][0], points[i][1]);
    //     wait(0.5);
    // }
    // return;
    // for(i = 0; i < point_idx; i++)
    // pc.printf("%f | %f", points[i][0], points[i][1]);
    // if(points[0][0] + points[0][1] > 3) {
        deg_ser = 100;
         *stdservo_ptr = 100;
         PointToPoint(points[0][0], points[0][1]);
         deg_ser = 61;
         xbee.printf("%5.1f%5.1f", points[0][0], points[0][1]);
         wait(1);
    // }

    *stdservo_ptr = deg_ser;
    for( i = 1; i < point_idx; i++) 
    {
        // if (points[i][0] < 0 || points[i][1] < 0) {
        //     *stdservo_ptr = 100;
        //     deg_ser = 100;
        //     if(i + 1 < point_idx) {
        //         PointToPoint(points[i+1][0], points[i+1][1]);
        //         i++;
        //     }            
        //     deg_ser = 63;
        // } else {
           
            if (points[i][0] < 0 || points[i][1] < 0) {
                if(i + 1 < point_idx) {
                    deg_ser = 100;
                    transit = 1;
                    PointToPoint(points[i+1][0], points[i+1][1]);
                    transit = 1;
                     xbee.printf("%5.1f%5.1f", points[i+1][0], points[i+1][1]);
                    deg_ser = 61;
                    i++;
                    continue;
                }
            }
            
            xbee.printf("%5.1f%5.1f", points[i][0], points[i][1]);
            wait(0.5);
            PointToPoint(points[i][0], points[i][1]);
            // if (i + 1 < point_idx && (points[i+1][0] < 0 || points[i+1][1] < 0))
            //     deg_ser = 100;
            // if (i - 1 >= 0 && (points[i-1][0] < 0 || points[i-1][1] < 0))
            //     deg_ser = 63; 
        // }
    }
    xbee.printf("e0000000000000");
    deg_ser = 100;
    PointToPoint(0, 0);
    
}
void xbeeConnect() {
    char xbee_reply[3];
   /*
    pc.baud(9600);
    pc.printf("imherej\r\n");
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

pc.printf("hhh\r\n");
*/
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
       
}
