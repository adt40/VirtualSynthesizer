#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class Modulator
{
public:
	virtual float oscillate(float time) = 0;
};