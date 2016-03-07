'''
Created on Feb 21, 2015

@author: Abdulaziz Alghamdi
'''

import sys, getopt
from os import listdir, path
import time
import pylab

import numpy 
import matplotlib.pyplot as plt

def plot(name, data, number_of_passes):
#     fig = plt.figure(name)
#     line, = plt.plot(data, linewidth=2)
#  
#     plt.show()

    #Calculating the split line location
    split = len(data)/number_of_passes
    
    pylab.figure(name)
    pylab.plot(data, label=name)
    for i in xrange(0, number_of_passes):
        pylab.axvline(x=split*i, linewidth=1, color='k')
    pylab.xlabel('Iterations')
    pylab.ylabel('Cost')
    pylab.title(name)
    pylab.show(block=True)

    return    


# def graph(name):
# #     fig = plt.figure(name)
# #     line, = plt.plot(data, linewidth=2)
# #  
# #     plt.show()
# #     pylab.figure(name)
#     pylab.ion()
#     hl, = pylab.plot([])
#     pylab.draw()
# 
#     return hl
#    
# def update_line(hl, new_data):
#     hl.set_xdata(numpy.append(hl.get_xdata(), [5]))
#     hl.set_ydata(numpy.append(hl.get_ydata(), [6]))
#     pylab.draw()

