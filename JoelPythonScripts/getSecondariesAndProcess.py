#Gets a list of all secondaries particles and their attached processes in a given directory and its subdirectories of scored files
#usage: python getSecondariesAndProcess.py <directory name>


import sys
import os

directory=sys.argv[1]

def main(root):
    ls = []
    for path, subdirs, files in os.walk(root):
        for name in files:
            if name.endswith('.txt'):
                ls = getPDGProcess(os.path.join(path, name), ls)
    with open(os.path.join(root, 'partAndProcess.data'), 'w') as f:
        f.write('Particle' + 'Process' +'\n')
        for el in ls:
            f.write(el + '\n')
    return


        


def getPDGProcess(location, ls):
    with open(location, 'r') as f:
            for line in f:
                row=line.split()
                newEl = str(row[5])+ ' ' + str(row[6]) + ' ' + str(row[10])
                if not newEl in ls:
                    ls.append(newEl) #particle and process
    return ls
    
    
main(directory)
