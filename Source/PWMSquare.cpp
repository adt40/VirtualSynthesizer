#include "PWMSquare.h"



PWMSquare::PWMSquare()
	:WaveTable() {
}

PWMSquare::PWMSquare(int sampleRate)
	: WaveTable(sampleRate, 256, 256, "PWM Square") {
	addSamples();
}

PWMSquare::~PWMSquare() {}

void PWMSquare::addSamples()
{
	float* samples = (float*)malloc(sizeof(float) * numSamplesPerState);
	for (int i = 0; i < numStates; i++) {
		pwmSquare(i, samples);
		addSamplesToState(i, samples);
	}
	free(samples);
}

void PWMSquare::pwmSquare(int currentState, float * outputArray) {
	for (int i = 0; i < numSamplesPerState; i++) {
		if (i > currentState) {
			outputArray[i] = 1;
		}
		else {
			outputArray[i] = -1;
		}
	}
}