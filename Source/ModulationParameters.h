#include "../JuceLibraryCode/JuceHeader.h"
#include "Modulator.h"

class ModulationParameters {
private:
public:
	ModulationParameters(Modulator* source, double* valToMod, double minValue, double maxValue, double initialValue, double finalValue, double logMidPoint);
	Modulator* source;
	double* valToMod;
	double minValue;
	double maxValue;
	double initialValue;
	double finalValue;
	double logMidPoint;
	double mapValue;
};