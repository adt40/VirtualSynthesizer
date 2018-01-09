/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscillatorVoice.h"
#include "OscillatorSound.h"
#include "Filter.h"
#include "LFO.h"
#include "Envelope.h"
#include "ModulationController.h"

#define FILT1CUTOFF_ID "filter1Cutoff"
#define FILT1CUTOFF_NAME "Filter1Cutoff"
#define FILT1RES_ID "filter1Res"
#define FILT1RES_NAME "Filter1Res"

#define FILT2CUTOFF_ID "filter2Cutoff"
#define FILT2CUTOFF_NAME "Filter2Cutoff"
#define FILT2RES_ID "filter2Res"
#define FILT2RES_NAME "Filter2Res"

#define FILTMIX_ID "filtMix"
#define FILTMIX_NAME "FiltMix"

#define LFO1FREQ_ID "lfo1Frequency"
#define LFO1FREQ_NAME "LFO1Frequency"
#define LFO1AMP_ID "lfo1Amp"
#define LFO1AMP_NAME "LFO1Amp"

#define LFO2FREQ_ID "lfo2Frequency"
#define LFO2FREQ_NAME "LFO2Frequency"
#define LFO2AMP_ID "lfo2Amp"
#define LFO2AMP_NAME "LFO2Amp"

//==============================================================================
/**
*/
class VirtualsynthAudioProcessor  : public AudioProcessor
{
public:
	const int numVoices = 8;
	double wavetablePosition;
    double hostBPM;
    bool isSeries;

    //==============================================================================
    VirtualsynthAudioProcessor();
    ~VirtualsynthAudioProcessor();

    //==============================================================================
	void initializeSynth();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    
    AudioProcessorValueTreeState stateTree;
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
	void updateWavetables(double wtPos, int oscNum);
	void updateWavetableType(String name, int oscNum);
	void updateVolumes(double volume, int oscNum);
	void updateSemitones(int amount, int oscNum);
	void updateVoices(int amount, int oscNum);
	void updateSpreads(double spread, int oscNum);
    void updateCutoffs(double cutoff, int filterNum);
    void updateResonances(double qValue, int filterNum);
    void updateFilterTypes(int type, int filterNum);
	void updateLFOShape(int selection, int lfoNum);
	void updateLFOFrequency(float frequency, int lfoNum);
	void updateLFOAmplitude(float amplitude, int lfoNum);
	void updateLFOPhase(float phase, int lfoNum);
	void updateEnvAttack(float time, int envNum);
	void updateEnvDecay(float time, int envNum);
	void updateEnvSustain(float val, int envNum);
    void updateIsSeries(bool seriesOn);

	void modulateFilterCutoff(Modulator* mod, int fltNum);
	void modulateFilterResonance(Modulator* mod, int fltNum);
	void modulateOscillatorSpread(Modulator* mod, int oscNum);
	void modulateOscillatorWavetable(Modulator* mod, int oscNum);
	void modulateOscillatorVolume(Modulator* mod, int oscNum);
    
	void resetENVtime(AudioSampleBuffer& buffer, int numChannels, int numSamples);

	void initModulation();
	void updateModulation(int i, double knobVal);


	void combineBuffers(AudioSampleBuffer& output, AudioSampleBuffer inBuff1, AudioSampleBuffer inBuff2, int numChannels, int numSamples);
    void mixFilters(AudioSampleBuffer& output, AudioSampleBuffer inBuff1, AudioSampleBuffer inBuff2, int numChannels, int numSamples);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
	MidiKeyboardState keyboardState;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    
    Synthesiser oscillator1;
	Synthesiser oscillator2;
    Filter filter1;
    Filter filter2;
	LFO lfo1;
	LFO lfo2;
	Envelope env1;
	Envelope env2;
	ModulationController modControl;

	float LFOtime;
    float ENVtime;
    
    Time pluginTime;
    
private:

	
	
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualsynthAudioProcessor)
};
