#include "Filter.h"



//Constructor
Filter::Filter()
{
    cutoff = 50;
	oldCutoff = cutoff;
    resonance = 0.1;
	oldResonance = resonance;
	currType = LOPASS;
    
}

void Filter::filter(AudioSampleBuffer& input)
{
    int numSamples = input.getNumSamples();
    int numChannels = input.getNumChannels();
    
    float* leftSamples = input.getWritePointer(0);
    float* rightSamples = input.getWritePointer(1);
    
    filterL.processSamples(leftSamples, numSamples);
    filterR.processSamples(rightSamples, numSamples);
}

void Filter::updateCutoff(double cutoffFreq) {
    switch(currType) {
        case HIPASS: {
            coeffs = IIRCoefficients::makeHighPass(sampleRate, cutoffFreq);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            cutoff = cutoffFreq;
            break;
        }
        case LOPASS: {
            coeffs = IIRCoefficients::makeLowPass(sampleRate, cutoffFreq);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            cutoff = cutoffFreq;
            break;
        }
        case BANDPASS: {
            coeffs = IIRCoefficients::makeBandPass(sampleRate, cutoffFreq);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            cutoff = cutoffFreq;
            break;
        }
    }
}

void Filter::updateResonance(double qValue) {
    switch(currType) {
        case HIPASS: {
            coeffs = IIRCoefficients::makeHighPass(sampleRate, cutoff, qValue);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            resonance = qValue;
            break;
        }
        case LOPASS: {
            coeffs = IIRCoefficients::makeLowPass(sampleRate, cutoff, qValue);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            resonance = qValue;
            break;
        }
        case BANDPASS: {
            coeffs = IIRCoefficients::makeBandPass(sampleRate, cutoff, qValue);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            resonance = qValue;
            break;
        }
    }
}

void Filter::updateFromModulation() {
	if (cutoff != oldCutoff || resonance != oldResonance) {
		switch (currType) {
			case HIPASS: {
				coeffs = IIRCoefficients::makeHighPass(sampleRate, cutoff, resonance);
				filterL.setCoefficients(coeffs);
				filterR.setCoefficients(coeffs);
				break;
			}
			case LOPASS: {
				coeffs = IIRCoefficients::makeLowPass(sampleRate, cutoff, resonance);
				filterL.setCoefficients(coeffs);
				filterR.setCoefficients(coeffs);
				break;
			}
			case BANDPASS: {
				coeffs = IIRCoefficients::makeBandPass(sampleRate, cutoff, resonance);
				filterL.setCoefficients(coeffs);
				filterR.setCoefficients(coeffs);
				break;
			}
		}
	}
}

//create appropriate filter type
//need to get samplerate data from somewhere
void Filter::changeType(int type) {
switch(type)
    {
        case 1: {
            coeffs = IIRCoefficients::makeLowPass(sampleRate, cutoff, resonance);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            currType = LOPASS;
            break;
        }
        case 2: {
            coeffs = IIRCoefficients::makeBandPass(sampleRate, cutoff, resonance);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            currType = BANDPASS;
            break;
        }
        case 3: {
            coeffs = IIRCoefficients::makeHighPass(sampleRate, cutoff, resonance);
            filterL.setCoefficients(coeffs);
            filterR.setCoefficients(coeffs);
            currType = HIPASS;
            break;
        }
    }
}

void Filter::setSampleRate(double sampleRate) {
	this->sampleRate = sampleRate;
}

double* Filter::getCutoffPointer() {
	return &cutoff;
}

double* Filter::getResonancePointer() {
	return &resonance;
}
