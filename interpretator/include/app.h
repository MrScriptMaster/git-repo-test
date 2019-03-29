#pragma once

#include "ofMain.h"
#include "ofxTerminal.h"

#include <string>
#include <vector>

// usage:
// you can call:
// - amplitude
// - frequency
// - speed
// - length
//
// with some kind of value


class testApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);

	ofxTerminal<testApp> terminal;
	
	std::string setFrequency(std::vector<std::string> args);
	std::string setAmplitude(std::vector<std::string> args);
	std::string setLength(std::vector<std::string> args);
	std::string setSpeed(std::vector<std::string> args);
	
	std::string blink(std::vector<std::string> args);
	std::string setPS1(std::vector<std::string> args);
	
	float counter, speed;
	int length;
	float frequency, amplitude;
	
};