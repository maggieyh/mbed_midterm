from svgpathtools import svg2paths, wsvg
import numpy as np 
SAMPLES_PER_PX = 1

fname = "./ccc.svg"
paths, attributes = svg2paths(fname)

myPaths = {}
for path,attr in zip(paths, attributes):
    myPathList = []
    pathLength = path.length()
    numSamples = int(pathLength * SAMPLES_PER_PX)
    for i in range(numSamples):
        #parametric length = ilength(geometric length)
        myPathList.append(path.point(path.ilength(pathLength * i / (numSamples-1))))
    print(np.array(myPathList))