/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4SystemOfUnits.hh"
#include "G4VModularPhysicsList.hh"
#include "G4LossTableManager.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4UnitsTable.hh"

#include <filesystem>

using namespace B1;

int main(int argc, char** argv){

    // Geometry parameters
    G4double plasticDiameterInput = std::stod(argv[1]) * cm;
    G4double plasticSizeZInput = std::stod(argv[2]) * cm;
    G4double gaggSizeXInput = std::stod(argv[3]) * cm;
    G4double gaggSizeYInput = std::stod(argv[4]) * cm;
    G4double gaggSizeZInput = std::stod(argv[5]) * cm;

    // Run parameters
    G4double energyMin = 0.;
    G4double energyMax = 0.;
    G4double energyStep = 0.;
    G4int nEvents = 1;

    G4double indexMin = 0.;
    G4double indexMax = 0.;

    // Detect interactive mode (if only geometry parameters passed) and define UI session
    G4UIExecutive* ui = nullptr;
    if (argc == 6) {

        ui = new G4UIExecutive(argc, argv);

    }

    else{

        // Run parameters
        energyMin = std::stod(argv[6]) * MeV;
        energyMax = std::stod(argv[7]) * MeV;
        energyStep = std::stod(argv[8]) * MeV;
        nEvents = std::stoi(argv[9]);

        indexMin = std::log10(energyMin/MeV);
        indexMax = std::log10(energyMax/MeV);

    }

    // RunManager
    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly); //SerialOnly or Default

    // DetectorConstruction
    auto detectorConstruction = new DetectorConstruction();
	detectorConstruction->SetPlasticDimensions(plasticDiameterInput, plasticSizeZInput);
	detectorConstruction->SetGAAGDimensions(gaggSizeXInput, gaggSizeYInput, gaggSizeZInput);
    runManager->SetUserInitialization(detectorConstruction);

    // Physics list
    G4VModularPhysicsList* physicsList = new G4VModularPhysicsList();
	G4LossTableManager::Instance();
	physicsList->SetDefaultCutValue(0.01*mm);
	physicsList->RegisterPhysics(new G4EmStandardPhysics_option3);
	G4StepLimiterPhysics* stepLimitPhys = new G4StepLimiterPhysics();
	stepLimitPhys->SetApplyToAll(true);
	physicsList->RegisterPhysics(stepLimitPhys);
	runManager->SetUserInitialization(physicsList);

    // ActionInitialization
    runManager->SetUserInitialization(new ActionInitialization());
    runManager->Initialize();

    // Initialize visualization with the default graphics system
    auto visManager = new G4VisExecutive(argc, argv);
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    auto UImanager = G4UImanager::GetUIpointer();

    // **********
    // Output file
    // **********

    // Geometry parameters
    G4LogicalVolume* GAGGLV = detectorConstruction->GetScoringVolumeGAGG();
    G4Box* GAGGSolid = dynamic_cast<G4Box*>(GAGGLV->GetSolid());
    G4double gaggSizeX = (GAGGSolid->GetXHalfLength()) * 2.0;
    G4double gaggSizeY = (GAGGSolid->GetYHalfLength()) * 2.0;
    G4double gaggSizeZ = (GAGGSolid->GetZHalfLength()) * 2.0;
    G4double gaggVolume = GAGGLV->GetSolid()->GetCubicVolume();
    G4double gaggMass = GAGGLV->GetMass();
    G4double gaggDensity = GAGGLV->GetMaterial()->GetDensity();

    G4LogicalVolume* plasticLV = detectorConstruction->GetScoringVolumePlastic();
    G4Tubs* plasticSolid = dynamic_cast<G4Tubs*>(plasticLV->GetSolid());
    G4double plasticRadius = plasticSolid->GetOuterRadius();
    G4double plasticSizeZ = (plasticSolid->GetZHalfLength()) * 2.0;
    G4double plasticTotalVolume = plasticLV->GetSolid()->GetCubicVolume();
    G4double plasticVolume = plasticTotalVolume - gaggVolume;
    G4double plasticTotalMass = plasticLV->GetMass();
    G4double plasticMass = plasticTotalMass - gaggMass;
    G4double plasticDensity = plasticLV->GetMaterial()->GetDensity();

    // Print parameters in output file
    std::string filename = "output.txt";
    std::ofstream outFile(filename);
    if (outFile) {
        std::cout << "File created successfully: " << filename << std::endl;

        outFile << "nEvents = " << nEvents << "\n";
        outFile << "GAAG:" << "\n";
        outFile << "dx = " << G4BestUnit(gaggSizeX, "Length") << " dy = " << G4BestUnit(gaggSizeY, "Length") << " dz = " << G4BestUnit(gaggSizeZ, "Length") << "\n";
        outFile << "Density = " << G4BestUnit(gaggDensity, "Volumic Mass") << " Volume = " << G4BestUnit(gaggVolume, "Volume") << " Mass = " << G4BestUnit(gaggMass, "Mass") << "\n";
        outFile << "Plastic:" << "\n";
        outFile << "r = " << G4BestUnit(plasticRadius, "Length") << " h = " << G4BestUnit(plasticSizeZ, "Length") << "\n";
        outFile << "Density = " << G4BestUnit(plasticDensity, "Volumic Mass") << " Volume = " << G4BestUnit(plasticVolume, "Volume") << " Mass = " << G4BestUnit(plasticMass, "Mass") << "\n";
        outFile << "photonEnergy / MeV" << "\t" << "eDepGAGG / GeV" << "\t" << "dEDepGAGG / GeV" << "\t" << "eDepPlastic / GeV" << "\t" << "dEDepPlastic / GeV" << "\t" << "doseGAGG / Gy" << "\t" << "dDoseGAGG / Gy" << "\t" << "dosePlastic / Gy" << "\t" << "dDosePlastic / Gy" << "\n";

        outFile.close();
    }
    else {
        std::cerr << "Failed to create the file: " << filename << std::endl;
    }

    // Process macro or start UI session
    if (!ui) {

        // Batch mode
        for (G4double index = indexMin; index <= (indexMax + energyStep); index += energyStep) {

            G4double energy = std::pow(10, index);

            if (std::filesystem::exists(filename)) {
                std::ofstream file;
                file.open(filename, std::ios::app);
                file << energy / MeV << "\t";
                file.close();
            }

            std::cout << energy << "\n";
            UImanager->ApplyCommand("/gun/particle gamma");
            std::ostringstream energyCmd;
            energyCmd << "/gun/energy " << G4BestUnit(energy, "Energy");
            std::cout << energyCmd.str() << "\n";
            UImanager->ApplyCommand(energyCmd.str());

            std::ostringstream beamOnCmd;
            beamOnCmd << "/run/beamOn " << nEvents;

            G4cout
            << G4endl
            << "------------------------------------------------------------"
            << G4endl
            << "The run consists of " << nEvents << " gammas of energy " << G4BestUnit(energy, "Energy")
            << G4endl;

            UImanager->ApplyCommand(beamOnCmd.str());
            
        }

    }
    else {

        // interactive mode
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;

    }

    delete visManager;
    delete runManager;
    
}