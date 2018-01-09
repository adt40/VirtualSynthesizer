/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VirtualsynthAudioProcessor::VirtualsynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
        stateTree(*this, nullptr)
#endif
{
	initializeSynth();
}

VirtualsynthAudioProcessor::~VirtualsynthAudioProcessor()
{
}

//==============================================================================
void VirtualsynthAudioProcessor::initializeSynth()
{
	for (int i = numVoices; --i >= 0;) {
		oscillator1.addVoice(new OscillatorVoice);
		oscillator2.addVoice(new OscillatorVoice);
        
	}
	oscillator1.addSound(new OscillatorSound());
	oscillator2.addSound(new OscillatorSound());
    
    //assertion failed - sampleRate>0 uh oh
    //filter1.changeType(1);
    //filter2.changeType(1);
    
	isSeries = true;

    //define skewfactors for parameters here? could be useful elsewhere as well
    //same math as modulation controller?
    
    NormalisableRange<float> cutoffRange(20.0f, 20000.0f, 1.0f, 0.2);
    NormalisableRange<float> resonanceRange(0.01f, 1.0f, 0.01f);
    NormalisableRange<float> filtMixRange(-1.0f, 1.0f);
    NormalisableRange<float> LFOFreqRange(0.05f, 5.0f, 0.01f, 0.2);
    NormalisableRange<float> LFOAmpRange(0.0, 1.0, 0.01);
    
    stateTree.createAndAddParameter(FILT1CUTOFF_ID, FILT1CUTOFF_NAME, FILT1CUTOFF_NAME, cutoffRange, 500, nullptr, nullptr);
    stateTree.createAndAddParameter(FILT1RES_ID, FILT1RES_NAME, FILT1RES_NAME, resonanceRange, 0.0f, nullptr, nullptr);
    
    stateTree.createAndAddParameter(FILT2CUTOFF_ID, FILT2CUTOFF_NAME, FILT2CUTOFF_NAME, cutoffRange, 500, nullptr, nullptr);
    stateTree.createAndAddParameter(FILT2RES_ID, FILT2RES_NAME, FILT2RES_NAME, resonanceRange, 0.0f, nullptr, nullptr);
    
    stateTree.createAndAddParameter(FILTMIX_ID, FILTMIX_NAME, FILTMIX_NAME, filtMixRange, 0.0f, nullptr, nullptr);
    
    stateTree.createAndAddParameter(LFO1FREQ_ID, LFO1FREQ_NAME, LFO1FREQ_NAME, LFOFreqRange, 1.0f, nullptr, nullptr);
    stateTree.createAndAddParameter(LFO1AMP_ID, LFO1AMP_NAME, LFO1AMP_NAME, LFOAmpRange, 1.0f, nullptr, nullptr);
    
    stateTree.createAndAddParameter(LFO2FREQ_ID, LFO2FREQ_NAME, LFO2FREQ_NAME, LFOFreqRange, 1.0f, nullptr, nullptr);
    stateTree.createAndAddParameter(LFO2AMP_ID, LFO2AMP_NAME, LFO2AMP_NAME, LFOAmpRange, 1.0f, nullptr, nullptr);
    
}


const String VirtualsynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VirtualsynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VirtualsynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double VirtualsynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VirtualsynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VirtualsynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VirtualsynthAudioProcessor::setCurrentProgram (int index)
{
}

const String VirtualsynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void VirtualsynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VirtualsynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	oscillator1.setCurrentPlaybackSampleRate(sampleRate);
	oscillator2.setCurrentPlaybackSampleRate(sampleRate);
	keyboardState.reset();

    filter1.setSampleRate(getSampleRate());
    //filter1.filterL.reset();
    //filter1.filterR.reset();
    filter1.changeType(1);
    
    filter2.setSampleRate(getSampleRate());
    //filter2.filterL.reset();
    //filter2.filterR.reset();
    filter1.changeType(1);
    
	initModulation();

	LFOtime = 0;
    ENVtime = 0;
}

