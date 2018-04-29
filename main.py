import serial
import sys
import time
from svgpathtools import svg2paths, wsvg, kinks, smoothed_path
def frange(beg, end, step):
    i = beg
    while i <= end:
        yield i
        i += step
# XBee setting
str = sys.argv[1]
paths, _ = svg2paths(str)
paths = paths[2:-1:2]
print paths
print "------"
for i in frange(0, 1, 0.2):
    print paths[0].point(i)
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

    # send to remote
    s.write("abcd")
    line = s.read(5)
    print(line)

    s.close()


# setupXbee()


