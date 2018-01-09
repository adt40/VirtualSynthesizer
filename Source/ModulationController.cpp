#include "ModulationController.h"
#include "Envelope.h"


//Constructor
ModulationController::ModulationController()
{
	modParams = (ModulationParameters*)malloc(sizeof(ModulationParameters));
	num = 0;
}

void ModulationController::addModulation(Modulator* source, double* value, double minValue, double maxValue, double initialValue, double finalValue, double logMidPoint)
{
	ModulationParameters* tempModParams = (ModulationParameters*)malloc(sizeof(ModulationParameters) * num);

	for (int i = 0; i < num; i++) {
		tempModParams[i] = ModulationParameters(modParams[i]);
	}

	free(modParams);

	modParams = (ModulationParameters*)malloc(sizeof(ModulationParameters) * (num + 1));

	for (int i = 0; i < num; i++) {
		modParams[i] = ModulationParameters(tempModParams[i]);
	}

	modParams[num] = ModulationParameters(source, value, minValue, maxValue, initialValue, finalValue, logMidPoint);
	
	num++;

	free(tempModParams);
}

/*
set newInitialValue or newFinalValue to double max value to not change it
*/
void ModulationController::updateModulation(Modulator* source, double* value, double newInitialValue, double newFinalValue) {
	int mod = getModulation(source, value);
	if (mod != -1) {
		if (newInitialValue != DBL_MAX) {
			modParams[mod].initialValue = newInitialValue;
			double mappedValue = modParams[mod].mapValue * (modParams[mod].maxValue - modParams[mod].initialValue) + modParams[mod].initialValue;
			modParams[mod].finalValue = mappedValue;
		}
		if (newFinalValue != DBL_MAX) {
			modParams[mod].mapValue = newFinalValue;
			double mappedValue = newFinalValue * (modParams[mod].maxValue - modParams[mod].initialValue) + modParams[mod].initialValue;
			modParams[mod].finalValue = mappedValue;
		}
	}
	else {
		Logger::getCurrentLogger()->writeToLog("modulation doesn't exist!");
	}
}

int ModulationController::getModulation(Modulator* source, double* value) {
	for (int i = 0; i < num; i++) {
		bool sourceEq = modParams[i].source == source;
		bool valEq = modParams[i].valToMod == value;
		if (sourceEq && valEq) {
			return i;
		}
	}
	return -1;
}

int ModulationController::getSource(double* value, Modulator**& sources) {
	int sourceNum = 0;
	for (int i = 0; i < num; i++) {
		if (modParams[i].valToMod == value) {
			sources[sourceNum] = modParams[i].source;
			sourceNum++;
		}
	}
	return sourceNum;
}

int ModulationController::getValue(Modulator* source, double**& values) {
	int valNum = 0;
	for (int i = 0; i < num; i++) {
		if (modParams[i].source == source) {
			values[valNum] = modParams[i].valToMod;
			valNum++;
		}
	}
	return valNum;
}

void ModulationController::modulateAll(float LFOtime, float ENVtime)
{
	for (int i = 0; i < num; i++) {
		*(modParams[i].valToMod) = modParams[i].initialValue;
	}
	for (int i = 0; i < num; i++) {

        if (modParams[i].finalValue == modParams[i].initialValue) {
            continue;
			//save some cycles by skiping over modulations that aren't actually on
        }
        if (typeid(*modParams[i].source) == typeid(Envelope)) {
            *(modParams[i].valToMod) += modulate(modParams[i], modParams[i].source->oscillate(ENVtime), false);
        }
        else {
            *(modParams[i].valToMod) += modulate(modParams[i], modParams[i].source->oscillate(LFOtime), true);
        }
        
    }
}

double ModulationController::modulate(ModulationParameters modParam, double valFromSource, bool isLFO)
{
	
	
	if (isLFO) {
		valFromSource = ((valFromSource + 1) / 2); //set between 0 and 1
	}

	//If multiple things are modulating the same parameter, this can go above 1 or below 0 so enforce the limit
	if (valFromSource > 1) {
		valFromSource = 1;
	}
	else if (valFromSource < 0) {
		valFromSource = 0;
	}

	return linearMod(modParam, valFromSource);
}

double ModulationController::linearMod(ModulationParameters modParam, double normalizedVal)
{
	//return modParam.initialValue + (normalizedVal * (modParam.finalValue - modParam.initialValue));
	return (normalizedVal * (modParam.finalValue - modParam.initialValue));
}

double ModulationController::logMod(ModulationParameters modParam, double normalizedVal)
{
	//code adapted from juce_Slider.cpp

	//calculate skew factor
	double logBase = (modParam.logMidPoint - modParam.minValue) / (modParam.maxValue - modParam.minValue);
	double skewFactor = std::log(0.5) / std::log(logBase);

	//convert initial value to a log proportion on the slider (from 0 to 1, but logarithmically placed in this range)
	double initValue = std::pow((modParam.initialValue - modParam.minValue) / (modParam.maxValue - modParam.minValue), skewFactor);
	double initDistanceFromMiddle = 2.0 * initValue - 1.0;
	double initProportion = (1.0 + std::pow(std::abs(initDistanceFromMiddle), skewFactor) * (initDistanceFromMiddle < 0 ? -1 : 1)) / 2.0;

	//convert final value to a log proportion on the slider (from 0 to 1, but logarithmically placed in this range)
	double finalValue = std::pow((modParam.finalValue - modParam.minValue) / (modParam.maxValue - modParam.minValue), skewFactor);
	double finalDistanceFromMiddle = 2.0 * finalValue - 1.0;
	double finalProportion = (1.0 + std::pow(std::abs(finalDistanceFromMiddle), skewFactor) * (finalDistanceFromMiddle < 0 ? -1 : 1)) / 2.0;

	//increase proportion by the amount given from the source
	double newProportion = initProportion + (normalizedVal * (finalProportion - initProportion));

	//convert proportion back to a value on the slider
	double skewedProportion = std::exp(std::log(newProportion) / skewFactor);
	double skewedValue = modParam.minValue + (modParam.maxValue - modParam.minValue) * skewedProportion;

	return skewedValue;
}

