/***** Granulator.h *****/


#include "GrainGenerator/GrainGenerator.h"
#include <vector>


class Granulator 
{
public:
	Granulator(int numVoices);
	~Granulator();
	
	//passes grain generator parameters to all voices
	void setAllParams(float speed);
	//process next sample
	void process();
	//trigger a note with pitch stored at given index in pitch array
	void triggerNote(int pitch_idx);
	
	//resizes the array of grain generators
	void setNumVoices(int voices_num) {
		this->numVoices = voices_num;
	}
	
private:
	int gateTimeSamps;
	float gateTimeSecs;
	int numVoices;
	
	std::vector<GrainGenerator> generators_;
	
	
	
};
