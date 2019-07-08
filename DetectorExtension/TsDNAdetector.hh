
#ifndef TsDNAdetector_hh
#define TsDNAdetector_hh

#include "TsVGeometryComponent.hh"


class TsDNAdetector : public TsVGeometryComponent
{    
public:
	TsDNAdetector(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
				  TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name);
	~TsDNAdetector();
	
	G4VPhysicalVolume* Construct();
    
  
};

#endif
