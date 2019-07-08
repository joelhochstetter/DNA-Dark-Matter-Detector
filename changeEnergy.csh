#!/bin/csh
#need to run this script from same folder as topas (executible file). This assumes ./rotateScript.py is in the topas folder
#To run: e.g. source changeEnergy.csh <paramater file path>
#changes energy, runs topas and exports data to file specified by particle type and run number


if ($#argv == 0) then #exists if no topas paramater file supplied
    echo "Error: No topas paramater file supplied"
    exit 0
endif 
set topasParamFile = $1
cp $topasParamFile testFile1.txt


#Enter valid values for following paramaters in topas
#If units are required use quotation marks
set particle = "GenericIon(79,197)"
set historiesinrun = 200
set angle = 260

#replaces lines in parameter file
grep -v "s:Ge/MyCell/Holder/Material" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
grep -v "s:So/Disk/BeamParticle" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
grep -v "i:So/Disk/NumberOfHistoriesInRun" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
grep -v "d:So/Disk/BeamEnergy" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
grep -v ":Gr/ViewA" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
grep -v "Ts/PauseBeforeQuit" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
grep -v "s:Sc/Cluster/OutputFile" testFile1.txt > temp.txt; mv temp.txt testFile1.txt
echo s:So/Disk/BeamParticle = \"$particle\" >> testFile1.txt
echo s:Ge/MyCell/Holder/Material = \"G4_Au\" >> testFile1.txt
echo i:So/Disk/NumberOfHistoriesInRun=$historiesinrun >> testFile1.txt
echo "Ts/PauseBeforeQuit" = \"false\" >> testFile1.txt
echo s:Sc/Cluster/OutputFile = \"t2\" >> testFile1.txt
python ./rotateScript.py $angle testFile1.txt tmp.txt #to use see ./rotateScript.pys
mv tmp.txt testFile1.txt
#echo i:Ts/NumberOfThreads= 8 >> testFile1.txt


#creates folders to store parameter files and scoring output files if an outputFile of same index doesn't already exist
set j = 1
while (-d "outputFiles/$particle/Run$j")
   @ j++
end
mkdir -p testFiles/$particle/Run$j #creates folder for topas parameter files if they do not already exist
mkdir -p outputFiles/$particle/Run$j #creates folder for topas scoring output files if they do not already exist


#foreach x (`seq $minE $Einc $maxE`) #first angle, distance between successive angles, last angle
foreach x (0.01 0.1  0.5 1  5 10 50 100 500)
    cp testFile1.txt testFiles/$particle/Run$j/Energy$x"mev.txt"
    echo d:So/Disk/BeamEnergy = $x "MeV" >> testFiles/$particle/Run$j/Energy$x"mev.txt"
    topas testFiles/$particle/Run$j/Energy$x"mev.txt"
    mv t2.phsp outputFiles/$particle/Run$j/DataEnergy$x"mev.txt"
end
rm testFile1.txt
#EOF
