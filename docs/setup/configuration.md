# Additional Configuration

There are some additional parameters that can be set as well.

`ROTATION_MINUTES` 

This defines how many minutes will pass until the sentences switch. The default is 2 minutes

`MUSICAL_MODE`

The new software includes some aspects of musicality - 
if you would like to turn these off, set this to false.

*The code included in the configuration file is shown below:*

``` c++
/*
This file contains all high level parameters needed to configure the installation 
More details are given in the setup guide, as well as in the comments below
*/

/*
Set this to true in order to inspect the distance readings from the sensors 
distance readings will be printed in the console with the following format:
sensor_1: [distance from sensor1]    sensor_2: [distance from sensor2]
It will probably be helpful to use two people for this task. 
Have one person stand where the distance cutoff should be, and have the other person inspect the distance outputs.
*/
#define PRINT_DISTANCE false

/*
Defines the Maxmimum distance that should be triggerable
In order to fine tune this distance to the space, set PRINT_DISTANCE to true to inspect the desired distance values
*/
#define MAX_DISTANCE 400

/*
Whether or not to use the "musical" version of the installation
The musical version will fade in granular synthesis of piano notes towards the furthest end of the distance range
*/
#define MUSICAL_MODE true

/*
Defines how many minutes to spend on each sentence, before rotating to the next pair
*/
#define ROTATION_MINUTES 2



/* BACKUP VALUES 

In case the values were changed, and you would like to restore to the original defaults, they are listed here below for your convienence:


#define PRINT_DISTANCE false

#define MAX_DISTANCE 200

#define MUSICAL_MODE true

#define ROTATION_MINUTES 3

*/
```