#include "mbed.h"
#include "glibr.h"
#include "mbed_rpc.h"
#include "bbcar.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2);
glibr GSensor(D14,D15);
Serial pc(USBTX, USBRX);
Serial xbee(D9, D7);
PwmOut pin11(D11), pin12(D12);
DigitalIn pin3(D3);
DigitalOut p(LED1);



parallax_encoder *encoder3_ptr;
parallax_servo *servo0_ptr, *servo1_ptr;
int isr_flag = 0;

MyCar BBCar;
void sys_init();
void gesture_handler();

Ticker gesture_ticker;

int main(void)
{
    sys_init();

    
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
    BBCar.init(pin11, pin12, pin3);

    // Initialize Sensor with I2C
    if ( GSensor.ginit() ) {
        p = 1;
        pc.printf("APDS-9960 initialization complete\r\n");
    } else {
        p = 0;
        pc.printf("Something went wrong during APDS-9960 init\r\n");
    }
    
    // Start running the APDS-9960 gesture sensor engine
    if ( GSensor.enableGestureSensor(true) ) {
        pc.printf("Gesture sensor is now running\r\n");
    } else {
        pc.printf("Something went wrong during gesture sensor init!\r\n");
    }

    gesture_ticker.attach(&gesture_handler, .01);
    
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
