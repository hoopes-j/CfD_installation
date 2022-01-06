### Important to things to consider when running this new polyphonic version:

1: I wrote all of this while working with 2 of the short range sensors, so the third sensor won't work by default, 
  although the logic to retrieve the amplitude from the third is included in the new function calcAmplitudeArray. I reset the ranges back to the ranges for the long range sensor so that part should be all set.
I also haven't tested this in like at least a week, but everything should be working okay, hopefully lol.
  
<br>

2: At this point, the GUI is disconnected - it was causing a lot of problems when I was refactoring, and I wanted to try to start decoupling the program from the GUI since that how it will be in the final installation. Of course, if we think it will still be helpful it will be fairly simple to reconnect

3: all of the granular audio processing is now moved outside of the render file and handled in the class GranularStream. Distance to Amplitude calculations (and all other parameter calculation) is still being done inside render, then being passed to each respective stream (we should keep it this way).

Each Granular Stream Instance basically just does everything that render was doing before, but now we can map each sensor to its own granular stream (yayy polyphony).

You'll see in line 307, we get all the audio by calling GranularStream.process(). Eventually this should loop over all the streams, and sum the outputs instead of just hardcoding it in like it is here.


### Quick list of some next steps:
- In render, make a vector of {GranularStreams} instead of individual hardcoded variables.

- Each granular stream uses a different audio file 

- Implement amplitude envelopes and possibly LFOs (this should be pretty easy, it was basically already done, and im planning on grinding out basically all of this functionality this weekend. 

- Finalize how many sensors we are gonna use in the final thing and get those ordered asap (they shipped in like a day so should be fine)
- Record Vocal samples 
- I think thats it, off the top of my head 

### OTHER NOTES:
-the TF-LUNA long range sensors are on the way



 
