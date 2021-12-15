/***** sketch.js *****/



var config = {
	speed: 1000,
	duration: 750,
	filePos: 1000,
	onset: 1000,
	spray: 0,
	delay: 100,
	feedback: 0
}


//buffer to send to Bela. 8 elements: 
//five sliders: + 1 button
// 0 = speed
// 1 = grain duration
// 2 = file position
// 3 = onset time
// 4 = spray
// 5 = x position
// 6 = y position

let buffer = [0,0,0,0,0,0,0,0,0,0];
let mockArea;
let area_h = 200;
let area_l = 50;
let area_x = 600;
let area_y = 200;

let area_margin = 100;

//how many mock sensors (and thus notes of the scale) are in GU
let numAreas = 6;


let sensor_ ;

function windowResized() {
  resizeCanvas(windowWidth, windowHeight*2/3);
}

function setup() {
	createCanvas(windowWidth, windowHeight * 2 / 3);
	mockArea = createGraphics(500,200);
	mockArea2 = createGraphics(500,200);
	

	speedSlider = createSlider(0, 4000, config.speed);
	grainSizeSlider = createSlider(0, 2000, config.duration);
	filePosSlider = createSlider(0,4000, config.filePos);
	grainOffsetSlider = createSlider(0,4000, config.onset);
	spraySlider = createSlider(0,1000,804, config.spray);
	delayTimeSlider = createSlider(0,500, config.delay);
	delayFeedbackSlider = createSlider(0,900, config.feedback);
	

	//Text
	speedLabel = createP("speed");
	grainSizeLabel = createP("grain duration [ms]:");
	filePosLabel = createP("file position [ms]:");
	grainOffsetLabel=createP("onset time [ms]");
	sprayLabel=createP("spray [ms]");
	delayTimeLabel=createP("delay time [ms]");
	delayFeedbackLabel=createP("feedback ");
	
	gSpeedVal = createP("0");
	gGrainSizeVal = createP("100");
	gFilePosVal = createP("0");
	gGrainOffsetVal = createP("0");
	gSprayVal = createP("0");
	gXPosVal = createP("0");
	gYPosVal = createP("0");
	delayTimeVal = createP("0");
	delayFeedBackVal = createP("0");
	
	


	//This function will format colors and positions of the DOM elements (sliders, button and text)
	formatDOMElements();

}




function draw() {
	let lineZero = windowHeight/3;
	let lineMax= lineZero+50;
	let lineMin = lineZero-50;
	let waveStart = 20;
	let waveWidth = 400;
	let waveHeight = 100;

	
    background(5,5,255,1);

	fill(color('rgb(0,255,255)'));
    // rect(area_x,area_y,area_l,area_h);
    // rect(area_x+area_margin,area_y,area_l,area_h);
    for (let i = 0;i < numAreas; i++) {
    	rect(area_x + (area_margin*i),area_y,area_l,area_h);
    }
    
    
    
    //pass slider values
	buffer[0]=speedSlider.value()/1000;
	buffer[1]=grainSizeSlider.value();
	buffer[2]=filePosSlider.value();
	buffer[3]=grainOffsetSlider.value();
	buffer[4]=spraySlider.value();
	
	//check if cursor is above each sensor
    for (let i = 0; i < numAreas; i++) {
    	if (mouseX > area_x+(area_margin*i) && mouseX < area_l+area_x+(area_margin*i)) {
    		if (mouseY > area_y && mouseY < area_y+area_h) {
	    		let c = color('rgb(255,0,0)');
				fill(c)
	    		line(area_x+(area_margin*i),mouseY,area_l+area_x+(area_margin*i),mouseY);
	    		line(mouseX,area_y,mouseX,area_y+area_h);
	    		//x to bela
	    		buffer[5] = floor(map(mouseX,area_x,area_x+area_l,0,100));
	    		// y to bela
	    		buffer[6] = floor(map(mouseY,area_y,area_y+area_h,0,100));
	    		//sensor number to bela
	    		buffer[7] = i;
	    		// buffer[2]=buffer[6];
	    		// buffer[0]=mapPitch(buffer[5]);
	    		break;
	    	}
    	}
    	else {
    		buffer[5] = 0;
    		buffer[6] = 0;
    		buffer[7] = 0;
    	}
    }
    
    buffer[8] = delayTimeSlider.value();
    buffer[9] = delayFeedbackSlider.value();

    
    function mapPitch(input) {
    	return floor(map(input,0,1,1,12))/12 + 1;
    }
    
	
		
	// let c = color('rgb(0,0,0)');
	// fill(c)
	// rect(waveStart,lineZero,waveWidth,waveHeight);
	
	// c = color('hsl(160, 100%, 50%)');
	// fill(c);
	// sampleWidth = waveWidth*buffer[1];
	// sampleStart = waveStart+waveWidth*buffer[2];
	// rect(waveStart+(buffer[2]*waveWidth),lineZero,sampleWidth,waveHeight);
	
	//set all slider labels with their slider value
	gSpeedVal.html(buffer[0]);
	gGrainSizeVal.html(buffer[1]);
	gFilePosVal.html(buffer[2]);
	gGrainOffsetVal.html(buffer[3]);
	gSprayVal.html(buffer[4]);
	gXPosVal.html(buffer[5]);
	gYPosVal.html(buffer[6]);
	delayTimeVal.html(buffer[8]);
	delayFeedBackVal.html(buffer[9]);
	


	
	image(mockArea, 600,200);
	
	Bela.data.sendBuffer(0, 'float', buffer);

}

function formatDOMElements() {

	//Format sliders
	speedSlider.position(0,windowHeight/2 - 200);
	grainSizeSlider.position(200,windowHeight/2 - 200);
	filePosSlider.position(400,windowHeight/2 - 200);
	grainOffsetSlider.position(600,windowHeight/2 - 200);
	spraySlider.position(800,windowHeight/2 - 200);
	delayTimeSlider.position(1000,windowHeight/2 - 200);
	delayFeedbackSlider.position(1200,windowHeight/2 - 200);

	//Format text as p elements
	speedLabel.position(0,windowHeight/2 - 250);
	grainSizeLabel.position(200,windowHeight/2 - 250);
	filePosLabel.position(400,windowHeight/2 - 250);
	grainOffsetLabel.position(600,windowHeight/2 - 250);
	sprayLabel.position(800,windowHeight/2 - 250);
	delayTimeLabel.position(1000,windowHeight/2 - 250);
	delayFeedbackLabel.position(1200,windowHeight/2 - 250);
		
	gSpeedVal.position(0,windowHeight/2 - 225);
	gGrainSizeVal.position(200,windowHeight/2 - 225);
	gFilePosVal.position(400,windowHeight/2 - 225);
	gGrainOffsetVal.position(600,windowHeight/2 - 225);
	gSprayVal.position(800,windowHeight/2 - 225);
	delayTimeVal.position(1000,windowHeight/2 - 225);
	delayFeedBackVal.position(1200,windowHeight/2 - 225);
	
	createP("X: ").position(625,550);
	createP("Y: ").position(725,550);
	gXPosVal.position(650,550);
	gYPosVal.position(750,550);
	

}


