#This function will take a folder of outputs from NtupleForDetectorScorer and can plot a chosen column from each file against the independent variable
#Produces separate plots for events in DNA and holder separately
# Usage
# python ./deposits.py <directory name> <column to plot data> <select by particle>
# e.g. python ./depotisextractDataByColumn.py ./files 4 -1
#particle code of -1 plots for all particles in file (use this)
# This script assumes that the data file does not have column headers.

import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re
from getPDGCodes import getPDGCodes

directory=sys.argv[1]
column = int(sys.argv[2]) #4 plots energy deposits
partID = int(sys.argv[3]) #0 means plot for all particles, -1 means plot all particles separately

def plotEvents(directory, column, partID):
    pdg = []
    directory = directory + '/'
    if partID == 0: #plot for all particles
        recoilDirectory = directory
        pdg.append('/')
    else:
        if partID == -1: #plot for all particles separately
            for filename in os.listdir(directory):
                if filename.endswith('.txt') or filename.endswith('.phsp'):
                    fullName = directory + filename
                    pdg.extend(list(getPDGCodes(fullName)))
                    pdg = list(set(pdg))
        else:
            pdg.append(partID)
        recoilDirectory = directory + 'recoils/'
        if not os.path.exists(recoilDirectory):
            os.makedirs(recoilDirectory)
        for filename in os.listdir(directory):
            for part in pdg:
                if filename.endswith('.txt') or filename.endswith('.phsp'):
                    fullName = directory + filename
                    if not os.path.exists(recoilDirectory + str(part) + '/DNA/'):
                        os.makedirs(recoilDirectory + str(part) + '/DNA/')
                    newLocation = recoilDirectory + str(part) + '/deposit' + filename
                    extractByType(5, int(part), fullName, newLocation)
                    
                    DNALocation = recoilDirectory + str(part) + '/DNA/deposit' + filename
                    extractByType(10, 'Holder', newLocation, DNALocation, 'not')
                    
                    if not os.path.exists(recoilDirectory + str(part) + '/Holder/'):
                        os.makedirs(recoilDirectory + str(part) + '/Holder/')
                    HolderLocation = recoilDirectory + str(part) + '/Holder/deposit' + filename
                    extractByType(10, 'Holder', newLocation, HolderLocation)                                       
    dnaDict = {}
    holDict = {}
    allDict = {}
    
    for part in pdg:
        partDir = recoilDirectory + str(part)
        l = extractDataByColumn(partDir, column)
        if part == '/':
            part = 'all'
        else:
            
            ldna = extractDataByColumn(partDir + '/DNA/', column)
            plotEachRecoil(ldna, str(part) + 'dna')
            plotCounts(ldna, str(part) + 'dna')
            plotMeanErrorBars(ldna, str(part) + 'dna')
            
            lhol = extractDataByColumn(partDir + '/Holder/', column)
            plotEachRecoil(lhol, str(part) + 'hol')
            plotCounts(lhol, str(part) + 'hol')
            plotMeanErrorBars(lhol, str(part) + 'hol')
            
        plotEachRecoil(l, str(part))
        plotCounts(l, str(part))
        plotMeanErrorBars(l, str(part))
        dnaDict[part] = [ldna[2], ldna[4], ldna[5]]
        holDict[part] = [lhol[2], lhol[4], lhol[5]]
        allDict[part] = [l[2], l[4], l[5]]
    myplot(dnaDict, 'dna')
    myplot(holDict, 'hol')
    myplot(allDict, 'all')
        
       

def extractDataByColumn(directory, column):
    indepList = [] #Value of independent variables e.g.energy if we are changing energy
    meanList = []
    sdList = []
    countList = []
    indVarList = []
    dpList = [] #lists all datapoints for a given independent variable
    directory = directory + '/'
    for filename in os.listdir(directory):
        if filename.endswith('.txt') or filename.endswith('.phsp'):
            fullName = directory + filename
            l = listOfData(fullName, column)
            dpList.append(l)
            countList.append(len(l))
            if len(l) > 0:                
                meanList.append(np.mean(l))
                sdList.append(np.std(l))
            else:
                meanList.append(0)
                sdList.append(0)
            indVarList = getIndVar(filename)
            indepList.append(indVarList[1])        
    if dpList == []:
        return [0,0,0,0,0,0,0]
    else:
        return [indVarList[0], indVarList[2], indepList, countList, meanList, sdList, dpList]     

    
#extracts list of all elements in a column. Column numbers start from 1
def listOfData(fileName, col):
    with open(fileName, 'r') as f:
        myList=[]
        for line in f:
            row=line.split()
            myList.append(float(row[col]))   
    return myList;
    