void VirtualsynthAudioProcessor::modulateFilterCutoff(Modulator* mod, int fltNum) {
	if (fltNum == 1) {
		double* val = filter1.getCutoffPointer();
		modControl.addModulation(mod, val, 20.0, 20000.0, *val, *val, 500.0);
	}
	else if (fltNum == 2) {
		double* val = filter2.getCutoffPointer();
		modControl.addModulation(mod, val, 20.0, 20000.0, *val, *val, 500.0);
	}
}

void VirtualsynthAudioProcessor::modulateFilterResonance(Modulator* mod, int fltNum) {

	if (fltNum == 1) {
		double* val = filter1.getResonancePointer();
		modControl.addModulation(mod, val, 0.0, 1.0, *val, *val, 0.0);
	}
	else if (fltNum == 2) {
		double* val = filter2.getResonancePointer();
		modControl.addModulation(mod, val, 0.0, 1.0, *val, *val, 0.0);
	}
}

void VirtualsynthAudioProcessor::modulateOscillatorSpread(Modulator* mod, int oscNum) {
	if (oscNum == 1) {
		int numVoices = oscillator1.getNumVoices();
		for (int i = 0; i < numVoices; i++) {
			OscillatorVoice* voice = (OscillatorVoice*)oscillator1.getVoice(i);
			double* val = voice->getSpreadPointer();
			modControl.addModulation(mod, val, 0.0, 1.0, *val, *val, 0.0);
		}
	}
	else if (oscNum == 2) {
		int numVoices = oscillator2.getNumVoices();
		for (int i = 0; i < numVoices; i++) {
			OscillatorVoice* voice = (OscillatorVoice*)oscillator2.getVoice(i);
			double* val = voice->getSpreadPointer();
			modControl.addModulation(mod, val, 0.0, 1.0, *val, *val, 0.0);
		}
	}
}

void VirtualsynthAudioProcessor::modulateOscillatorWavetable(Modulator* mod, int oscNum) {
	if (oscNum == 1) {
		int numVoices = oscillator1.getNumVoices();
		for (int i = 0; i < numVoices; i++) {
			OscillatorVoice* voice = (OscillatorVoice*)oscillator1.getVoice(i);
			double* val = voice->getWavetablePositionPointer();
			modControl.addModulation(mod, val, 0.0, 0.99, *val, *val, 0.0);
		}
	}
	else if (oscNum == 2) {
		int numVoices = oscillator2.getNumVoices();
		for (int i = 0; i < numVoices; i++) {
			OscillatorVoice* voice = (OscillatorVoice*)oscillator2.getVoice(i);
			double* val = voice->getWavetablePositionPointer();
			modControl.addModulation(mod, val, 0.0, 0.99, *val, *val, 0.0);
		}
	}
	
}

void VirtualsynthAudioProcessor::modulateOscillatorVolume(Modulator* mod, int oscNum) {
	if (oscNum == 1) {
		int numVoices = oscillator1.getNumVoices();
		for (int i = 0; i < numVoices; i++) {
			OscillatorVoice* voice = (OscillatorVoice*)oscillator1.getVoice(i);
			double* val = voice->getVolumePointer();
			modControl.addModulation(mod, val, 0.0, 1.0, *val, *val, 0.0);
		}
	}
	else if (oscNum == 2) {
		int numVoices = oscillator2.getNumVoices();
		for (int i = 0; i < numVoices; i++) {
			OscillatorVoice* voice = (OscillatorVoice*)oscillator2.getVoice(i);
			double* val = voice->getVolumePointer();
			modControl.addModulation(mod, val, 0.0, 1.0, *val, *val, 0.0);
		}
	}
}


