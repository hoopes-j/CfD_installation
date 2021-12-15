/***** Granulator.cpp *****/
#include "Granulator.h"


	
	
Granulator::Granulator(int numVoices) {
	this->numVoices = numVoices;
	// stdvector<GrainGenerator> voices_vector(numVoices, GrainGenerator())
}

Granulator::~Granulator() {
	
}


//passes grain generator parameters to all voices
void Granulator::setAllParams(float speed)
{
	for (int i = 0; i < this->generators_.size(); i++) {
		generators_[i].setSpeed(speed);
	}
}
//process next sample
void Granulator::process()
{
	
}
//trigger a note with pitch stored at given index in pitch array
void Granulator::triggerNote(int pitch_idx)
{
	
}
	
