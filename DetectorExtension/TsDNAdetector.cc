// Component for TsDNAdetector

// Builds Particle Detector with a rectangular prism holder and regularly spaced DNA Strands arranged in an array hanging down
//Made Holder dimensions and DNA spacing parameters for paramater file. DNA is spaced evenly over entire size of holder

/* Example Use in paramater files:
s:Ge/MyCell/Type="tsdnadetector"
s:Ge/MyCell/Parent="World"
s:Ge/MyCell/Material="vacuum"

s:Ge/MyCell/Holder/Material="G4_Au"
s:Ge/MyCell/Holder/Color="yellow"
b:Ge/MyCell/Cyl/Invisible="true"
s:Ge/MyCell/Cyl/Material="vacuum"             

s:Ge/MyCell/BP/Material="G4_DNA_ADENINE"
s:Ge/MyCell/BP/Color="grey220"
s:Ge/MyCell/BP/DrawingStyle="solid"

#Setting the material of sugar1 and sugar2
s:Ge/MyCell/Sugar1/Color="Olive"
s:Ge/MyCell/Sugar1/Material="G4_Water"

s:Ge/MyCell/Sugar2/Color="Red"
s:Ge/MyCell/Sugar2/Material="G4_Water"

d:Ge/MyCell/DNASpacing=25 nm 
d:Ge/MyCell/HolderHLX=100 nm
d:Ge/MyCell/HolderHLY=25 nm
d:Ge/MyCell/HolderHThickness=3 nm
*/

#include "TsDNAdetector.hh"

#include "TsParameterManager.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4Orb.hh"
#include "G4Ellipsoid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"

#include "G4VisAttributes.hh"

TsDNAdetector::TsDNAdetector(TsParameterManager* pM, TsExtensionManager* eM, TsMaterialManager* mM, TsGeometryManager* gM,
			 TsVGeometryComponent* parentComponent, G4VPhysicalVolume* parentVolume, G4String& name) :
TsVGeometryComponent(pM, eM, mM, gM, parentComponent, parentVolume, name)
{
}


TsDNAdetector::~TsDNAdetector()
{
}


