#include "mbed.h"
// #include "glibr.h"
// #include "mbed_rpc.h"
// #include "bbcar.h"
#include "parallax.h"
// #include "uLCD_4DGL.h"

// uLCD_4DGL uLCD(D1, D0, D2);
// glibr GSensor(D14,D15);
Serial pc(USBTX, USBRX);
// Serial xbee(D9, D7);
PwmOut pin11(D11), pin12(D12);
DigitalIn pin3(D3), pin2(D2);
DigitalOut p(LED1);

int isr_flag = 0;
void sys_init();
void gesture_handler();

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
    pc.printf("jjjj");
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

void ServoTurn(float deg){
    int speed = 60;
    encoder3_ptr->reset();
    if (deg < 0) { speed = -speed; deg = -deg;}
    //25 encoder 3
    servo0_ptr->set_speed(speed*0.4);
    servo1_ptr->set_speed(speed*0.7);
    float del = abs(encoder3_ptr->get_cm() - deg * 32 / 360.0);
    while(encoder3_ptr->get_cm() < deg * 32 / 360.0) {
        if(abs(encoder3_ptr->get_cm() - deg * 32 / 360.0) < 0.1 * del) {
            speed = 30;
            servo0_ptr->set_speed(speed*0.4);
            servo1_ptr->set_speed(speed*0.7);
        }
           
        // servo0_ptr->set_speed(30*( encoder2_ptr->get_cm() - deg * 15.0 / 360.0 ));
        // servo1_ptr->set_speed(30*( encoder2_ptr->get_cm() - deg * 15.0 / 360.0 )); 
        wait_ms(5);
    } 
    ServoStop();
}


int main(void)
{
    car_init(pin11, pin12, pin3, pin2);
    p = 1;
    ServoTurn(90.0);
    // ServoCtrl(100);
    // wait(1.0);
    p  = 0;
    
    ServoStop();
    wait(1000);
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

/*
void sys_init() {
    // car_init(pin11, pin12, pin3);

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
*/
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