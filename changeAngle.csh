#!/bin/csh
#need to run this script from same folder as topas (executible file). This assumes ./rotateScript.py is in the topas folder
#To run: e.g. source changeAngle.csh <paramater file path>
#changes angle, runs topas and exports data

set minAng = 270
set maxAng = 180
set Anginc = -10

if ($#argv == 0) then #exists if no topas paramater file supplied
    echo "Error: No topas paramater file supplied"
    exit 0
endif 
set topasParamFile = $1
cp $topasParamFile testFile.txt


#Enter valid values for following paramaters in topas
#If units are required use quotation marks
set particle = GenericIon(7, 14)
set historiesinrun = 500
set beamE = 0.1

#replaces lines in parameter file
grep -v "s:So/Disk/BeamParticle" testFile.txt > temp.txt; mv temp.txt testFile.txt
grep -v "i:So/Disk/NumberOfHistoriesInRun" testFile.txt > temp.txt; mv temp.txt testFile.txt
grep -v "d:So/Disk/BeamEnergy" testFile.txt > temp.txt; mv temp.txt testFile.txt
grep -v "d:Ge/BeamPosition/Trans" testFile.txt > temp.txt; mv temp.txt testFile.txt
grep -v "d:Ge/BeamPosition/Rot" testFile.txt > temp.txt; mv temp.txt testFile.txt
grep -v ":Gr/ViewA" testFile.txt > temp.txt; mv temp.txt testFile.txt
grep -v "Ts/PauseBeforeQuit" testFile.txt > temp.txt; mv temp.txt testFile.txt
echo s:So/Disk/BeamParticle = \"$particle\" >> testFile.txt
echo i:So/Disk/NumberOfHistoriesInRun=$historiesinrun >> testFile.txt
echo d:So/Disk/BeamEnergy = $beamE "MeV" >> testFile.txt
echo "Ts/PauseBeforeQuit" = \"false\" >> testFile.txt
#echo i:Ts/NumberOfThreads= -5 >> testFile.txt

#creates folders to store parameter files and scoring output files if an outputFile of same index doesn't already exist
set j = 1
while (-d "outputFiles/$particle/Run$j")
   @ j++
end
mkdir -p testFiles/$particle/Run$j #creates folder for topas parameter files if they do not already exist
mkdir -p outputFiles/$particle/Run$j #creates folder for topas scoring output files if they do not already exist


foreach x (`seq $minAng $Anginc $maxAng`) #first angle, distance between successive angles, last angle
    python ./rotateScript.py $x testFile.txt testFiles/$particle/Run$j/indegrees$x.txt #to use see ./rotateScript.pys
    topas testFiles/$particle/Run$j/indegrees$x.txt
    mv ./NAtDetector.phsp outputFiles/$particle/Run$j/outdegrees$x.txt
end
rm testFile.txt
#EOF
