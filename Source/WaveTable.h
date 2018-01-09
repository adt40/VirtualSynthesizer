#pragma once
#include "JuceHeader.h"

class WaveTable
{
protected:
	int sampleRate;
	float* samples;
	void init(int sampleRate, int numStates, int numSamplesPerState, String name);
	void addSamplesToState(int state, float* samples);
public:
	WaveTable();
	WaveTable(int sampleRate, int numStates, int numSamplesPerState, String name);
	~WaveTable();
	String name;
	int numStates;
	int numSamplesPerState;
	int totalNumSamples;
	void getSamples(float* sampleArray);
};

