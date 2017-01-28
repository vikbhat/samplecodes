import math
import os
import sys
import random


alpha = float(10**6)
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


J = 10**8
minJ = 10**8
arr = [random.randint(1000,5000) for x in range(100)]
minx = max(arr)
xp = max(arr)
learn_rate = 0.1

for x in range(10000) :
    J1 = 0.0
    for y in arr :
        J1 += abs(y-xp)

    if J1<minJ :
        minJ = J1
        minx = xp

    diffx = 0.0
    for y in arr :
        sx1 = sigmoid(float(y-xp),alpha)
        sx2 = sigmoid(float(xp-y),alpha)
        diffx += -sx1 - alpha*(y-xp)*sx1*(1-sx1) + sx2 + alpha*(xp-y)*sx2*(1-sx2)

    xp = xp-learn_rate*diffx

print "median is : ",minx
arr.sort()
print "actual median is : ",arr[len(arr)/2]

