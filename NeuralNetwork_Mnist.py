import sys
import os
import numpy as np
import getopt
import random
import math
import cPickle
import gzip
import scipy as sp
from scipy.ndimage import interpolation

class NeuralNetwork :

    def __init__(self,layers,eta,gamma):
        self.layers = layers
        self.no_layers = len(layers)
        np.random.seed(1234)
        self.bias = [np.random.randn(y,1) for y in layers[1:]]
        self.W = [np.random.randn(ls2,ls1) for ls1,ls2  in zip(layers[:-1],layers[1:])]
        self.outfunc = self.softmax
        #self.outfunc = self.sigmoid
        self.afuncs = [self.sigmoid] * (self.no_layers - 2)
        #self.afuncs = [self.tanh]*(self.no_layers - 2)
        self.afuncs.append(self.outfunc)
        self.func_grads = [self.sigmoid_grad]*(self.no_layers - 2)
        #self.func_grads = [self.tanh_grad]*(self.no_layers-2)
        #self.out_grad = self.softmax_grad_sse
        self.out_grad = self.softmax_grad_ce
        self.loss_func = self.cross_entrpy_loss
        #self.loss_func = self.squared_loss
        self.gamma = gamma
        self.eta = eta
        self.beta1 = 0.9
        self.beta2  = 0.999
        self.epsilon = 1e-8
        self.mw = [np.zeros((ls2,ls1)) for ls1,ls2  in zip(layers[:-1],layers[1:])]
        self.vw = [np.zeros((ls2,ls1)) for ls1,ls2  in zip(layers[:-1],layers[1:])]
        self.mb = [np.zeros((y,1)) for y in layers[1:]]
        self.vb = [np.zeros((y,1)) for y in layers[1:]]
        self.update_weights = self.vanila_gd
        #self.update_weights = self.momentum_gd
        self.update_weights = self.adam_gd
        #self.update_weights = self.na_gd
        self.lbda = 5
        self.old_W = [w for w in self.W]
        self.old_bias = [b for b in self.bias]
        #self.load_weights = self.load_nag_weights
        self.load_weights = self.load_no_weights
        self.annealing = True
        self.annealing_W = [w for w in self.W]
        self.annealing_bias = [b for b in self.bias]
        self.previous_loss = -1
        #self.out_grad = self.sigmoid_grad_sse



    @staticmethod
    def tanh(inp):
        inp = np.maximum(inp, -100)
        inp = np.minimum(inp, 100)
        return np.tanh(inp)


    @staticmethod
    def tanh_grad(inp):
        return (1-np.power(inp,2))

    @staticmethod
    def sigmoid(inp):
        inp = np.maximum(inp,-100)
        return 1.0/(1.0+np.exp(-inp))

    @staticmethod
    def sigmoid_grad_sse(y,yhat):
        return (yhat-y)*yhat*(1-yhat)

    @staticmethod
    def softmax(inp):
        x = np.amax(inp)
        a = inp-x
        a = np.exp(a)
        return a/np.sum(a)

    @staticmethod
    def softmax_grad_sse(y,yhat):
        return np.dot((np.identity(len(y))-yhat)*(yhat-y).transpose(),yhat)

    @staticmethod
    def softmax_grad_ce(y,yhat):
        return (yhat-y)

    @staticmethod
    def sigmoid_grad(inp) :
        return inp*(1-inp)

    @staticmethod
    def squared_loss(trv,prv):
        return np.sum(np.power(trv-prv,2))

    @staticmethod
    def cross_entrpy_loss(trv,prv):
        return -np.sum(trv*np.log(prv))


    def back_prop(self,input,output):
        z = input
        a = [z]
        for i in xrange(self.no_layers-1) :
            z = self.afuncs[i](np.dot(self.W[i],z)+self.bias[i])
            a.append(z)
        delta_w = [np.ones(w.shape) for w in self.W]
        delta_b = [np.zeros(b.shape) for b in self.bias]
        local_gradients = self.out_grad(output,z)
        for i in xrange(self.no_layers-2,-1,-1) :
            delta_b[i] = local_gradients
            delta_w[i] = delta_w[i]*local_gradients*np.transpose(a[i])
            if i!=0 :
                local_gradients = np.dot(local_gradients.transpose(),self.W[i]).transpose()*self.func_grads[i-1](a[i])
        return (delta_b,delta_w)

    def vanila_gd(self,delta_w,delta_b):
        for i in xrange(len(self.W)) :
            self.W[i] -= self.eta*delta_w[i]
            self.bias[i] -= self.eta*delta_b[i]

    def momentum_gd(self,delta_w,delta_b):
        for i in xrange(len(self.W)) :
            self.mw[i] = self.eta*delta_w[i]+self.gamma*self.mw[i]
            self.mb[i] = self.eta*delta_b[i]+self.gamma*self.mb[i]
            self.W[i] -= self.mw[i]
            self.bias[i] -= self.mb[i]

    def adam_gd(self,delta_w,delta_b):
        for i in xrange(len(self.W)) :
            mwp = np.copy(self.mw[i])
            vwp = np.copy(self.vw[i])
            mbp = np.copy(self.mb[i])
            vbp = np.copy(self.vb[i])
            self.mw[i] = self.beta1*mwp+(1-self.beta1)*delta_w[i]
            self.vw[i] = self.beta2*vwp+(1-self.beta2)*np.power(delta_w[i],2)
            self.mb[i] = self.beta1*mbp+(1-self.beta1)*delta_b[i]
            self.vb[i] = self.beta2*vbp+(1-self.beta2)*np.power(delta_b[i],2)
            self.W[i] -= (self.eta/(1-self.beta1))*self.mw[i]/np.sqrt(self.vw[i]/(1-self.beta2)+self.epsilon)
            self.bias[i] -= (self.eta/(1-self.beta1))*self.mb[i]/np.sqrt(self.vb[i]/(1 - self.beta2)+self.epsilon)

    def load_no_weights(self):
        pass

    def load_nag_weights(self):
        for i in xrange(len(self.W)) :
            np.copyto(self.old_W[i],self.W[i])
            np.copyto(self.old_bias[i],self.bias[i])
            self.W[i] -= self.gamma*self.mw[i]
            self.bias[i] -= self.gamma*self.mb[i]

    def na_gd(self,delta_w,delta_b):
        for i in xrange(len(self.W)) :
            self.mw[i] = self.eta*delta_w[i] + self.gamma*self.mw[i]
            self.mb[i] = self.eta*delta_b[i] + self.gamma*self.mb[i]
            self.W[i] = self.old_W[i] - self.mw[i]
            self.bias[i] = self.old_bias[i] - self.mb[i]


    def forward_pass(self,inp):
        z = inp
        for i in xrange(self.no_layers - 1):
            z = self.afuncs[i](np.dot(self.W[i], z) + self.bias[i])
        return z

    def evaluate_loss(self,data) :
        loss = 0.0
        for tr,y in data :
            yhat = self.forward_pass(tr)
            loss = loss + self.loss_func(y,yhat)
        return loss

    def validate(self,valid_data) :
        accuracy = 0.0
        predicted = self.predict(valid_data[0])
        accuracy = float(sum([int(x==y) for x,y in zip(predicted,list(valid_data[1]))]))/len(valid_data[0])
        return accuracy




    def train(self,train_data,max_epochs,batch_size,valid_data=None):
        total_data = len(train_data)
        print total_data
        for i in xrange(max_epochs) :
            np.random.shuffle(train_data)
            mini_batches = [train_data[k:k + batch_size] for k in xrange(0,total_data,batch_size)]
            for batch in mini_batches :
                delta_w = [np.zeros(w.shape) for w in self.W]
                delta_bias = [np.zeros(b.shape) for b in self.bias]
                self.load_weights()
                for tr,outp in batch :
                    d_b,d_w = self.back_prop(tr,outp)
                    delta_bias = [db1+db2 for db1,db2 in zip(delta_bias,d_b)]
                    delta_w = [dw1+dw2 for dw1,dw2 in zip(delta_w,d_w)]
                delta_w = [(dw+w*self.lbda/total_data)/len(batch) for dw,w in zip(delta_w,self.W)]
                delta_bias = [b /len(batch) for b in delta_bias]
                self.update_weights(delta_w,delta_bias)
            print "end of %d epoch .."%i
            print "loss function value : %f"%self.evaluate_loss(train_data)
            if valid_data :
                print "accuracy for validation : %f"%self.validate(valid_data)
            if i%5 == 0 :
                self.eta *= 0.5




    def predict(self,test_data,classify=True):
        output = []
        for t in test_data :
            z = self.forward_pass(t)
            if classify :
                z = np.argmax(z)
            output.append(z.transpose())
        return output




