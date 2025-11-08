/// \file B1/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace B1{
	
	PrimaryGeneratorAction::PrimaryGeneratorAction(){

		G4int nParticle = 1;
		fParticleGun = new G4ParticleGun(nParticle);

		// Default kinematics
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4String particleName;
		G4ParticleDefinition* particle = particleTable->FindParticle(particleName = "gamma");
		fParticleGun->SetParticleDefinition(particle);
		fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
		fParticleGun->SetParticleEnergy(6. * MeV);

	}

	PrimaryGeneratorAction::~PrimaryGeneratorAction(){

		delete fParticleGun;

	}

	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event){

		G4double plasticRadius = 0;
		G4double plasticSizeZ = 0;

		if (!fPlasticSolid) {

			G4LogicalVolume* plasticLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Plastic");
			if (plasticLV) fPlasticSolid = dynamic_cast<G4Tubs*>(plasticLV->GetSolid());

		}


		if (fPlasticSolid) {

			plasticRadius = fPlasticSolid->GetOuterRadius();
			plasticSizeZ = fPlasticSolid->GetZHalfLength() * 2.;

		}

		G4double randomAngle = 2 * CLHEP::pi * G4UniformRand();
		G4double randomRadius = plasticRadius * pow(G4UniformRand(), 0.5);
		G4double x0 = randomRadius * std::cos(randomAngle);
		G4double y0 = randomRadius * std::sin(randomAngle);
		G4double z0 = -0.5 * plasticSizeZ;

		fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
		fParticleGun->GeneratePrimaryVertex(event);

	}

}