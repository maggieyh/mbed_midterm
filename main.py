import serial
import sys
import os
import time
from svgpathtools import svg2paths, wsvg, kinks, smoothed_path, path

def frange(beg, end, step):
    i = beg
    while i <= end:
        yield i
        i += step

def setupXbee():
    s.write("+++")
    char = s.read(2)
    print("Enter AT mode.")
    print(char)

    s.write("ATMY 0x81\r\n")
    char = s.read(3)
    print("Set MY 0x81.")
    print(char)

    s.write("ATDL 0x80\r\n") 
    char = s.read(3)
    print("Set DL 0x80.")
    print(char)

    s.write("ATWR\r\n")
    char = s.read(3)
    print("Write config.")
    print(char)

    s.write("ATMY\r\n")
    char = s.read(3)
    print("MY :")
    print(char)

    s.write("ATDL\r\n")
    char = s.read(3)
    print("DL : ")
    print(char)

    s.write("ATCN\r\n")
    char = s.read(3)
    print("Exit AT mode.")
    print(char)
    print("connecting...")
    # send to remote
    s.write("2341j")
    line = s.read(4)
    print(line)
    print("Connected")

def getPoints(paths):
    for apath in paths:
        if type(apath[0]) == path.Line:
            # or ((apath.point(0)-apath.point(1)).real ** 2 +(apath.point(0)-apath.point(1)).imag ** 2) < 4 
            yield apath.point(0)
            yield apath.point(1)
        # elif apath.point(0)
        else:
            for i in frange(0, 1, 0.1):
                yield apath.point(i)
        yield -1-1j

serdev = '/dev/tty.usbserial-AE019MG3'
s = serial.Serial(serdev, 9600)

# XBee setting
# setupXbee()

s.write("8888 j")
time.sleep(1)
line = s.read(4)
print(line)
print("Connected")
str = sys.argv[1]
mypaths, _ = svg2paths(os.path.abspath(str))
if len(mypaths) == 1:
    mypaths = mypaths[1]
else:
    mypaths = mypaths[2:-1:2]
print mypaths
print "------"

minx = 100000
miny = 100000
maxy = 0
points = list(getPoints(mypaths))
for i in points:
    if not type(i) is complex: 
        continue
    x = i.real/20.0
    y = i.imag/20.0
    if x < minx and x > 0:
        minx = x
    if y < miny and y > 0:
        miny = y
    if y > maxy: 
        maxy = y
f = open('render.svg','w')
message = '<svg xmlns="http://www.w3.org/2000/svg" version="1.1">'
idx = 0
for i in points:  
    if not type(i) is complex: 
        continue
    x = i.real/20.0
    y = i.imag/20.0
    print '{0:.4f} {1:.4f} .'.format(x-minx, maxy-y)
    message += '<circle cx="{0:.4f}" cy="{1:.4f}" r="1" stroke="black" stroke-width="0" fill="red" /><text x="{0:.4f}" y="{1:.4f}" fill="black" style="font-size:3px;">{2}</text>'.format(x-minx+10, y-miny+10, idx)
    s.write('{0:.4f} {1:.4f} j'.format(x-minx, maxy-y))
    time.sleep(1)
    ack = s.read(4)
    print ack
    idx += 1
message += '</svg>'
f.write(message)
f.close()
s.write("endj")
s.read(4)
# time.sleep
while True:
    loc = s.read(10)
    print loc