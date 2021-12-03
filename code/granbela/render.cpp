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

//Sample file
std::string gFilename = "samples/AI.wav";

//level
float kAmplitude = .5;

//Gui vars
Gui gui;
//GuiController controller;

int gCounter = 0;

int numVoices = 4;
int voiceLocation = 0;

GrainGenerator grain_generators[4]; 


float onset_samps;
float onset_ms;

//gate on for amplitude envelope
bool gate_on;
float amp_envelope;
float amp_duration_ms = 1000;
float amp_duration_samps;
int envelope_ptr;

float pitch = 0;
std::vector<float> pitches = {1.25,1.7,1.9,2,2.25,2.5};

bool hold;


bool setup(BelaContext *context, void *userData)
{	
    
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
	
	amp_envelope = 0;
	gate_on = false;
	//hold = true;

	//setup gui
	gui.setup(context->projectName);
	gui.setBuffer('f', 8);
	return true;
	
}

void triggerNote(int sensor_idx) { 
	gate_on = true;
	envelope_ptr = 0;
	pitch = pitches[sensor_idx];
}




void render(BelaContext *context, void *userData)
{


	DataBuffer& buffer = gui.getDataBuffer(0);
	float* data = buffer.getAsFloat();
	
	float speed = data[0];
	float duration = data[1];
	float filePos = data[2]/1000;
	onset_ms = data[3];
	onset_samps = (onset_ms / 1000) * context->audioSampleRate;
	float spray = data[4]/1000;
	
	int sensor_index = data[7];
	
	if (sensor_index >= 1 && gate_on == false) {
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
	
	

    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	
    	//stop note playback when envelope ptr meets decay time in samples
    	if (!hold && envelope_ptr >= (amp_duration_ms/1000)*context->audioSampleRate) {
			gate_on = false;
		}
    	
    	
    	//// Granulation Stage
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
		float amp = 1 - (envelope_ptr / amp_duration_samps);
		
		
		//// Effects Stage
		float out[2];
		out[0] = in * amp;
		out[1] = in * amp;
		//mute output
		if (!hold && !gate_on) {
			out[0] = 0;
			out[1] = 0;
		}
		
		
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			// Write the sample to every audio output channel
			audioWrite(context, n, channel, out[channel]*kAmplitude);	

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
