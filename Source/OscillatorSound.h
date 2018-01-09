#pragma once
#include "JuceHeader.h"

class OscillatorSound : public SynthesiserSound
{
public:
	OscillatorSound();
	~OscillatorSound();
	bool appliesToNote(int midiNoteNumber) override;
	bool appliesToChannel(int midiNoteNumber) override;
};

