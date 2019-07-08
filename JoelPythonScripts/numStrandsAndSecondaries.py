#This file takes in a directory of our scored files and outputs for the primary and each secondary the average, std, min and max 
#number of strands that a unique particle interacts with as well as number of unique secondaries produced that go onto interact with the DNA
#Writes this data to a file StrandInfo1.dat in the same directory that you use
#command to run: python numStrandsAndSecondaries.py <directory file path to use>

import os
import sys
import numpy as np
import statistics 

directory=sys.argv[1]


def main(directory):
    data = [] #each element is [filename, pdg code, average num strands, std] - could modify for total energy deposition also
    directory = directory + '/'
    partDict = {}
    for filename in os.listdir(directory):
        if filename.endswith('.txt') or filename.endswith('.phsp'):
            fullName = directory + filename
            for loc in ['Holder', 'DNA']:
                partDict = getStrandData(fullName, loc)
                for partID in partDict.keys():
                    sd = partDict.get(partID)
                    data.append([filename, loc, partID, sd[0], sd[1], sd[2], sd[3], sd[4], sd[5], sd[6], sd[7]])
    with open(directory + 'StrandInfo1.dat', 'w') as f:
        f.write('filename loc pdgcode nsmean nsstd nsmin nsmax nparmean nparstd nparmin nparmax\n')
        for el in data:
            f.write(el[0] + '    ' +  str(el[1])+ '    ' +  str(el[2])+ '    ' +  str(el[3])+ ' ' + str(el[4]) + ' ' + str(el[5]) + '    ' +  str(el[6])+ '    ' +  str(el[7])+ '    ' +  str(el[8]) + ' ' + str(el[9]) + ' ' + str(el[10])  +'\n')
    return data

                    
def getStrandData(fullName, loc): #partID = 0 means any particle
    partDictEvents = {}
    statDict = {}
    with open(fullName, 'r') as f:
        for line in f:
            row = line.split()
            if (loc == row[10] and loc == 'Holder') or (row[10] != 'Holder' and loc == 'DNA'):
                pdg = int(row[5])
                event = int(row[7])
                track = int(row[8])
                strand = int(row[9])
                if not pdg in partDictEvents.keys():
                    partDictEvents[pdg] = set()
                partDictEvents[pdg].add((event,track,strand)) #adds a tuple if not already in set
    for partID in partDictEvents.keys(): 
        subDict = {}
        eventDict = {}
        numStrands = []
        numSec = []
        for event, track, strand in partDictEvents.get(partID):
            if not (event, track) in subDict.keys():
                subDict[(event, track)] = set() #stores number of strands and set of strands
            subDict[(event, track)].add(strand) #adds a tuple if not already in set
            if not event in eventDict.keys():
                eventDict[event] = set()
            eventDict[event].add(track) #adds a tuple if not already in set          
        for x in subDict:
            numStrands.append(len(subDict.get(x)))
        for y in eventDict:
            numSec.append(len(eventDict.get(y))-1)       
        if len(numStrands) > 0:
            nsmean = sum(numStrands)/float(len(numStrands))
            nsstd = statistics.stdev(numStrands)
            nsmaxi = max(numStrands)
            nsmini = min(numStrands)
        else:
            nsmean = 0.0 
            nsmaxi = 0.0
            nsstd = 0.0
            nsmini = 0.0
        if len(numSec) > 0:
            nspmean = sum(numSec)/float(len(numSec))
            nspstd = statistics.stdev(numSec)
            nspmaxi = max(numSec)
            nspmini = min(numSec)
        else:
            nspmean = 0.0 
            nspmaxi = 0.0
            nspstd = 0.0
            nspmini = 0.0
        statDict[partID] = [nsmean, nsstd, nsmini, nsmaxi, nspmean, nspstd, nspmini, nspmaxi]
    return statDict
    
    
main(directory)
