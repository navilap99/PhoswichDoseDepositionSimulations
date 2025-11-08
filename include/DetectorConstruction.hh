/// \file B1/include/DetectorConstruction.hh
/// \brief Definition of the B1::DetectorConstruction class

#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

namespace B1{

    class DetectorConstruction : public G4VUserDetectorConstruction{

        public:

            DetectorConstruction() = default;
            ~DetectorConstruction() override = default;

            G4VPhysicalVolume* Construct() override;

            G4LogicalVolume* GetScoringVolumePlastic() const { return fScoringVolumePlastic; }
            G4LogicalVolume* GetScoringVolumeGAGG() const { return fScoringVolumeGAGG; }

            void SetPlasticDimensions(G4double diameter, G4double sizeZ);
            void SetGAAGDimensions(G4double sizeX, G4double sizeY, G4double sizeZ);

        protected:

            G4LogicalVolume* fScoringVolumePlastic = nullptr;
            G4LogicalVolume* fScoringVolumeGAGG = nullptr;

            G4double gaggSizeX = 0.75 * cm;
            G4double gaggSizeY = gaggSizeX;
            G4double gaggSizeZ = 2.0 * cm;
            G4double plasticDiameter = 2.1 * cm;
            G4double plasticSizeZ = 2.0 * cm;

    };

}

#endif
