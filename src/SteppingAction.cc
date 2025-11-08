/// \file B1/src/SteppingAction.cc
/// \brief Implementation of the B1::SteppingAction class

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"

namespace B1{

    SteppingAction::SteppingAction(EventAction* eventAction) : fEventAction(eventAction) {}

    void SteppingAction::UserSteppingAction(const G4Step* step){

        if (!fScoringVolumeGAGG) {

            const auto detConstruction = static_cast<const DetectorConstruction*>(
                G4RunManager::GetRunManager()->GetUserDetectorConstruction()
            );
            fScoringVolumeGAGG = detConstruction->GetScoringVolumeGAGG();

        }

        if (!fScoringVolumePlastic) {

            const auto detConstruction = static_cast<const DetectorConstruction*>(
                G4RunManager::GetRunManager()->GetUserDetectorConstruction()
            );
            fScoringVolumePlastic = detConstruction->GetScoringVolumePlastic();

        }

        // Get volume of the current step
        G4LogicalVolume* volume =
            step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

        // Collect energy deposition
        G4double eDepStep = step->GetTotalEnergyDeposit();
        if (volume != fScoringVolumeGAGG) {

            if (volume != fScoringVolumePlastic) {

                return;

            }

            fEventAction->AddEDepPlastic(eDepStep);
            return;
        }
        
        fEventAction->AddEDepGAGG(eDepStep);

    }

}
