#include "parallax.h"
extern parallax_servo *servo0_ptr, *servo1_ptr;
extern Ticker servo_ticker;
extern Ticker encoder_ticker;
extern parallax_encoder *encoder3_ptr;

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
        
};