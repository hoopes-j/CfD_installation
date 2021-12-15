/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 8: Filters
*/

// This is a simple class encapsulating the playback of a sound
// loaded from an audio file. It offers basic controls to loop, start
// and stop the playback. It assumes a mono audio file.

#pragma once

#include <vector>
#include <string>
#include <cmath>

class GrainGenerator {
public:
	// Constructors: the one with arguments automatically calls setup()
	GrainGenerator() {}
	GrainGenerator(const std::string& filename, bool loop = true, bool autostart = true);
	
	// Load an audio file from the given filename. Returns true on success.
	bool setup(const std::string& filename,int sampleRate, bool loop = true, bool autostart = true);
	
	void setSpeed(float speed);
	void setDuration(int duration);
	void setFilePos(int filePos);
	void setSpray(float spray) { this->spray_ = spray;}
	
	// Start or stop the playback
	void trigger();
	void stop() { stopped = true; }
	

	// Return the length of the buffer in samples
	unsigned int size() { return sampleBuffer_.size(); }
	
	//Return the file buffer
	std::vector<float> getSampleBuffer() { return sampleBuffer_; }
	
	// Return the next sample of the loaded audio file
	float process();
	
	float getWindowedAmplitude(float sample);
	
	// Destructor
	~GrainGenerator() {}
	
private:
	std::vector<float> sampleBuffer_;			// Buffer that holds the sound file
	float playbackSpeed = 1;					// speed of playback
	int duration_samps;							// grain size
	int filePos;								// grain starting pos
	int randy;									// randomsized sample offset
	float spray_ = 0;							// amount of randomness
	int envelopePointer;						// file position envelope pointer
	bool stopped;								// grain will not be generated when true
};

