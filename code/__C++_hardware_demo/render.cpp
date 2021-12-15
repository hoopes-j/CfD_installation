/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

The platform for ultra-low latency audio and sensor processing

http://bela.io

A project of the Augmented Instruments Laboratory within the
Centre for Digital Music at Queen Mary University of London.
http://www.eecs.qmul.ac.uk/~andrewm

(c) 2016 Augmented Instruments Laboratory: Andrew McPherson,
  Astrid Bin, Liam Donovan, Christian Heinrichs, Robert Jack,
  Giulio Moro, Laurel Pardue, Victor Zappi. All rights reserved.

The Bela software is distributed under the GNU Lesser General Public License
(LGPL 3.0), available here: https://www.gnu.org/licenses/lgpl-3.0.txt
*/


#include <Bela.h>
#include <cmath>

float gPhase;
float gInverseSampleRate;
int gAudioFramesPerAnalogFrame = 0;

// Set the analog channels to read from
int gSensorInput1 = 0;
int gSensorInput2 = 1;
int gSensorInput3 = 2;

int counter = 0;
int counterMax;

bool setup(BelaContext *context, void *userData)
{
	
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

	counterMax = .25 * context->audioSampleRate;
	return true;
}

void render(BelaContext *context, void *userData)
{
	float ir1, ir1_dist;
	float ir2, ir2_dist;
	float ir3, ir3_dist;

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		if(gAudioFramesPerAnalogFrame && !(n % gAudioFramesPerAnalogFrame)) {
			// read analog inputs and update frequency and amplitude
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
			
		}

		float out = 0;

		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
		counter++;
		
		/*
		 * DISTANCE SENSOR VALUES (FOR DEBUGGING)
		 */
		if(counter == counterMax) {
			rt_printf("[sensor1: %f, sensor2 %f, sensor3 %f] \n", ir1_dist, ir2_dist, ir3_dist);
			counter = 0;
		}
	}
	

}

void cleanup(BelaContext *context, void *userData)
{

}


/**
\example analog-input/render.cpp

Connecting potentiometers
-------------------------

This sketch produces a sine tone, the frequency and amplitude of which are 
modulated by data received on the analog input pins. Before looping through each audio 
frame, we declare a value for the `frequency` and `amplitude` of our sine tone; 
we adjust these values by taking in data from analog sensors (for example potentiometers)
with `analogRead()`.

- connect a 10K pot to 3.3V and GND on its 1st and 3rd pins.
- connect the 2nd middle pin of the pot to analogIn 0.
- connect another 10K pot in the same way but with the middle pin connected to analogIn 1.

The important thing to notice is that audio is sampled twice as often as analog 
data. The audio sampling rate is 44.1kHz (44100 frames per second) and the 
analog sampling rate is 22.05kHz (22050 frames per second). Notice that we are 
processing the analog data and updating frequency and amplitude only on every 
second audio sample, since the analog sampling rate is half that of the audio.

````
if(!(n % gAudioFramesPerAnalogFrame)) {
    // Even audio samples: update frequency and amplitude from the analog inputs
    frequency = map(analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInputFrequency), 0, 1, 100, 1000);
    amplitude = analogRead(context, n/gAudioFramesPerAnalogFrame, gSensorInputAmplitude);
}
````

*/