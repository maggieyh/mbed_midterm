import serial
import sys
import os
import time
from svgpathtools import svg2paths, wsvg, kinks, smoothed_path, path
# w_fac = 960.0/10.0
# h_fac = 720.0/10.0
# serdev = '/dev/tty.usbserial-AE019MG3'
# s = serial.Serial(serdev, 9600)
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
            yield apath.point(0)
            yield apath.point(1)
        else:
            for i in frange(0, 1, 0.1):
                yield apath.point(i)
        yield -1-1j


# XBee setting
# setupXbee()

# s.write("testj")
# time.sleep(1)
# line = s.read(4)
# print(line)
# print("Connected")
str = sys.argv[1]
mypaths, _ = svg2paths(os.path.abspath(str))
mypaths = mypaths[2:-1:2]
print mypaths
print "------"

# s.write('0 0 j')
# time.sleep(1)
# ack = s.read(4)
# print ack
# s.write('10 0 j')
# time.sleep(1)
# ack = s.read(4)
# print ack
# s.write('10 10 j')
# time.sleep(1)
# ack = s.read(4)
# print ack
# s.write('0 10 j')
# time.sleep(1)
# ack = s.read(4)
# print ack
# s.write('0 0 j')
# time.sleep(1)
# ack = s.read(4)
# print ack
# s.write('-1 -1 j')
# time.sleep(1)
# ack = s.read(4)
# print ack
minx = 100000
miny = 100000
maxy = 0
points = list(getPoints(mypaths))
for i in points:
    x = i.real/10.0
    y = i.imag/10.0
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
    x = i.real/10.0
    y = i.imag/10.0
    print '{0:.4f} {1:.4f} .'.format(x-minx, maxy-y)
    message += '<circle cx="{0:.4f}" cy="{1:.4f}" r="1" stroke="black" stroke-width="0" fill="red" /><text x="{0:.4f}" y="{1:.4f}" fill="black" style="font-size:3px;">{2}</text>'.format(x-minx+10, y-miny+10, idx)
    # s.write('{0:.4f} {1:.4f} j'.format(x-minx, maxy-y))
    # time.sleep(1)
    # ack = s.read(4)
    # print ack
    idx += 1
message += '</svg>'
f.write(message)
f.close()
s.write("endj")