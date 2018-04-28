#include "mbed.h"
// #include "glibr.h"
// #include "mbed_rpc.h"
// #include "bbcar.h"
#include "parallax.h"
// #include "uLCD_4DGL.h"
#include <cmath>      
#define PI 3.14159265

// uLCD_4DGL uLCD(D1, D0, D2);
// glibr GSensor(D14,D15);
Serial pc(USBTX, USBRX);
Serial xbee(D9, D7);
PwmOut pin11(D11), pin12(D12);
DigitalIn pin3(D3), pin2(D2);
DigitalOut p(LED1);

int isr_flag = 0;
void sys_init();
void gesture_handler();
void xbeeConnect();
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
Ticker gesture_ticker;

float car_x, car_y, car_theta;
parallax_servo *servo0_ptr, *servo1_ptr;
parallax_encoder *encoder3_ptr, *encoder2_ptr;

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
void ServoCtrl( int speed ){
    servo0_ptr->set_speed(speed*0.3);
    servo1_ptr->set_speed(-speed*0.7);
    return;
}

void ServoStop(){
    servo0_ptr->set_speed(0);
    servo1_ptr->set_speed(0);
    servo0_ptr->set_factor(1);
    servo1_ptr->set_factor(1);
    return;
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
    float del = abs(encoder3_ptr->get_cm() - deg * 31.5 / 360.0);
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


int main(void)
{
    sys_init();
    p = 0;
    // ServoTurn(-160);
    // PointToPoint(10, 0);
    p = 1;
    // PointToPoint(6.4, 4.8);
    p = 0;
    // PointToPoint(0,0);
    // PointToPoint(10, 10);
    p = 1;
    // PointToPoint(0, 0);
  
    // ServoCtrl(100);
    // wait(1.0);
    p  = 1;
    
    // ServoStop();
    // wait(1000);
    /*
    xbeeConnect();
    int i;
    char buf[100] = {0};

    while(1) 
    {
        i = 0;
        xbee.getc();
        while(i<4) {
            buf[i] = xbee.getc();
            i++;
            buf[i]='\0';
        }
        xbee.printf("%s", buf);
        wait(0.1);
        i = 0; 
        while(buf[i] != '\0'){
           buf[i] = 0;
           i++;
         }
    }*/
}


void sys_init() {
    car_init(pin11, pin12, pin3, pin2);
    xbeeConnect();
    // Initialize Sensor with I2C
    // if ( GSensor.ginit() ) {
    //     p = 1;
    //     pc.printf("APDS-9960 initialization complete\r\n");
    // } else {
    //     p = 0;
    //     pc.printf("Something went wrong during APDS-9960 init\r\n");
    // }
    
    // // Start running the APDS-9960 gesture sensor engine
    // if ( GSensor.enableGestureSensor(true) ) {
    //     pc.printf("Gesture sensor is now running\r\n");
    // } else {
    //     pc.printf("Something went wrong during gesture sensor init!\r\n");
    // }

    // gesture_ticker.attach(&gesture_handler, .01);
    
}

// void default_sketch1() {

// }



void reply_messange(char *xbee_reply, char *messange){
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    xbee_reply[2] = xbee.getc();
    if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    pc.printf("%s\r\n", messange);
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
    pc.printf("%s = %c%c\r\n", messenger, xbee_reply[1], xbee_reply[2]);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
}

void xbeeConnect() {
    
    pc.baud(9600);

    char xbee_reply[3];

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
    xbee.getc();

    // start
    pc.printf("start\r\n");
    char buf[100] = {0};
    int i = 0;

        xbee.getc();
        while (i < 4){
          buf[i] = xbee.getc();
          i++;
          buf[i] = '\0';
        }
        xbee.printf("%s", buf);
        wait(0.1);

        i = 0;
        while(buf[i] != '\0'){
          buf[i] = 0;
          i++;
        }
}

/*
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
*/