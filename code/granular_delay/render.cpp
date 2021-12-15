/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

http://bela.io
*/

#include <Bela.h>
#include "./GrainGenerator/GrainGenerator.h"
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <vector>

/*
 * HARDWARE GLOBALS
 */
float gPhase;
float gInverseSampleRate;
int gAudioFramesPerAnalogFrame = 0;

// Set the analog channels to read from
int gSensorInput1 = 0;
int gSensorInput2 = 1;
int gSensorInput3 = 2;
int gNumberOfSensors = 3;

// These define the "working range" of the SHARP IR sensor
float gMinDist = 20;
float gTriggerDist = 40;

// These will be used to print debug output
int gDebugCounter = 0;
int gDebugCounterMax;

//Sample file
std::string gFilename = "samples/AI.wav";

//level
float kAmplitude = .5;

//Gui vars
Gui gui;
//GuiController controller;

int gCounter = 0;

int numVoices = 20;
GrainGenerator grain_generators[20]; 
int voiceLocation = 0;


float onset_samps;
float onset_ms;

//gate on for amplitude envelope
bool gate_on;
float amp_duration_ms = 10000;
float amp_duration_samps;
int envelope_ptr;

float pitch = 0;
std::vector<float> pitches = {1.25,1.7,1.9,2,2.25,2.5};

bool hold;

//delay
std::vector<float> delayBuffer;
unsigned int readPointer = 0;
unsigned int writePointer = 0;

bool setup(BelaContext *context, void *userData)
{	
    /*
     * HARDWARE SETUP
     */

	// Check if analog channels are enabled
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return false;
	}

	// Useful calculations
	if(context->analogFrames)
		gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
	gInverseSampleRate = 1.0 / context->audioSampleRate;
	gPhase = 0.0;

	// print hardware debug info every 1/4 second
	gDebugCounterMax = .25 * context->audioSampleRate;


	/*
	 * GRANULAR SETUP
	 */

    for (int i=0;i<numVoices;i++) {
		GrainGenerator grain;
		// Load the audio file into each grain voice
		grain_generators[i] = grain;
		if(!grain_generators[i].setup(gFilename,context->audioSampleRate,false,false)) {
    		rt_printf("Error loading audio file '%s'\n", gFilename.c_str());
    		return false;
		}
	}
	
	onset_ms = 4000;
	onset_samps = (onset_ms / 1000) * context->audioSampleRate;
	
	amp_duration_samps = amp_duration_ms/1000 * context->audioSampleRate;
	
	gate_on = true;
	//hold = true;
	
	//delay setup
	delayBuffer.resize(.5 * context->audioSampleRate);
	readPointer = (writePointer -  (int)(0.1 * context->audioSampleRate ) + delayBuffer.size() ) % delayBuffer.size();

	//setup gui
	gui.setup(context->projectName);
	gui.setBuffer('f', 10);


	return true;
	
}

void triggerNote(int sensor_idx) { 
	gate_on = true;
	envelope_ptr = 0;
	pitch = pitches[sensor_idx];
}

float amplitudeFromRaw(float raw1, float raw2, float raw3) {
/*
 * Converts the three sensor inputs [cm] to a desired amplitude value to allow expressive control.
 * Inputs:
 * 		raw1, raw2, raw3: all three sensor values [cm]
 */

	// check if all of the sensors are not triggered
	if (raw1>gTriggerDist && raw2>gTriggerDist && raw3>gTriggerDist) {
		return 0;
	}
	//otherwise, map the output of the least-valued sensor between 0 and 1, make it the global amplitude
	else {
	    float minDist = std::min(raw1, std::min(raw2, raw3));
		return map(minDist, gTriggerDist, gMinDist, 0, 1);
	}
}

float getPitchFromSensors(float *rawInputs) {

	/*
	 * Takes an array of raw inputs, finds minimum in the array, loop through sensors
	 */

	float min = 1000;
	int activeSensorIdx = -1;

	for (int i=0; i<gNumberOfSensors; i++) {
		if (rawInputs[i] < min) {
			min = rawInputs[i];
			activeSensorIdx = i;
		}
	}

	// return the corresponding pitch
	return pitches[activeSensorIdx];

}