void VirtualsynthAudioProcessor::initModulation() {
	modulateOscillatorSpread(&lfo1, 1);
	modulateOscillatorWavetable(&lfo1, 1);
	modulateOscillatorVolume(&lfo1, 1);
	modulateOscillatorSpread(&lfo1, 2);
	modulateOscillatorWavetable(&lfo1, 2);
	modulateOscillatorVolume(&lfo1, 2);
	modulateFilterCutoff(&lfo1, 1);
	modulateFilterResonance(&lfo1, 1);
	modulateFilterCutoff(&lfo1, 2);
	modulateFilterResonance(&lfo1, 2);
	
	modulateOscillatorSpread(&lfo2, 1);
	modulateOscillatorWavetable(&lfo2, 1);
	modulateOscillatorVolume(&lfo2, 1);
	modulateOscillatorSpread(&lfo2, 2);
	modulateOscillatorWavetable(&lfo2, 2);
	modulateOscillatorVolume(&lfo2, 2);
	modulateFilterCutoff(&lfo2, 1);
	modulateFilterResonance(&lfo2, 1);
	modulateFilterCutoff(&lfo2, 2);
	modulateFilterResonance(&lfo2, 2);
	
	modulateOscillatorSpread(&env1, 1);
	modulateOscillatorWavetable(&env1, 1);
	modulateOscillatorVolume(&env1, 1);
	modulateOscillatorSpread(&env1, 2);
	modulateOscillatorWavetable(&env1, 2);
	modulateOscillatorVolume(&env1, 2);
	modulateFilterCutoff(&env1, 1);
	modulateFilterResonance(&env1, 1);
	modulateFilterCutoff(&env1, 2);
	modulateFilterResonance(&env1, 2);

	modulateOscillatorSpread(&env2, 1);
	modulateOscillatorWavetable(&env2, 1);
	modulateOscillatorVolume(&env2, 1);
	modulateOscillatorSpread(&env2, 2);
	modulateOscillatorWavetable(&env2, 2);
	modulateOscillatorVolume(&env2, 2);
	modulateFilterCutoff(&env2, 1);
	modulateFilterResonance(&env2, 1);
	modulateFilterCutoff(&env2, 2);
	modulateFilterResonance(&env2, 2);
}

void VirtualsynthAudioProcessor::updateModulation(int i, double knobVal) {
	double* value;
	Modulator* source;
	switch (i) {
	case 0:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getSpreadPointer();
			source = &lfo1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 1:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getWavetablePositionPointer();
			source = &lfo1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 2:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getVolumePointer();
			source = &lfo1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 3:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getSpreadPointer();
			source = &lfo1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 4:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getWavetablePositionPointer();
			source = &lfo1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 5:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getVolumePointer();
			source = &lfo1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 6:
		value = filter1.getCutoffPointer();
		source = &lfo1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 7:
		value = filter1.getResonancePointer();
		source = &lfo1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 8:
		value = filter2.getCutoffPointer();
		source = &lfo1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 9:
		value = filter2.getResonancePointer();
		source = &lfo1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 10:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getSpreadPointer();
			source = &lfo2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 11:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getWavetablePositionPointer();
			source = &lfo2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 12:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getVolumePointer();
			source = &lfo2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 13:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getSpreadPointer();
			source = &lfo2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 14:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getWavetablePositionPointer();
			source = &lfo2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 15:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getVolumePointer();
			source = &lfo2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 16:
		value = filter1.getCutoffPointer();
		source = &lfo2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 17:
		value = filter1.getResonancePointer();
		source = &lfo2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 18:
		value = filter2.getCutoffPointer();
		source = &lfo2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 19:
		value = filter2.getResonancePointer();
		source = &lfo2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 20:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getSpreadPointer();
			source = &env1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 21:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getWavetablePositionPointer();
			source = &env1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 22:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getVolumePointer();
			source = &env1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 23:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getSpreadPointer();
			source = &env1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 24:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getWavetablePositionPointer();
			source = &env1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 25:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getVolumePointer();
			source = &env1;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 26:
		value = filter1.getCutoffPointer();
		source = &env1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 27:
		value = filter1.getResonancePointer();
		source = &env1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 28:
		value = filter2.getCutoffPointer();
		source = &env1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 29:
		value = filter2.getResonancePointer();
		source = &env1;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 30:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getSpreadPointer();
			source = &env2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 31:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getWavetablePositionPointer();
			source = &env2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 32:
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator1.getVoice(i))->getVolumePointer();
			source = &env2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 33:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getSpreadPointer();
			source = &env2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 34:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getWavetablePositionPointer();
			source = &env2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 35:
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			value = ((OscillatorVoice*)oscillator2.getVoice(i))->getVolumePointer();
			source = &env2;
			if (source != NULL) {
				modControl.updateModulation(source, value, DBL_MAX, knobVal);
			}
		}
		break;
	case 36:
		value = filter1.getCutoffPointer();
		source = &env2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 37:
		value = filter1.getResonancePointer();
		source = &env2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 38:
		value = filter2.getCutoffPointer();
		source = &env2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	case 39:
		value = filter2.getResonancePointer();
		source = &env2;
		if (source != NULL) {
			modControl.updateModulation(source, value, DBL_MAX, knobVal);
		}
		break;
	default:
		break;
	}
}


