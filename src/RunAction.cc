/// \file B1/src/RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4AccumulableManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <filesystem>

namespace B1{

    RunAction::RunAction(){

        // Add new units for dose
        const G4double milligray = 1.e-3 * gray;
        const G4double microgray = 1.e-6 * gray;
        const G4double nanogray = 1.e-9 * gray;
        const G4double picogray = 1.e-12 * gray;

        new G4UnitDefinition("milligray", "milliGy", "Dose", milligray);
        new G4UnitDefinition("microgray", "microGy", "Dose", microgray);
        new G4UnitDefinition("nanogray", "nanoGy", "Dose", nanogray);
        new G4UnitDefinition("picogray", "picoGy", "Dose", picogray);

        // Register accumulable to the accumulable manager
        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Register(fEDepGAGG);
        accumulableManager->Register(fEDep2GAGG);
        accumulableManager->Register(fEDepPlastic);
        accumulableManager->Register(fEDep2Plastic);

    }

    void RunAction::BeginOfRunAction(const G4Run*){

        // Inform the runManager to save random number seed
        G4RunManager::GetRunManager()->SetRandomNumberStore(false);

        // Reset accumulables to their initial values
        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Reset();

    }

    void RunAction::EndOfRunAction(const G4Run* run){

        G4int nofEvents = run->GetNumberOfEvent();
        if (nofEvents == 0) return;

        // Merge accumulables
        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Merge();

        if (IsMaster()) {

            // Compute total energy deposit in a run and its variance
            G4double eDepGAGG = fEDepGAGG.GetValue();
            G4double eDep2GAGG = fEDep2GAGG.GetValue();
            G4double eDepPlastic = fEDepPlastic.GetValue();
            G4double eDep2Plastic = fEDep2Plastic.GetValue();

            G4double rmsEDepPlastic = eDep2Plastic - eDepPlastic * eDepPlastic / (nofEvents + 0.);
            if (rmsEDepPlastic > 0.)
                rmsEDepPlastic = std::sqrt(rmsEDepPlastic);
            else
                rmsEDepPlastic = 0.;

            G4double rmsEDepGAGG = eDep2GAGG - eDepGAGG * eDepGAGG / (nofEvents + 0.);
            if (rmsEDepGAGG > 0.)
                rmsEDepGAGG = std::sqrt(rmsEDepGAGG);
            else
                rmsEDepGAGG = 0.;

            // Compute dose and its variance
            const auto detConstruction = static_cast<const DetectorConstruction*>(
                G4RunManager::GetRunManager()->GetUserDetectorConstruction()
            );

            G4double massGAGG = detConstruction->GetScoringVolumeGAGG()->GetMass();
            G4double doseGAGG = eDepGAGG / massGAGG;
            G4double rmsDoseGAGG = rmsEDepGAGG / massGAGG;

            G4double massPlastic = detConstruction->GetScoringVolumePlastic()->GetMass();
            massPlastic -= massGAGG;
            G4double dosePlastic = eDepPlastic / massPlastic;
            G4double rmsDosePlastic = rmsEDepPlastic / massPlastic;

            // Print
            G4cout
            << "Deposited energy in GAGG: "
            << G4BestUnit(eDepGAGG, "Energy") << " rms = " << G4BestUnit(rmsEDepGAGG, "Energy")
            << G4endl
            << "Cumulated dose in GAAG: "
            << G4BestUnit(doseGAGG, "Dose") << " rms = " << G4BestUnit(rmsDoseGAGG, "Dose")
            << G4endl
            << "Deposited energy in plastic: "
            << G4BestUnit(eDepPlastic, "Energy") << " rms = " << G4BestUnit(rmsEDepPlastic, "Energy")
            << G4endl
            << "Cumulated dose in plastic: "
            << G4BestUnit(dosePlastic, "Dose") << " rms = " << G4BestUnit(rmsDosePlastic, "Dose")
            << G4endl
            << "------------------------------------------------------------"
            << G4endl
            << G4endl;

        }

    }

    void RunAction::AddEDepGAGG(G4double eDep){

        fEDepGAGG += eDep;
        fEDep2GAGG += eDep * eDep;

    }

    void RunAction::AddEDepPlastic(G4double eDep){

        fEDepPlastic += eDep;
        fEDep2Plastic += eDep * eDep;

    }

}
