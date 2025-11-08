/// \file B1/include/RunAction.hh
/// \brief Definition of the B1::RunAction class

#ifndef B1RunAction_h
#define B1RunAction_h 1

#include "G4UserRunAction.hh"

#include "G4Accumulable.hh"
#include "globals.hh"

class G4Run;

namespace B1{

    class RunAction : public G4UserRunAction{

        public:

            RunAction();
            ~RunAction() override = default;

            void BeginOfRunAction(const G4Run*) override;
            void EndOfRunAction(const G4Run*) override;

            void AddEDepPlastic(G4double eDep);
            void AddEDepGAGG(G4double eDep);

        private:

            G4Accumulable<G4double> fEDepGAGG = 0.;
            G4Accumulable<G4double> fEDep2GAGG = 0.;
            G4Accumulable<G4double> fEDepPlastic = 0.;
            G4Accumulable<G4double> fEDep2Plastic = 0.;

    };

}

#endif
