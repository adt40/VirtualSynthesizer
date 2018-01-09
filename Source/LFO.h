#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Modulator.h"

class LFO : public Modulator
{
public:
	LFO();

	float frequency;
	float phase;
	float amplitude;
	int currentFunction;

	float oscillate(float time); //This calculates the output given the current time and the period of this LFO
private:
	float sin(float time);
	float saw(float time);
	float square(float time);
	float triangle(float time);
};