def moments(image):
    c0,c1 = np.mgrid[:image.shape[0],:image.shape[1]] # A trick in numPy to create a mesh grid
    totalImage = np.sum(image) #sum of pixels
    m0 = np.sum(c0*image)/totalImage #mu_x
    m1 = np.sum(c1*image)/totalImage #mu_y
    m00 = np.sum((c0-m0)**2*image)/totalImage #var(x)
    m11 = np.sum((c1-m1)**2*image)/totalImage #var(y)
    m01 = np.sum((c0-m0)*(c1-m1)*image)/totalImage #covariance(x,y)
    mu_vector = np.array([m0,m1]) # Notice that these are \mu_x, \mu_y respectively
    covariance_matrix = np.array([[m00,m01],[m01,m11]]) # Do you see a similarity between the covariance matrix
    return mu_vector, covariance_matrix


def deskew(image):
    c,v = moments(image)
    alpha = v[0,1]/v[0,0]
    affine = np.array([[1,0],[alpha,1]])
    ocenter = np.array(image.shape)/2.0
    offset = c-np.dot(affine,ocenter)
    return interpolation.affine_transform(image,affine,offset=offset)



def make_one_hot(data,n) :
    a = []
    for i in data :
        t = np.zeros([n,1])
        t[i] = 1.0
        a.append(t)
    return a


