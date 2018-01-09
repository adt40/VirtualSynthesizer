#include "BasicShapes.h"

BasicShapes::BasicShapes()
: WaveTable() {
}

BasicShapes::BasicShapes(int sampleRate)
: WaveTable(sampleRate, 4, 256, "Basic Shapes") {
	addSamples();
}

BasicShapes::~BasicShapes() {}

void BasicShapes::addSamples() {
	float* samples = (float*)malloc(sizeof(float) * numSamplesPerState);
	sinWave(samples);
	addSamplesToState(0, samples);
	sawWave(samples);
	addSamplesToState(1, samples);
	squareWave(samples);
	addSamplesToState(2, samples);
	triangleWave(samples);
	addSamplesToState(3, samples);
	free(samples);
}

void BasicShapes::sinWave(float* outputArray) {
	for (int i = 0; i < numSamplesPerState; i++) {
		outputArray[i] = sin(2.0 * double_Pi * (double)i / (double)numSamplesPerState);
	}
}

void BasicShapes::sawWave(float* outputArray) {
	for (int i = 0; i < numSamplesPerState; i++) {
		outputArray[i] = -1.0 + (2.0 * (double)i / (double)numSamplesPerState);
	}
}

void BasicShapes::squareWave(float* outputArray) {
	for (int i = 0; i < numSamplesPerState; i++) {
		if (i < numSamplesPerState / 2) {
			outputArray[i] = -1.0;
		}
		else {
			outputArray[i] = 1.0;
		}
	}
}

void BasicShapes::triangleWave(float* outputArray) {
	for (int i = 0; i < numSamplesPerState; i++) {
		if (i < numSamplesPerState / 2) {
			outputArray[i] = -1.0 + (4.0 * (double)i / (double)numSamplesPerState);
		}
		else {
			outputArray[i] = 1.0 - (4.0 * (double)(i - numSamplesPerState / 2) / (double)numSamplesPerState);
		}
	}
}