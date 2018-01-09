# VirtualSynthesizer

## Developed by Austin Toot and Drew Blazewicz

### Summary
This is the project by Austin Toot and Drew Blazewicz for Case Western's senior project in computer science course. It is a rudimentary wavetable synthesizer, coded in C++ using the JUCE library. It's far from perfect, with notable bugs being memory leaks that occur every time you play a note, and some parameters can't really be modulated by multiple sources without crashing. But, 95% of it works pretty well considering we've never coded in C++ or worked with digital signal processing before this. The following features section is copy-pasted from our final report to illustrate what each part of the synth does.

### Features

#### Oscillators
The heart of the synthesizer is the noise generating oscillators. It contains two oscillators which can each produce a variable number of signals (voices) which can be pitch-offset (spread) to create a thicker sound. The oscillators generate noise by filling an audio buffer with samples. Samples are generated through wavetable functions allowing for multiple types of signal generation beyond a simple sine wave.
Oscillators have two primary parameters beyond number of voices and spread. The first is ‘wavetable’, which allows the user to select a desired wavetable from a list of predefined functions. Which wavetables are available can be selected through the drop down menu to the right of the oscillator. ‘Volume’ is simply the amplitude multiplier on the generated samples, ranging from 0 to 1. 

#### Filters
Filters provide the primary control over shaping the generated sound. The drop down menu for filter types allows the user to select either low-pass, high-pass, or band-pass filters for sculpting the sound. Each filter provides individual control over cutoff point and resonance (Q value).

The series/parallel toggle button changes the signal flow through the filters. In parallel mode each oscillator is affected independently; Filter 1 controls Oscillator 1’s signal and Filter 2 controls Oscillator 2’s signal. In series mode the audio buffers from each oscillator are first combined and then passed through the filters, so Filter 1 controls the output signal from both Oscillator 1 and Oscillator 2 and Filter 2 controls the output signal from Filter 1. 

#### LFOs and Envelopes
In modern electronic music one of the primary ways to create interesting and unique sounds is through modulation. LFOs and envelopes are the two main ways of generating modulation signals. The two LFOs in this synth work very similarly to the oscillators, but generate much lower frequencies such that the individual waveforms can be heard distinctly. The LFOs offer three parameters; ‘Frequency’ allows the user to control the rate of oscillation of the LFO, ‘Amplitude’ allows the user to control the strength of the signal, and ‘Phase’ allows the user to offset the generation of the signal from the noise for interesting effects. Each LFO is capable of producing sine, saw, square, and triangle waves, which can be selected from the dropdown menu under the LFO selection menu.

Envelopes are grouped with LFOs due to their similarity in function. While an LFO is continually oscillating, and envelope only fires once. This makes them useful for sculpting the sound of a note by controlling the attack, decay, and sustain of the signal. Attack allows the user to set the length of time for the signal to reach its peak amplitude, decay sets the time to return to 0 amplitude, and sustain sets what that peak amplitude is. Envelopes are primarily used for controlling the shape of a note hit, but they can also be mapped to other parameters through the modulation matrix.

#### Modulation
Once a modulation signal has been generated from the LFOs and/or envelopes it becomes necessary to map that signal to the different parameters of the synthesizer. This is accomplished through the modulation matrix. Each parameter which can be modulated has a knob corresponding to a different modulation source. Turned fully on, the modulation source will totally modulate the parameter, while at lower settings it will have a more subtle effect. This is essential for creating interest and variance in electronic instruments and music.
