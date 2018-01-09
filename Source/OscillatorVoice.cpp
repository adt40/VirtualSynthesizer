#include "OscillatorVoice.h"

OscillatorVoice::OscillatorVoice()
{
	wavetable = BasicShapes(getSampleRate());
	numVoices = 1;
	wavetablePosition = 0.0;
	semitones = 0;
	coarsePitch = 1.0;
	isPlaying = false;
	externalVolume = 0.0;
	currentVelocity = 0.0;
	voiceSpread = 0.0;
}

OscillatorVoice::~OscillatorVoice() {}

bool OscillatorVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<OscillatorSound*> (sound) != nullptr;
}

void OscillatorVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
	samples = (float**)malloc(sizeof(float*) * numVoices);
	for (int i = 0; i < numVoices; i++) {
		samples[i] = (float*)malloc(sizeof(float) * wavetable.totalNumSamples);
	}
	phase = (double*)malloc(sizeof(double) * numVoices);
	increment = (double*)malloc(sizeof(double) * numVoices);
	
	//currentVelocity = velocity;
	currentVelocity = 0.7;

	double pitchChange = std::pow(std::pow(2, (double)(1.0 / 12.0)), semitones);
	double maxSpread = std::pow(2, (double)(1.0 / 12.0)) * voiceSpread * 0.015; //capped at 0.015 because that's where it stops sounding good
	for (int i = 0; i < numVoices; i++) {
		Random rand;
		phase[i] = rand.nextDouble() * wavetable.numSamplesPerState;
		double currentSpread = 1 + (maxSpread * (double)(i - (double)((numVoices - 1.0) / 2.0)));
		double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber) * pitchChange * currentSpread;
		double cyclesPerSample = cyclesPerSecond / getSampleRate();
		increment[i] = cyclesPerSecond * wavetable.numSamplesPerState / getSampleRate();
		wavetable.getSamples(samples[i]);
	}
	isPlaying = true;
}

void OscillatorVoice::stopNote(float velocity, bool allowTailOff)
{
	isPlaying = false;
	allowTailOff = false; //Just so I don't have to worry about this at all. This is totally gonna bite someone later lol
	if (allowTailOff) {

	}
	else {
		clearCurrentNote();
	}
}

double* OscillatorVoice::getVolumePointer() {
	return &externalVolume;
}

double* OscillatorVoice::getSpreadPointer() {
	return &voiceSpread;
}

double* OscillatorVoice::getWavetablePositionPointer() {
	return &wavetablePosition;
}


void OscillatorVoice::setWavetablePosition(double wtPos) {
    wavetablePosition = wtPos;
}

void OscillatorVoice::setWavetableType(String name) {
	if (name == "Basic Shapes") {
		wavetable = BasicShapes(getSampleRate());
	}
	else if (name == "Sync Saw") {
		wavetable = SyncSaw(getSampleRate());
	}
	else if (name == "PWM Square") {
		wavetable = PWMSquare(getSampleRate());
	}
}

void OscillatorVoice::setVolume(double volume) {
	externalVolume = volume;
}

void OscillatorVoice::setCoarsePitch(double pitch) {
	coarsePitch = pitch;
}

void OscillatorVoice::incrementSemitones(int tones) {
	semitones += tones;
}

void OscillatorVoice::incrementVoices(int voices) {
	if (numVoices + voices >= 1) {
		numVoices += voices;
	}
}

void OscillatorVoice::setSpread(double spread) {
	voiceSpread = spread;
}

void OscillatorVoice::pitchWheelMoved(int newPitchWheelValue)
{
	//Do later
}

void OscillatorVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
	//Do later
}

void OscillatorVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	processBlock(outputBuffer, startSample, numSamples);
}

void OscillatorVoice::renderNextBlock(AudioBuffer<double>& outputBuffer, int startSample, int numSamples)
{
	processBlock(outputBuffer, startSample, numSamples);
}


template <typename FloatType>
void OscillatorVoice::processBlock(AudioBuffer<FloatType>& outputBuffer, int startSample, int numSamples)
{
	if (isPlaying) {
		amplitude = currentVelocity * 0.5 * externalVolume;
		//Logger::getCurrentLogger()->writeToLog((String)externalVolume);
		for (int i = startSample; i < numSamples; i++)
		{
			float currentSample = 0;
			for (int k = 0; k < numVoices; k++) {
				currentSample += samples[k][(int)floor(phase[k]) + wavetable.numSamplesPerState * (int)(wavetablePosition * wavetable.numStates)] * amplitude;
			}
			currentSample = currentSample / numVoices;
			
			for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
				outputBuffer.addSample(i, startSample, currentSample);
			}

			for (int k = 0; k < numVoices; k++) {
				phase[k] = fmod((phase[k] + increment[k]), wavetable.numSamplesPerState);
			}
			startSample++;
		}
	}
}

