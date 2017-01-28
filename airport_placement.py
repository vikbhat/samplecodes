import os
import math
import sys
import random
import matplotlib.pyplot as plt

alpha = float(10**8)
print alpha

def sigmoid(x,alpha) :
    try :
        r = 1.0/(1+math.pow(math.e,float(-alpha*x)))
        return r
    except OverflowError,e :
        if x<0 :
            return 0.0
        return 1.0
    return 0.0


arr = [[random.randint(0,1000),random.randint(0,1000)] for x in range(100)]
aiports = [[random.randint(0,10000),random.randint(0,10000)] for x in range(3)]

iterations = 1000
J = 10**8
minJ = 10**8
minA = airports[:]
for i in range(iterations) :
    J1 = 0
    for x in arr :
        mind = 10**8
        for y in airports :
            dist = math.sqrt(math.pow((x[0]-y[0]),2)+math.pow((x[1]-y[1]),2))
            if dist < mind :
                mind = dist
        J1 += mind
    if J1 < minJ :
        minJ = J1
        minA = airports[:]
    

    
