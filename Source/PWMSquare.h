#pragma once
#include "WaveTable.h"
class PWMSquare :
	public WaveTable
{
private:
	void pwmSquare(int currentState, float* outputArray);
public:
	PWMSquare();
	PWMSquare(int sampleRate);
	~PWMSquare();
	void addSamples();
};