#include "OscillatorSound.h"

OscillatorSound::OscillatorSound()
{
}

OscillatorSound::~OscillatorSound()
{
}

bool OscillatorSound::appliesToNote(int midiNoteNumber) { return true; }

bool OscillatorSound::appliesToChannel(int midiNoteNumber) { return true; }