void VirtualsynthAudioProcessor::releaseResources()
{
	keyboardState.reset();
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VirtualsynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VirtualsynthAudioProcessor::updateWavetables(double wtPos, int oscNum) {
	if (oscNum == 1) {
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator1.getVoice(i))->setWavetablePosition(wtPos);
			double* value = ((OscillatorVoice*)oscillator1.getVoice(i))->getWavetablePositionPointer();
			Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
			int count = modControl.getSource(value, sources);
			for (int i = 0; i < count; i++) {
				modControl.updateModulation(sources[i], value, wtPos, DBL_MAX);
			}
			free(sources);
		}
	}
	else if (oscNum == 2) {
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator2.getVoice(i))->setWavetablePosition(wtPos);
			double* value = ((OscillatorVoice*)oscillator2.getVoice(i))->getWavetablePositionPointer();
			Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
			int count = modControl.getSource(value, sources);
			for (int i = 0; i < count; i++) {
				modControl.updateModulation(sources[i], value, wtPos, DBL_MAX);
			}
			free(sources);
		}
	}
}

void VirtualsynthAudioProcessor::updateWavetableType(String name, int oscNum)
{
	if (oscNum == 1) {
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator1.getVoice(i))->setWavetableType(name);
		}
	}
	else if (oscNum == 2) {
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator2.getVoice(i))->setWavetableType(name);
		}
	}
}

void VirtualsynthAudioProcessor::updateVolumes(double volume, int oscNum) {
	if (oscNum == 1) {
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator1.getVoice(i))->setVolume(volume);
			double* value = ((OscillatorVoice*)oscillator1.getVoice(i))->getVolumePointer();
			Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
			int count = modControl.getSource(value, sources);
			for (int i = 0; i < count; i++) {
				modControl.updateModulation(sources[i], value, volume, DBL_MAX);
			}
			free(sources);
		}
	}
	else if (oscNum == 2) {
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator2.getVoice(i))->setVolume(volume);
			double* value = ((OscillatorVoice*)oscillator2.getVoice(i))->getVolumePointer();
			Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
			int count = modControl.getSource(value, sources);
			for (int i = 0; i < count; i++) {
				modControl.updateModulation(sources[i], value, volume, DBL_MAX);
			}
			free(sources);
		}
	}
}

void VirtualsynthAudioProcessor::updateSemitones(int amount, int oscNum) {
	if (oscNum == 1) {
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator1.getVoice(i))->incrementSemitones(amount);
		}
	}
	if (oscNum == 2) {
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator2.getVoice(i))->incrementSemitones(amount);
		}
	}
}

void VirtualsynthAudioProcessor::updateVoices(int amount, int oscNum) {
	if (oscNum == 1) {
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator1.getVoice(i))->incrementVoices(amount);
		}
	}
	if (oscNum == 2) {
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator2.getVoice(i))->incrementVoices(amount);
		}
	}
}

