#pragma once
#include "JuceHeader.h"
#include "OscillatorSound.h"
#include "WaveTable.h"
#include "BasicShapes.h"
#include "SyncSaw.h"
#include "PWMSquare.h"

class OscillatorVoice : public SynthesiserVoice
{
private:
	double amplitude;
	double* increment;
	double* phase;
	double externalVolume;
	double currentVelocity;
	double coarsePitch;
	int semitones;
	bool isPlaying;
	int numVoices; //only odd number unless we want to do more math, which I don't yet
	double voiceSpread; //from 0 to 1

	WaveTable wavetable;
	float** samples;
	double wavetablePosition;
	
	template <typename FloatType>
	void processBlock(AudioBuffer<FloatType>& outputBuffer, int startSample, int numSamples);
public:
	OscillatorVoice();
	~OscillatorVoice();
	
	bool canPlaySound(SynthesiserSound* sound) override;
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
	void renderNextBlock(AudioBuffer<double>& outputBuffer, int startSample, int numSamples) override;
    
	void setWavetablePosition(double wtPos);
	void setWavetableType(String name);
	void setVolume(double volume);
	void setCoarsePitch(double pitch);
	void incrementSemitones(int tones);
	void incrementVoices(int voices);
	void setSpread(double spread);

	double* getVolumePointer();
	double* getSpreadPointer();
	double* getWavetablePositionPointer();
};

