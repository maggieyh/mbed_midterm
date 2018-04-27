#include "mbed.h"
#include "glibr.h"
// #include "parallax.h"
#include "bbcar.h"
glibr GSensor(D14,D15);
Serial pc(USBTX, USBRX);
PwmOut pin11(D11), pin12(D12);
DigitalIn pin3(D3);
DigitalOut p(LED1);

parallax_encoder *encoder3_ptr;
parallax_servo *servo0_ptr, *servo1_ptr;
int isr_flag = 0;


int main(void)
{
    p = 0;
    
    // Initialize Sensor with I2C
    if ( GSensor.ginit() ) {
        pc.printf("APDS-9960 initialization complete\r\n");
    } else {
        pc.printf("Something went wrong during APDS-9960 init\r\n");
        return -1;
    }
 
    // Start running the APDS-9960 gesture sensor engine
    if ( GSensor.enableGestureSensor(true) ) {
        pc.printf("Gesture sensor is now running\r\n");
    } else {
        pc.printf("Something went wrong during gesture sensor init!\r\n");
        return -1;
    }
    
    while(1) {
        //pc.printf(".....\r\n");
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
}

