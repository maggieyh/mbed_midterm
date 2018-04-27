#include "parallax.h"
#include "mbed.h"
extern parallax_servo *servo0_ptr, *servo1_ptr;
extern Ticker servo_ticker;
extern Ticker encoder_ticker;
extern parallax_encoder *encoder3_ptr;
extern Serial xbee;
extern Serial pc;
class MyCar 
{
    public:
        float car_x, car_y, car_theta;
        
        void init(PwmOut &pin_servo0, PwmOut &pin_servo1,  DigitalIn &d3) {
            static parallax_servo servo0(pin_servo0);
            static parallax_servo servo1(pin_servo1);
            static parallax_encoder encoder3(d3);

            servo0_ptr = &servo0;
            servo1_ptr = &servo1;
            encoder3_ptr = &encoder3;

            servo_ticker.attach(&servo_control, .5);
            servo0 = 0; servo1 = 0;
            servo0_ptr->set_ramp(100);
            servo1_ptr->set_ramp(1500);
            
            encoder_ticker.attach(&encoder_control, .01);
            
            
        }
        float readEncoder() {
            return (float)(*encoder3_ptr);
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

        void ServoTurn( int speed ){
            servo0_ptr->set_speed(speed*0.3);
            servo1_ptr->set_speed(speed*0.7);
        }

        void PointToPoint(float x, float y) {

        }

        void default_sketch1() {

        }
};





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
 
 void check_addr(char *xbee_reply, char *messenger){
   xbee_reply[0] = xbee.getc();
   xbee_reply[1] = xbee.getc();
   xbee_reply[2] = xbee.getc();
   pc.printf("%s = %c%c\r\n", messenger, xbee_reply[1], xbee_reply[2]);
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
   xbee_reply[2] = '\0';
 }

 void xbeeConnect() 
 {
   
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
    xbee.printf("ATMY 0x60\r\n");
    reply_messange(xbee_reply, "setting MY : 0x60");

    xbee.printf("ATDL 0x61\r\n");
    reply_messange(xbee_reply, "setting DL : 0x62");

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

}