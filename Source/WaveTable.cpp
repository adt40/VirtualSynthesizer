#include "WaveTable.h"

WaveTable::WaveTable()
{
	init(0, 0, 0, "");
}

WaveTable::WaveTable(int sampleRate, int numStates, int numSamplesPerState, String name)
{
	init(sampleRate, numStates, numSamplesPerState, name);
}


WaveTable::~WaveTable()
{
	//free(samples);
}

void WaveTable::init(int sampleRate, int numStates, int numSamplesPerState, String name) {
	this->sampleRate = sampleRate;
	this->numStates = numStates;
	this->numSamplesPerState = numSamplesPerState;
	this->name = name;
	this->totalNumSamples = numStates * numSamplesPerState;
	samples = (float*)malloc(sizeof(float)*this->totalNumSamples);
}

void WaveTable::addSamplesToState(int state, float* samples) {
	int k = 0;
	for (int i = numSamplesPerState * state; i < numSamplesPerState * (state + 1); i++) {
		this->samples[i] = samples[k];
		k++;
	}
}

void WaveTable::getSamples(float* sampleArray) {
	for (int i = 0; i < totalNumSamples; i++) {
		sampleArray[i] = samples[i];
	}
}