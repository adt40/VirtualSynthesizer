#pragma once
#include "JuceHeader.h"
#include "WaveTable.h"
class BasicShapes :
	public WaveTable
{
private:
	void sinWave(float* outputArray);
	void sawWave(float* outputArray);
	void squareWave(float* outputArray);
	void triangleWave(float* outputArray);
public:
	BasicShapes();
	BasicShapes(int sampleRate);
	~BasicShapes();
	void addSamples();
};

