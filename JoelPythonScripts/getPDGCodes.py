#has functions to get a list of all the particle codes in a given topas scorer file
#to use "import getPDGCodes.py", else uncomment last line of file to run as a script


import sys
import matplotlib.pyplot as plt
import numpy as np

location=sys.argv[1]

def getPDGCodes(location):
    ls = []
    dep = []
    with open(location, 'r') as f:
            for line in f:
                row=line.split()
                ls.append(int(row[5]))
                dep.append(float(row[4]))
    return list(set(ls))

#getPDGCodes(location) 