def format_input(file_name) :
    f = gzip.open(file_name, 'rb')
    train_set, valid_set, test_set = cPickle.load(f)
    train_data = ([np.reshape(x,(len(x),1)) for x in train_set[0]],make_one_hot(train_set[1],10))
    test_data = ([np.reshape(x,(len(x),1)) for x in test_set[0]],test_set[1])
    valid_data = ([np.reshape(x,(len(x), 1)) for x in valid_set[0]], valid_set[1])
    train_data = ([deskew(x.reshape(28,28)).reshape(784,1) for x in train_data[0]],train_data[1])
    valid_data = ([deskew(x.reshape(28,28)).reshape(784, 1) for x in valid_data[0]],valid_data[1])
    test_data = ([deskew(x.reshape(28,28)).reshape(784, 1) for x in test_data[0]],test_data[1])
    f.close()
    return (train_data,valid_data,test_data)

if __name__ == '__main__' :
    train_data,valid_data,test_data = format_input(sys.argv[1])
    clf = NeuralNetwork([784,100,10],0.01,0.1)
    train_data = [np.array([x,y]) for x,y in zip(train_data[0],train_data[1])]
    clf.train(train_data,30,30,valid_data)
    predicted = clf.predict(test_data[0])
    predicted = list(np.transpose(predicted))
    #print predicted
    #print len(list(test_data[1]))
    accuracy = float(sum([int(x==y) for x,y in zip(predicted,list(test_data[1]))]))/len(predicted)
    print "test data accuracy : %f"%accuracy


