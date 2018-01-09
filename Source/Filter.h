#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class Filter
{
private:
	
	double cutoff, resonance;
	double oldCutoff, oldResonance;
	double sampleRate;
    IIRCoefficients coeffs;
    
	
public:
	enum FType { HIPASS, LOPASS, BANDPASS };
	Filter();
	void filter(AudioSampleBuffer& input);
	void setSampleRate(double sampleRate);
	void changeType(int type);
    void updateCutoff(double cutoffFreq);
    void updateResonance(double qValue);
	void updateFromModulation();
    FType currType;

	double* getCutoffPointer();
	double* getResonancePointer();
    
    IIRFilter filterL;
    IIRFilter filterR;
};

