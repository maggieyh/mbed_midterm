# Doodle Car
A Boet Bot Car with a pen installed is able to draw sketch as soon as it receives raw sketch from remote PC, or locally draw some geometry if not connecting to PC.

## Hardware
* Boet Bot Car
* IR Gesture censor
* uLCD 
* optical encoder
* servo motor * 2
* standard servo (for installing pen)
* Battery box
* Xbee * 2  

## Steps   
1. Assemble the Boet Bot Car as the picutre shown   
![Imgur](https://i.imgur.com/ihjfjRo.jpg)     


2. config    
    2.1           
    First adjust the servo motor speeds to allow the car move straight. The speeds of individual servo motor can be updated in methods including `ServoCtrl` , `ServoDistance` in the `main.cpp`.   
    2.2   
    lines 9-25 in `main.cpp` configures the pin number connecting to the correesponding components. 
    lines 25-41 lists the major functions that will be used in the whole procedure

3. flash the `main.cpp` into the k64F board  

4. Press reset on K64f, the uLCD will display the menu: choosing between remote or local. Remote function will connects to the PC which is executing `main.py` under the same directory with the paired Xbees, and receives the goal sketch from PC, then proceeds to draw. As for local function, the doodle car can draw some simple sketch in case it is not connected to remote PC.    
  4.1 For remote function of BBCar   
  First connect the XBee to your PC, check your directory of Xbee     
 For main.py, please first install the dependency `svgpathtools`, which will help parsing the scale vector image(which you draw by yourself!) into individual commands for the car.   
  run the main.py with the following the commands `python main.py _your_img_file`    
  The python file will connects the PC to the Boet Bot Car by using XBee. As the BBCar proceeds to draw the sketch, the PC will sequencially receive the BBCar position and print out on the command line.      


## Core Tech & Code Explain    
0. Core function: draw out arbitrary sketch with multiple marks and curves   
To allow the BBCar to darw out curves, I use svgpathtool to approximate the curves to Bezier curves, then sample the points on these curves then send these individual points to BBCar, so that BBCar sequentially processes these points.
For multiple marks, the BBCar needs to know where a marks starts and ends, so the poitns sent from PC will have not only processed points(with positive value) but also those points of negative values which indicates BBCar to lift up the pen.   


1. main.cpp
  The main function runs `sys_init()` first:   
  The car_init configures the servo and encoder to encoder_ticker. 
  Then GSensor ginit starts running the gesture engine.
  The the sys_init will use uLCD display the mode menu on the LCD, and use gesture of up and down to move the  cursor, and right or left to confirm your choice.   
  `processPoints`:    
  As the name suggest, the function takes in individual coordinates.  `points`, global array, stores the sequential coordinates for the BBCar to draw out the corresponding sketch. processPoints method will go through these points and command the BBCar to move to these coordinates in right sequence. Note that BBCar supports multiple marks, so `points` stores the pause points which has either x or y to be less than zero, for that the BBCar simply proceed to the point without drawing out redundant lines.      
  `TurnPen`:  
  There is deviation between the center of the BBCar and the pen points, so to draw two successive lines with certain degree , the car needs to do adjustment before roating for the goal degree. 


2. main.py
The python file starts with parsing the input images into sampled points as in lines 87-104. Then the points will be sent through serial by Xbee to the BBCar.
