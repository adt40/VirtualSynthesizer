#pragma once
#include "WaveTable.h"
class SyncSaw :
	public WaveTable
{
private:
	void syncSaw(double syncAmount, float* outputArray);
public:
	SyncSaw();
	SyncSaw(int sampleRate);
	~SyncSaw();
	void addSamples();
};

