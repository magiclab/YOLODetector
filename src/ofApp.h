#pragma once

#include "ofMain.h"

#include "ofxDarknet.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int  key);

	ofxDarknet darknet;
	ofVideoGrabber video;
	float thresh;
	float threshInc;
};
