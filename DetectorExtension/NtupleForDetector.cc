// Scorer for NtupleForDetector

//An example ntuple scorer for the DNA circular plasmid geometry.
//File may be edited to score different quantities.

#include "NtupleForDetector.hh"

#include "G4PSDirectionFlag.hh"

#include "TsVGeometryComponent.hh"
//#include "TsGeometryManager.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UIcommand.hh"
#include "G4Tokenizer.hh"
#include "G4SystemOfUnits.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

NtupleForDetector::NtupleForDetector(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM,
                                 G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer)
: TsVNtupleScorer(pM, mM, gM, scM, eM, scorerName, quantity, outFileName, isSubScorer)
{    //Define ntuple columns
    
    //General quantities
    fNtuple->RegisterColumnF(&fPosX, "Position X", "cm");
    fNtuple->RegisterColumnF(&fPosY, "Position Y", "cm");
    fNtuple->RegisterColumnF(&fPosZ, "Position Z", "cm");
    fNtuple->RegisterColumnF(&fEnergy, "Energy", "MeV");
    fNtuple->RegisterColumnF(&fEnergyDep, "Energy Deposited", "MeV");
    fNtuple->RegisterColumnI(&fParticleType, "Particle Type (in PDG Format)");
    fNtuple->RegisterColumnS(&fProcess, "Process Type");
    fNtuple->RegisterColumnI(&fEventID, "Event ID");
    fNtuple->RegisterColumnI(&fTrackID, "Track ID");
    
    //Specific to the detector
    fNtuple->RegisterColumnI(&fStrandID, "StrandID");
    fNtuple->RegisterColumnS(&fVolume, "Volume");
    fNtuple->RegisterColumnI(&fRepNum, "RepNum");
 
}


NtupleForDetector::~NtupleForDetector() {;}


G4bool NtupleForDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{   
    if (!fIsActive) {
        fSkippedWhileInactive++;
        return false;
    }
    ResolveSolid(aStep);
    
    G4double minEtoScore;
    G4String name = GetFullParmName("minEtoScore");
    if (!fPm->ParameterExists(name)) {
        minEtoScore = 10*eV;
    } else {
        minEtoScore = fPm->GetDoubleParameter(name, "Energy");
    }
    //G4cout << minEtoScore << G4endl;

    fEnergyDep      = aStep->GetTotalEnergyDeposit();
    
    G4StepPoint* theStepPoint=aStep->GetTrack()->GetStep()->GetPreStepPoint();
    G4TouchableHandle theTouchable = theStepPoint->GetTouchableHandle();
    G4String volumeName = theTouchable->GetVolume()->GetName();   
    G4double volumeNumb = theTouchable->GetReplicaNumber();
    
    if (fEnergyDep >= minEtoScore) { 

        //Get position
        G4ThreeVector pos = theStepPoint->GetPosition();
        fPosX = pos.x();
        fPosY = pos.y();
        fPosZ = pos.z();
        
        //G4cout << volumeName1 << " " << volumeName1.substr(10,20) << G4endl;
        if ((volumeName == "BP")||(volumeName == "sugar1")||(volumeName == "sugar2")) {
        	G4String volumeName1 = theTouchable->GetVolume(1)->GetName();
        	G4String strand = volumeName1.substr(10,20);
			fStrandID = atoi(strand.c_str());
        } else {
			fStrandID = -1;
		}
      
        //Get kinetic energy
        fEnergy = theStepPoint->GetKineticEnergy();
        
        //Get energy deposition
        fEnergyDep = aStep->GetTrack()->GetStep()->GetTotalEnergyDeposit();
        
        //Get particle type
        fParticleType = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
        
        //Get Volume name and information
		
		if ((volumeName == "MyCell" && fPosX <= 50e-7 && fPosX >= -50e-7 && fPosZ>=-50e-7 && fPosZ<=50e-7 && fPosY>=335e-7 && fPosY<=345e-7)
		 || volumeName == "MyCell/Holder") {
    		volumeName = "Holder";
		}        
		fVolume = volumeName;
        fRepNum = volumeNumb;
        
		G4String processName;
		processName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
		fProcess = processName;  
        // Get IDs
        fTrackID = aStep->GetTrack()->GetTrackID();
        fRunID   = GetRunID();
        fEventID = GetEventID();
        
        fNtuple->Fill();
        return true;
    }
    return false;   
}
