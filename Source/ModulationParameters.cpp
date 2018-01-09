#include "ModulationParameters.h"

/*
Set logMidPoint to 0 if not a skewed parameter
*/
ModulationParameters::ModulationParameters(Modulator* source, double * valToMod, double minValue, double maxValue, double initialValue, double finalValue, double logMidPoint)
{
	this->source = source;
	this->valToMod = valToMod;
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->initialValue = initialValue;
	this->finalValue = finalValue;
	this->logMidPoint = logMidPoint;
	mapValue = 0;
}