G4VPhysicalVolume* TsDNAdetector::Construct()
{
	BeginConstruction();
	
    //User may specify paramaters: DNASpacing, HolderHThickness, HolderHLX and HolderHLY
    //If not specified then they default to certain values as shown below
    //Usage shown above
    G4double DNASpacing, HolderHThickness, HolderHLX, HolderHLY;
	G4String name = GetFullParmName("DNASpacing");
    if (!fPm->ParameterExists(name)) {
        DNASpacing = 10*nm;
    } else {
        DNASpacing = fPm->GetDoubleParameter(name, "Length");
    }
    
	name = GetFullParmName("HolderHThickness");
    if (!fPm->ParameterExists(name)) {
        HolderHThickness = 5*nm;
    } else {
        HolderHThickness = fPm->GetDoubleParameter(name, "Length");
    }

	name = GetFullParmName("HolderHLX");
    if (!fPm->ParameterExists(name)) {
        HolderHLX = 50*nm;
    } else {
        HolderHLX = fPm->GetDoubleParameter(name, "Length");
    }

	name = GetFullParmName("HolderHLY");
    if (!fPm->ParameterExists(name)) {
        HolderHLY = HolderHLX;
    } else {
        HolderHLY = fPm->GetDoubleParameter(name, "Length");
    }
    
    //***********************************************************************
    //              Envelope Geometry : Box
    //***********************************************************************
    //Half length of the box
    //Finds maximum height, width and depth of box specified
    G4double maxDim; 
    if (HolderHLX >= HolderHLY) {
        maxDim = HolderHLX+100*nm;
    } else {
        maxDim = HolderHLY+100*nm;
    }
    if (maxDim < HolderHThickness+800*nm) {
        maxDim = HolderHThickness+800*nm;
    }
    G4double HL = maxDim;
    G4Box* gWrap = new G4Box(fName, HL, HL, HL);
    G4LogicalVolume* fEnvelopeLog = CreateLogicalVolume(gWrap);
    G4VPhysicalVolume* fEnvelopePhys = CreatePhysicalVolume(fEnvelopeLog);
    
    //****************************************************************************
    //                             Holder (DNA strands are attached)
    //****************************************************************************

    // Holder dimensions
    G4double HLX = HolderHLX;
    G4double HLY = HolderHLY;
    G4double Hthickness = HolderHThickness;
    
    //Parameters to rotate and place box within the envelope
    G4double angle = 90*deg;
    G4double Hx =0*nm, Hy =340*nm+Hthickness, Hz = 0*nm;
    
    G4ThreeVector* position1 = new G4ThreeVector(Hx, Hy, Hz);
    G4RotationMatrix* rotHolder = new G4RotationMatrix();
    rotHolder->rotateX(angle);
    
    G4Box* gHolder = new G4Box("Holder", HLX, HLY,Hthickness);
    G4LogicalVolume* lHolder = CreateLogicalVolume("Holder", gHolder);
    G4VPhysicalVolume* pHolder = CreatePhysicalVolume("Holder", lHolder, rotHolder, position1, fEnvelopePhys);
    
    // DNA strands
    
    //****************************************************************************
    //                             Cylinder for strands
    //****************************************************************************
    
    // Each DNA strand (single BP + 2 sugars) is placed in a box, that is replicated to fill the ring
    
    G4Tubs* gCyl = new G4Tubs("gCyl",
                              0*nm,
                              2*nm,
                              340*nm,
                              0*deg,
                              360*deg);
    
    G4LogicalVolume* lCyl = CreateLogicalVolume("Cyl", gCyl);
    

    
    //**************************************************************************
    //                 Subcomponent 1: Base pair
    //**************************************************************************
    
    //Base pair - a cylinder of radius 0.5 nm and length 0.34 nm.
    
    G4String Subcomponent1 = "BasePair";
    G4Tubs* gBp1 = new G4Tubs("BP", 0, 0.5*nm, 0.17*nm, 0.0*deg, 90.0*deg);
    G4LogicalVolume* lBp1 = CreateLogicalVolume("BP", gBp1);
    
    
    
    //**************************************************************************
    //                 Subcomponent 2: Sugar phosphate backbone
    //**************************************************************************
    
    
    //Phosphodiester group - two sugars each consisting of quarter cylinders
    //The sugars are wrapped around the base pair
    
    G4String Subcomponent2 = "Sugar1";
    G4Tubs* gSugarPhosphate1 = new G4Tubs("Sugar1", 0.5*nm, 1.185*nm, 0.17*nm, 0*deg, 45*deg);
    G4LogicalVolume* lSugarPhosphate1 = CreateLogicalVolume(gSugarPhosphate1);
    
    G4VisAttributes* Sugar1VisAtt = new G4VisAttributes(G4Color(0,0,127));
    Sugar1VisAtt->SetForceSolid(true);
    lSugarPhosphate1->SetVisAttributes(Sugar1VisAtt);
    
   /* Uncomment for a double stranded helix
    G4String Subcomponent3 = "Sugar2";
    G4Tubs* gSugarPhosphate2 = new G4Tubs("Sugar2", 0.5*nm, 1.185*nm, 0.17*nm, 0*deg, 45*deg);
    G4LogicalVolume* lSugarPhosphate2 = CreateLogicalVolume(gSugarPhosphate2);
    
    G4VisAttributes* Sugar2VisAtt = new G4VisAttributes(G4Color(0,0.5,0.5));
    Sugar2VisAtt->SetForceSolid(true);
    lSugarPhosphate2->SetVisAttributes(Sugar2VisAtt);
    */
    
    
    // Rotation of strands around the base pair
    
    G4double x = 0.0;
    G4double y = 0.0;
    G4double z0 = -340*nm + 0.17*nm;
    
    for (int j = 0; j < 2000; j++){
        
        G4double theta = 36*deg*j;
        G4double z = z0 + j*0.34*nm;
        
        G4ThreeVector position = G4ThreeVector(x, y, z);
        G4RotationMatrix rot = G4RotationMatrix();
        rot.rotateZ(theta);
        
        G4RotationMatrix rotBP = G4RotationMatrix();
        rotBP.rotateX(0);
        
        G4Transform3D transformBP = G4Transform3D(rotBP, position);
        G4Transform3D transformSugars = G4Transform3D(rot, position);
        
        G4RotationMatrix rotSugar2 = G4RotationMatrix();
        rotSugar2.rotateZ(135*deg+36*deg*j);
        G4Transform3D transformSugar2 = G4Transform3D(rotSugar2, position);

        
        new G4PVPlacement(transformSugars, lSugarPhosphate1, "sugar1", lCyl, true, j, false);
        //new G4PVPlacement(transformSugar2, lSugarPhosphate2, "sugar2", lCyl, true, j, false); 
        new G4PVPlacement(transformBP, lBp1, "BP", lCyl, true, j, false);
        
    }
        
   //Put DNA in Cylinders
    
    G4double theta = 90*deg;

    G4double x_row = 0.0;
    G4double y_const = 0.0;
    G4double z_col = 0;
    G4int row, col;
    
    row = 0;
    col = 0;
    G4double shiftx = DNASpacing;
    G4double shifty = DNASpacing;
    G4int numInRow = 2*HLX/DNASpacing+1;
    G4int numInCol = 2*HLY/DNASpacing+1;
	G4int i;
	//shifts the strands so the DNA is symmetrical about the centre of the holder
	G4double startshiftX = (2*HLX-(numInRow-1)*DNASpacing)/2; 
	G4double startshiftY = (2*HLY-(numInCol-1)*DNASpacing)/2; 
	
    for (i=1; i < numInRow*numInCol+1 ; i++) {       
        if (i % numInCol != 0) {
            x_row = HLX - shiftx*row-startshiftX;
            z_col = HLY - shifty*col-startshiftY;
            col++;     
        }
        else{
            x_row = HLX - shiftx*row-startshiftX;
            z_col = HLY - shifty*col-startshiftY;
            row++;
            col = 0;
        }
        //G4cout << x_row/nm << " " << y_const/nm << " " << z_col/nm << G4endl;
        G4ThreeVector* position2 = new G4ThreeVector(x_row, y_const, z_col);
        G4RotationMatrix* rot1 = new G4RotationMatrix();
        rot1 ->rotateX(theta);
    
        G4VPhysicalVolume* pCyl = CreatePhysicalVolume("Cyl", i, true, lCyl, rot1, position2, fEnvelopePhys);
        
    }

    InstantiateChildren(fEnvelopePhys);
	
	return fEnvelopePhys;
}
