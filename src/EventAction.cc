/// \file B1/src/EventAction.cc
/// \brief Implementation of the B1::EventAction class

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"

namespace B1{
    
    EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}
    
    void EventAction::BeginOfEventAction(const G4Event*){

        fEDepEventPlastic = 0.;
        fEDepEventGAGG = 0.;

    }

    void EventAction::EndOfEventAction(const G4Event* event){

        G4int eventID = event->GetEventID();
        if (eventID % 1000000 == 0) {

            G4cout << "Processed " << eventID << " events..." << G4endl;

        }

        fRunAction->AddEDepPlastic(fEDepEventPlastic);
        fRunAction->AddEDepGAGG(fEDepEventGAGG);

    }

}