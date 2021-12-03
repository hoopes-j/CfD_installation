/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io

C++ Real-Time Audio Programming with Bela - Lecture 8: Filters
*/

#include <libraries/AudioFile/AudioFile.h>
#include "GrainGenerator.h"
#include <Bela.h>
#include <libraries/Scope/Scope.h>

// Constructor taking the path of a file to load
MonoFilePlayer::MonoFilePlayer(const std::string& filename, bool loop, bool autostart)
{
	setup(filename, loop, autostart,0);	
}

int gRand = 0;

// Load an audio file from the given filename. Returns true on success.
bool MonoFilePlayer::setup(const std::string& filename,  int sampleRate, bool loop, bool autostart)
{
	readPointer_ = 0;
	stopped = false;
	
	// Load the file
	sampleBuffer_ = AudioFileUtilities::loadMono(filename);
	
	// Check for error
	if(sampleBuffer_.empty()) {
		stopped = true;
    	return false;
	}
	
	this->samp_ptr = 0;
	float max_duration = 0;
	
	//init granular variables
	duration_samps = sampleBuffer_.size();
	
	this->randy = spray_ != 0 ? (rand()%(int)spray_) : 0;
	
	return true;
}

// Tell the buffer to start playing from the beginning
void MonoFilePlayer::trigger()
{
		if(sampleBuffer_.empty())
			return;
		envelopePointer = 0;
		stopped = false;
		this->randy = spray_ != 0 ? (rand()%(int)spray_) : 0;
}

void MonoFilePlayer::setSpeed(float speed) 
{
	this->playbackSpeed = speed;
}
void MonoFilePlayer::setDuration(int duration)
{
	this->duration_samps = duration;
}
void MonoFilePlayer::setFilePos(int filePos) 
{
	this->filePos = filePos;
}

float MonoFilePlayer::getWindowedAmplitude(float sample) {
	float threshold = 4410;
	if (sample < threshold) {
		return map(sample,0,4410,0,1);
	}
	else {
		return 1;
	}
}

// Return the next sample of the loaded audio file
float MonoFilePlayer::process()
{
	if(stopped)	
		return 0;
	
	float samp_increment = this->playbackSpeed;
	
	int p_l = this->filePos + randy;
	int p_u = p_l + ((samp_increment)*this->duration_samps)/this->size();
	
	//y is position in file to read from postion envelope
	float y = (samp_increment * envelopePointer) + p_l;
	
	//interpolate amp value from buffer
	int prev_samp = floorf(y);
	int next_samp = prev_samp+1;
	float frac_below = y - prev_samp;
	float frac_above = 1 - frac_below;
	float out = (frac_below * this->sampleBuffer_[prev_samp]) + (frac_above * this->sampleBuffer_[next_samp]);
	
	//calculate sin window
	float windowVal =  sinf((M_PI*envelopePointer)/duration_samps);
	
	out = out * .75 * windowVal;
	
	
	
	envelopePointer++;
	if (envelopePointer >= duration_samps) {
		stopped = true;
	}
		
	
	// int prevIdx = floorf(grainPointer);
	// int nextIdx = prevIdx + 1;
	// if (nextIdx >= grainSize) {
	// 	nextIdx = 0;
	// }
	
	// float fracBelow = grainPointer - prevIdx;
	// float fracAbove = 1 - fracBelow;
	
	// int prevPrime = prevIdx + filePos + randy;
	// int nextPrime = nextIdx + filePos + randy;
	
	// if(nextPrime >= size()) {
	// 	nextPrime = nextIdx % size();
	// }
	// if(prevPrime>= size()) {
	// 	prevPrime = prevIdx % size();
	// }
	// // Read the next sample from the buffer
	// float out = sampleBuffer_[prevPrime] * fracBelow + sampleBuffer_[nextPrime] * fracAbove;
        
	// // Increment read pointer
 //   grainPointer += playbackSpeed;
 //   envelopePointer++;
    
 //   //windowing
 //   out = out *getWindowedAmplitude(envelopePointer);
    
 //   // If we reach the end, decide whether to loop or stop
 //   if(grainPointer >= grainSize/playbackSpeed) {
 //    	grainPointer = 0;
     	
 //    	//if(!loop_)
 //   }
    
    
    return out;
}
	