def getIndVar(s): #returns name of independent variable, numerical value and units from the filename
    indVar = []
    l = filter(None, re.split(r'(\d+)', s))
    #print l
    indVar.append(l[0])
    if l[2] == '.':
        indVar.append(float(l[1]+l[2]+l[3]))
        if l[4] != '.txt':
          indVar.append(l[4])
        else:
            indVar.append(' ')
    else:
        indVar.append(float(l[1]))
        if l[2] != '.txt':
          indVar.append(l[2]) 
        else:
            indVar.append(' ')
    return indVar
    
def extractByType(col, val, oldData, newData, *param):
    with open(oldData, 'r') as old:
        with open (newData, 'w') as new:
            for line in old:
                row=line.split()
                if 'not' in param:
                    if str(row[col]) != str(val): ##row[5](6th column of data corresponds to particle id.
                        new.write(' '.join(map(str,row))+"\n")                
                else:
                    if str(row[col]) == str(val): ##row[5](6th column of data corresponds to particle id.
                        new.write(' '.join(map(str,row))+"\n")

def plotEachRecoil(dataList, plotName, *param):
    l = dataList
    x = []
    y =[]
    for i in l[6]:
        for j in i:
            x.append(l[2][l[6].index(i)])
            y.append(j)
    if x != [] and y != []:
        print plotName + str(np.mean(y)) + ' sd: ' + str(np.std(y))
        plt.scatter(x, y)
        plt.ylabel('Energy of Recoil (MeV)')
        plt.xlabel('%s (%s)' % (l[0], l[1]))
        if ('logx' in param):
            plt.xscale('log')
        if ('logy' in param):
            plt.yscale('log')
        #plt.show()
        if not os.path.exists(directory +'/plots/'):
            os.makedirs(directory +'/plots/')                
        plt.savefig(directory + '/plots/eachDeposit' + plotName + '.png')
        plt.close('all')
    
def plotMeanErrorBars(dataList, plotName, *param):
    l = dataList
    plt.errorbar(l[2],l[4], yerr=l[5],fmt='.')
    if ('logx' in param):
        plt.xscale('log')
    if ('logy' in param):
        plt.yscale('log')
    plt.ylabel('Energy of Recoil (MeV)')
    plt.xlabel('%s (%s)' % (l[0], l[1]))
    #plt.show()
    if not os.path.exists(directory +'/plots/'):
        os.makedirs(directory +'/plots/')  
    plt.savefig(directory + '/plots/meanwithErr' + plotName + '.png')
    plt.close('all')

def plotCounts(dataList, plotName, *param):
    l = dataList
    plt.scatter(l[2], l[3])
    if ('logx' in param):
        plt.xscale('log')
    if ('logy' in param):
        plt.yscale('log')
    plt.ylabel('Number of recoils')
    plt.xlabel('%s (%s)' % (l[0], l[1]))
    #plt.show()
    if not os.path.exists(directory +'/plots/'):
        os.makedirs(directory +'/plots/')  
    plt.savefig(directory + '/plots/counts' + plotName + '.png')
    plt.close('all')

#Plot deposits all on same graph    
#Add or remove lines for secondary particles that you want to display on the plot   
def myplot(data, name):
        f, ax = plt.subplots(1)
        ax.errorbar(data[1000791970][0],data[1000791970][1],xerr=None,yerr=data[1000791970][2],fmt='v',c='y',label='Au-197 Nucleus') 
        ax.errorbar(data[11][0],data[11][1],xerr=None,yerr=data[11][2],fmt='o',c='b',label='Electron')
        ax.errorbar(data[2212][0],data[2212][1],xerr=None,yerr=data[2212][2],fmt='D',c='r',label='Proton')
        #ax.errorbar(data[2112][0],data[2112][1],xerr=None,yerr=data[2112][2],fmt='s',c='g',label='Neutron')
        ax.errorbar(data[1000060120][0],data[1000060120][1],xerr=None,yerr=data[1000060120][2],fmt='*',c='c',label='C-12 nucleus')
        ax.errorbar(data[1000070140][0],data[1000070140][1],xerr=None,yerr=data[1000070140][2],fmt='h',c='m',label='N-14 nucleus')
        ax.errorbar(data[1000020040][0],data[1000020040][1],xerr=None,yerr=data[1000020040][2],fmt='D',c='k',label='Alpha')
        ax.errorbar(data[1000791980][0],data[1000791980][1],xerr=None,yerr=data[1000791980][2],fmt='^',c='g',label='Au-198 Nucleus')
   
        # plot scaling/labelling and saving.
        plt.xscale('log')
        plt.yscale('log')
        plt.xlabel('Incident Beam Energy (MeV)')
        plt.ylabel('Energy deposits (MeV)')
        ax.set_xlim(9e-3,12)
        f.savefig('plot' + name + '.png', bbox_inches='tight')


plotEvents(directory, column, partID)
