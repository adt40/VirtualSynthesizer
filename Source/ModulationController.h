#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ModulationParameters.h"
#include "LFO.h"
#include <stdio.h>
#include <vector>
using namespace std;

class ModulationController
{
private:
	double linearMod(ModulationParameters modParam, double normalizedVal);
	double logMod(ModulationParameters modParam, double normalizedVal);
public:
	ModulationController();
	ModulationParameters* modParams;
	int num;

	void addModulation(Modulator* source, double* value, double minValue, double maxValue, double initialValue, double finalValue, double logMidPoint);
	void updateModulation(Modulator* source, double* value, double newInitialValue, double newFinalValue);
	int getModulation(Modulator* source, double* value);
	int getSource(double* value, Modulator**& sources);
	int getValue(Modulator* source, double**& values);
	void modulateAll(float LFOtime, float ENVtime);
	double modulate(ModulationParameters modParam, double valFromSource, bool isLFO);
	
};

