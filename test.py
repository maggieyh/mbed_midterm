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

str = sys.argv[1]
mypaths, _ = svg2paths(os.path.abspath(str))
mypaths = mypaths[2:-1:2]
print mypaths
print "------"

minx = 100000
miny = 100000
maxy = 0
points = list(getPoints(mypaths))
for i in points:
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
    x = i.real/20.0
    y = i.imag/20.0
    print '{0:.4f} {1:.4f} .'.format(x-minx, maxy-y)
    message += '<circle cx="{0:.4f}" cy="{1:.4f}" r="1" stroke="black" stroke-width="0" fill="red" /><text x="{0:.4f}" y="{1:.4f}" fill="black" style="font-size:3px;">{2}</text>'.format(x-minx+10, y-miny+10, idx)
    idx += 1
message += '</svg>'
f.write(message)
f.close()