void render(BelaContext *context, void *userData)
{
	// SENSOR VALUES
	float ir1; 
	float ir1_dist = 0;
	float ir2;
	float ir2_dist = 0;
	float ir3;
	float ir3_dist = 0;
	float dists[3] = {-1, -1, -1};

	//retrieve data buffer from gui
	DataBuffer& buffer = gui.getDataBuffer(0);
	float* data = buffer.getAsFloat();
	
	float speed = data[0];
	float duration = data[1]/1000;
	float filePos = data[2]/1000;
	onset_ms = data[3]/8;
	onset_samps = (onset_ms / 1000) * context->audioSampleRate;
	float spray = data[4]/1000;
	float delayTime = (data[8]/1000);
	//float feedback = (data[9]/1000);
	
	// index of the last sensor 
	int sensor_index = data[7];
	
	if (sensor_index >= 1) {
		triggerNote(sensor_index);
	}


	//rt_printf("%f", speed);
	//pass parameters to grain generators
	for (int i=0;i<numVoices;i++) {
		grain_generators[i].setSpeed(speed*pitch); 
		grain_generators[i].setDuration(duration * context->audioSampleRate);
		grain_generators[i].setFilePos(filePos* context->audioSampleRate);
		grain_generators[i].setSpray(spray * context->audioSampleRate);
	}
	
	//calculate read pointer
	readPointer = (writePointer -  (int)(delayTime * context->audioSampleRate ) + delayBuffer.size() ) % delayBuffer.size();


	/***********************
	 *
	 * AUDIO LOOP
	 *
	 ***********************/
    for(unsigned int n = 0; n < context->audioFrames; n++) {

    	/***********************
    	 *
    	 * HARDWARE INPUT STAGE
    	 *
    	 ***********************/

    	if(gAudioFramesPerAnalogFrame && !(n % gAudioFramesPerAnalogFrame)) {
    		// read analog inputs and update the readings from sensors
    		// Depending on the sampling rate of the analog inputs, this will
    		// happen every audio frame (if it is 44100)
    		// or every two audio frames (if it is 22050)
    		ir1 = analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInput1);
    		ir2 = analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInput2);
    		ir3 = analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInput3);
    		
    		// map all raw values to distance
    		ir1_dist = map(ir1, 0.10, 0.60, 52.253, 18.92);
    		ir2_dist = map(ir2, 0.10, 0.60, 52.253, 18.92);
    		ir3_dist = map(ir3, 0.10, 0.60, 52.253, 18.92);
			
			dists[0] = ir1_dist;
			dists[1] = ir2_dist;
			dists[2] = ir3_dist;
			pitch = getPitchFromSensors(dists);
    		
    	}

		/*
		 * DISTANCE SENSOR VALUES (FOR DEBUGGING)
		 */
    	gDebugCounter++;
		if(gDebugCounter == gDebugCounterMax) {
			rt_printf("[sensor1: %f, sensor2 %f, sensor3 %f] \n", ir1_dist, ir2_dist, ir3_dist);
			gDebugCounter = 0;
		}
    	
		/*
    	//stop note playback when envelope ptr meets decay time in samples
    	if (!hold && envelope_ptr >= (amp_duration_ms/1000)*context->audioSampleRate) {
			gate_on = false;
		}
    	*/
    	
    	/***********************
    	 *
    	 * GRANULATION STAGE
    	 *
    	 ***********************/
    	float in = 0;

    	//trigger next grain generator voice
		if (gCounter == 0) {
			grain_generators[voiceLocation].trigger();
		}

		//sum the ouputs of all of the grain generators
		for (int i=0;i<numVoices;i++) {
			in += grain_generators[i].process();
		}
		
		//apply amplitude envelope
		//inverse ramp shape
		//float amp = 1 - (envelope_ptr / amp_duration_samps);
		
		
		
		/***********************
		 *
		 * EFFECTS STAGE
		 *
		 ***********************/
		float out;
		
		
		//DELAY LINE
		out = delayBuffer[readPointer] * amplitudeFromRaw(ir1_dist, ir2_dist, ir3_dist);
		delayBuffer[readPointer] = out;
		
		delayBuffer[writePointer] = in;/*+(feedback*out)*/
		
        writePointer++;
        if(writePointer >= delayBuffer.size())
        	writePointer = 0;
        readPointer++;
        if(readPointer >= delayBuffer.size())
        	readPointer = 0;
        	
        /*	
		//mute output
		if (!hold && !gate_on) {
			out = 0;
		}
		*/

    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			// Write the sample to every audio output channel
			audioWrite(context, n, channel, out*kAmplitude);
    	}
    	
    	
   		gCounter++;
    	if (gCounter >= onset_samps) {
    		gCounter = 0;
    		voiceLocation++;
    		if(voiceLocation>=numVoices) {
    			voiceLocation=0;
    		}
    	}
    	
    	envelope_ptr++;

    }
}

void cleanup(BelaContext *context, void *userData)
{

}
