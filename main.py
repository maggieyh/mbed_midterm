import serial
import sys
import os
import time
from svgpathtools import svg2paths, wsvg, kinks, smoothed_path, path
# w_fac = 960.0/10.0
# h_fac = 720.0/10.0

def frange(beg, end, step):
    i = beg
    while i <= end:
        yield i
        i += step

def setupXbee():
    serdev = '/dev/tty.usbserial-AE019MG3'
    s = serial.Serial(serdev, 9600)

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
    s.write("abcd")
    line = s.read(5)
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

str = sys.argv[1]
mypaths, _ = svg2paths(os.path.abspath(str))
mypaths = mypaths[2:-1:2]
print mypaths
print "------"

for i in getPoints(mypaths):
    x = i.real / 960.0*200.0
    y = i.imag / 720.0*200.0
    print '{0} {1}'.format(x,y)
    # s.write(x+" "+y)

    print x, y

