
#ifndef NtupleForDetector_hh
#define NtupleForDetector_hh

#include "TsVNtupleScorer.hh"

#include "G4VProcess.hh"

class NtupleForDetector : public TsVNtupleScorer
{
public:
    NtupleForDetector(TsParameterManager* pM, TsMaterialManager* mM, TsGeometryManager* gM, TsScoringManager* scM, TsExtensionManager* eM,
                G4String scorerName, G4String quantity, G4String outFileName, G4bool isSubScorer);
    
    virtual ~NtupleForDetector();

    G4bool ProcessHits(G4Step*,G4TouchableHistory*);

protected:
    // Output variables
    G4float fPosX;
    G4float fPosY;
    G4float fPosZ;
    G4float fEnergy;
    G4float fEnergyDep;
    G4int fParticleType;
    G4String fProcess;
    G4int fTrackID;
    G4String fVolume;
    G4int fRepNum;

    G4int fRunID;
    G4int fEventID;
    
    G4int fStrandID;
    G4int flagProcess;
};
#endif
