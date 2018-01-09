#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Modulator.h"

class Envelope : public Modulator
{
private:
	float attackVal(float normTime);
	float decayVal(float normTime);
public:
	Envelope();
	float oscillate(float time);
	float attack;
	float sustain;
	float decay;
	float release;
};