void VirtualsynthAudioProcessor::updateSpreads(double spread, int oscNum) {
	if (oscNum == 1) {
		for (int i = 0; i < oscillator1.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator1.getVoice(i))->setSpread(spread);
			double* value = ((OscillatorVoice*)oscillator1.getVoice(i))->getSpreadPointer();
			Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
			int count = modControl.getSource(value, sources);
			for (int i = 0; i < count; i++) {
				modControl.updateModulation(sources[i], value, spread, DBL_MAX);
			}
			free(sources);
		}
	}
	else if (oscNum == 2) {
		for (int i = 0; i < oscillator2.getNumVoices(); i++) {
			((OscillatorVoice*)oscillator2.getVoice(i))->setSpread(spread);
			double* value = ((OscillatorVoice*)oscillator2.getVoice(i))->getSpreadPointer();
			Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
			int count = modControl.getSource(value, sources);
			for (int i = 0; i < count; i++) {
				modControl.updateModulation(sources[i], value, spread, DBL_MAX);
			}
			free(sources);
		}
	}
}

void VirtualsynthAudioProcessor::updateFilterTypes(int type, int filterNum) {
    if(filterNum == 1) {
        filter1.changeType(type);
    }
    else if(filterNum == 2) {
        filter2.changeType(type);
    }
}

void VirtualsynthAudioProcessor::updateCutoffs(double cutoffFreq, int filterNum) {
    if(filterNum == 1) {
        filter1.updateCutoff(cutoffFreq);
		double* value = filter1.getCutoffPointer();
		Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
		int count = modControl.getSource(value, sources);
		for (int i = 0; i < count; i++) {
			modControl.updateModulation(sources[i], value, cutoffFreq, DBL_MAX);
		}
		free(sources);
    }
    else if(filterNum == 2) {
        filter2.updateCutoff(cutoffFreq);
		double* value = filter2.getCutoffPointer();
		Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
		int count = modControl.getSource(value, sources);
		for (int i = 0; i < count; i++) {
			modControl.updateModulation(sources[i], value, cutoffFreq, DBL_MAX);
		}
		free(sources);
    }
    
}

void VirtualsynthAudioProcessor::updateResonances(double qValue, int filterNum) {
    if(filterNum == 1) {
        filter1.updateResonance(qValue);
		double* value = filter1.getResonancePointer();
		Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
		int count = modControl.getSource(value, sources);
		for (int i = 0; i < count; i++) {
			modControl.updateModulation(sources[i], value, qValue, DBL_MAX);
		}
		free(sources);
    }
    else if(filterNum == 2) {
        filter2.updateResonance(qValue);
		double* value = filter2.getResonancePointer();
		Modulator** sources = (Modulator**)malloc(sizeof(Modulator*) * 4);
		int count = modControl.getSource(value, sources);
		for (int i = 0; i < count; i++) {
			modControl.updateModulation(sources[i], value, qValue, DBL_MAX);
		}
		free(sources);
    }
}

void VirtualsynthAudioProcessor::updateLFOShape(int selection, int lfoNum) {
	if (lfoNum == 1) {
		lfo1.currentFunction = selection;
	}
	else if (lfoNum == 2) {
		lfo2.currentFunction = selection;
	}
	
}

void VirtualsynthAudioProcessor::updateLFOFrequency(float frequency, int lfoNum) {
	if (lfoNum == 1) {
		lfo1.frequency = frequency;
	}
	else if (lfoNum == 2) {
		lfo2.frequency= frequency;
	}
}

void VirtualsynthAudioProcessor::updateLFOAmplitude(float amplitude, int lfoNum) {
	if (lfoNum == 1) {
		lfo1.amplitude = amplitude;
	}
	else if (lfoNum == 2) {
		lfo2.amplitude = amplitude;
	}
	
}

void VirtualsynthAudioProcessor::updateLFOPhase(float phase, int lfoNum) {
	if (lfoNum == 1) {
		lfo1.phase = phase;
	}
	else if (lfoNum == 2) {
		lfo2.phase = phase;
	}
	
}

