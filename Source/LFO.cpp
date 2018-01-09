#include "LFO.h"

//Constructor
LFO::LFO()
{
	currentFunction = 1;
	frequency = 1.0f;
	amplitude = 1.0f;
	phase = 0.0f;
}

float LFO::oscillate(float time) 
{
	if (currentFunction == 1) {
		return sin(time);
	}
	else if (currentFunction == 2) {
		return saw(time);
	}
	else if (currentFunction == 3) {
		return square(time);
	}
	else if (currentFunction == 4) {
		return triangle(time);
	}
	else {
		return 0.0f;
	}
}

float LFO::sin(float time) {
	return amplitude * std::sin(2 * double_Pi * (1/frequency) * (time + phase));
}

float LFO::saw(float time) {
	return amplitude * (-1.0f + 2 * fmod(time + phase, frequency) / frequency);
}

float LFO::square(float time) {
	if (fmod(time + phase, frequency) < frequency / 2) {
		return -amplitude;
	}
	else {
		return amplitude;
	}
}

float LFO::triangle(float time) {
	if (fmod(time + phase, frequency) < frequency / 2) {
		return amplitude * (-1.0 + (4.0 * fmod(time + phase, frequency) / frequency));
	}
	else {
		return amplitude * (3.0 - (4.0 * fmod(time + phase, frequency) / frequency));
	}
}