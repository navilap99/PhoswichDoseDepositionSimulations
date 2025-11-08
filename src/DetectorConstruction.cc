/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

namespace B1{

    G4VPhysicalVolume* DetectorConstruction::Construct() {
     
        // Get nist material manager
        G4NistManager* nist = G4NistManager::Instance();

        // Option to switch on/off checking of volumes overlaps
        G4bool checkOverlaps = true;

        // **********************
        // Plastic parameters 
        // **********************

        // Define material

        G4Material* plasticMat = nist->FindOrBuildMaterial("G4_TISSUE_SOFT_ICRU-4");

        // **********************
        // GAGG parameters 
        // **********************

        // Define material

        G4Material* GAAG = G4Material::GetMaterial("GAAG");
        if (!GAAG) {

            G4Element* Gd = new G4Element("Gadolinium", "Gd", 64, 157.24 * g / mole);
            G4Element* Al = new G4Element("Aluminium", "Al", 13, 26.98 * g / mole);
            G4Element* Ga = new G4Element("Gallium", "Ga", 31, 69.723 * g / mole);
            G4Element* O = new G4Element("Oxygen", "O", 8, 16.00 * g / mole);

            G4double gaagDensity = 6.64 * g / cm3;

            GAAG = new G4Material("GAAG", gaagDensity, 4);
            GAAG->AddElement(Gd, 3);
            GAAG->AddElement(Al, 2);
            GAAG->AddElement(Ga, 3);
            GAAG->AddElement(O, 12);

        }

        G4Material* gaggMat = GAAG;

        // **********************
        // World parameters 
        // **********************
    
        G4double worldSizeXY = 1.2 * plasticDiameter;
        G4double worldSizeZ = 1.2 * plasticSizeZ;
        G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");

        // **********************
        // Solids, Logicals and Placement
        // **********************

        // World

        auto solidWorld = new G4Box(
            "World",
            0.5 * worldSizeXY,
            0.5 * worldSizeXY,
            0.5 * worldSizeZ
        );

        auto logicWorld = new G4LogicalVolume(
            solidWorld,
            worldMat,
            "World" // its name
        );

        auto physWorld = new G4PVPlacement(
            nullptr,  // no rotation
            G4ThreeVector(),
            logicWorld,  // its logical volume
            "World",  // its name
            nullptr,  // its mother  volume
            false,  // no boolean operation
            0,  // copy number
            checkOverlaps
        );

        // Plastic

        auto solidPlastic = new G4Tubs(
            "Plastic",
            0.0,
            0.5 * plasticDiameter,
            0.5 * plasticSizeZ,
            0,
            360 * deg
        );

        auto logicPlastic = new G4LogicalVolume(
            solidPlastic,
            plasticMat,
            "Plastic"
        );

        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            logicPlastic,
            "Plastic",
            logicWorld,
            false,
            0,
            checkOverlaps
        );

        // GAGG

        auto solidGAGG = new G4Box(
            "GAGG",
            0.5 * gaggSizeX,
            0.5 * gaggSizeY,
            0.5 * gaggSizeZ
        );

        auto logicGAGG = new G4LogicalVolume(
            solidGAGG,
            gaggMat,
            "GAGG"
        );

        new G4PVPlacement(
            nullptr,
            G4ThreeVector(),
            logicGAGG,
            "GAGG",
            logicPlastic,
            false,
            0,
            checkOverlaps
        );

        // Set scoring volumes

        fScoringVolumePlastic = logicPlastic;
        fScoringVolumeGAGG = logicGAGG;

        // **********************
        // Always return the physical World
        // **********************

        return physWorld;

    }

}