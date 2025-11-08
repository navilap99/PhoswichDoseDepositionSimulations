/// \file B1/include/SteppingAction.hh
/// \brief Definition of the B1::SteppingAction class

#ifndef B1SteppingAction_h
#define B1SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class G4LogicalVolume;
class G4Step;

namespace B1{

    class EventAction;

    class SteppingAction : public G4UserSteppingAction{

        public:

            SteppingAction(EventAction* eventAction);
            ~SteppingAction() override = default;

            void UserSteppingAction(const G4Step*) override;

        private:

            EventAction* fEventAction = nullptr;
            G4LogicalVolume* fScoringVolumeGAGG = nullptr;
            G4LogicalVolume* fScoringVolumePlastic = nullptr;

    };

}

#endif