void VirtualsynthAudioProcessor::updateEnvAttack(float time, int envNum) {
	if (envNum == 1) {
		env1.attack = time;
	}
	else if (envNum == 2) {
		env2.attack = time;
	}
}
void VirtualsynthAudioProcessor::updateEnvDecay(float time, int envNum) {
	if (envNum == 1) {
		env1.decay = time;
	}
	else if (envNum == 2) {
		env2.decay = time;
	}
}
void VirtualsynthAudioProcessor::updateEnvSustain(float val, int envNum) {
	if (envNum == 1) {
		env1.sustain = val;
	}
	else if (envNum == 2) {
		env2.sustain = val;
	}
}

void VirtualsynthAudioProcessor::updateIsSeries(bool seriesOn) {
    isSeries = seriesOn;
}

void VirtualsynthAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	LFOtime += 0.01;
    ENVtime += 0.01;
	modControl.modulateAll(LFOtime, ENVtime);

	filter1.updateFromModulation();
	filter2.updateFromModulation();

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
	const int numSamples = buffer.getNumSamples();
	
	AudioSampleBuffer tempBuff1;
	AudioSampleBuffer tempBuff2;
	tempBuff1.setSize(buffer.getNumChannels(), numSamples, false, false, false);
	tempBuff2.setSize(buffer.getNumChannels(), numSamples, false, false, false);

	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, numSamples);
		tempBuff1.clear(i, 0, numSamples);
		tempBuff2.clear(i, 0, numSamples);
	}
	
	keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);
	
	oscillator1.renderNextBlock(tempBuff1, midiMessages, 0, numSamples);
	oscillator2.renderNextBlock(tempBuff2, midiMessages, 0, numSamples);
    
    mixFilters(buffer, tempBuff1, tempBuff2, totalNumOutputChannels, numSamples);
	
	resetENVtime(buffer, totalNumOutputChannels, numSamples);
}

void VirtualsynthAudioProcessor::combineBuffers(AudioSampleBuffer& output, AudioSampleBuffer inBuff1, AudioSampleBuffer inBuff2, int numChannels, int numSamples) {
	for (int i = 0; i < numChannels; i++) {
		const float* arrBuff1 = inBuff1.getReadPointer(i);
		const float* arrBuff2 = inBuff2.getReadPointer(i);
		for (int k = 0; k < numSamples; k++) {
			float averageSample = (arrBuff1[k] + arrBuff2[k]) / 2;
			output.addSample(i, k, fminf(averageSample, 0.99f)); //hard cap on amplitude
		}
	}
}

void VirtualsynthAudioProcessor::mixFilters(AudioSampleBuffer& output, AudioSampleBuffer inBuff1, AudioSampleBuffer inBuff2, int numChannels, int numSamples) {
    for (int i = 0; i < numChannels; i++) {
        const float* arrBuff1 = inBuff1.getReadPointer(i);
        const float* arrBuff2 = inBuff2.getReadPointer(i);
        for(int k = 0; k < numSamples; k++) {
            inBuff1.addSample(i, k, fminf(arrBuff1[k], 0.99f));
            inBuff2.addSample(i, k, fminf(arrBuff2[k], 0.99f));
            
        }
        
    }
    if(isSeries) {
     
        combineBuffers(output, inBuff1, inBuff2, numChannels, numSamples);
     
        filter1.filter(output);
        filter2.filter(output);
     
     
    } else {
         
        filter1.filter(inBuff1);
        filter2.filter(inBuff2);
     
        combineBuffers(output, inBuff1, inBuff2, numChannels, numSamples);
    }
    
    
}

void VirtualsynthAudioProcessor::resetENVtime(AudioSampleBuffer & buffer, int numChannels, int numSamples)
{
	for (int i = 0; i < numChannels; i++) {
		for (int k = 0; k < numSamples; k++) {
			if (buffer.getSample(i, k) != 0) {
				return;
			}
		}
	}
	ENVtime = 0;
}


//==============================================================================
bool VirtualsynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VirtualsynthAudioProcessor::createEditor()
{
    return new VirtualsynthAudioProcessorEditor (*this);
}

//==============================================================================
void VirtualsynthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VirtualsynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VirtualsynthAudioProcessor();
}
