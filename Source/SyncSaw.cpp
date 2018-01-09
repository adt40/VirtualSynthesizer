#include "SyncSaw.h"



SyncSaw::SyncSaw()
	:WaveTable() {
}

SyncSaw::SyncSaw(int sampleRate) 
	:WaveTable(sampleRate, 256, 256, "Sync Saw") {
	addSamples();
}

SyncSaw::~SyncSaw() {}

void SyncSaw::addSamples() {
	float* samples = (float*)malloc(sizeof(float) * numSamplesPerState);
	for (int i = 0; i < numStates; i++) {
		double syncAmount = ((i * 4.0) / (double)numStates) + 1.0; //double value between 1 and 5
		syncSaw(syncAmount, samples);
		addSamplesToState(i, samples);
	}
	free(samples);
}

void SyncSaw::syncSaw(double syncAmount, float* outputArray) {
	for (int i = 0; i < numSamplesPerState; i++) {
		outputArray[i] = -1.0 + fmodf((2.0 * syncAmount * (double)i / (double)numSamplesPerState), 2.0f);
	}
}