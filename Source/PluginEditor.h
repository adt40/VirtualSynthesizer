#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class VirtualsynthAudioProcessorEditor  : public AudioProcessorEditor,
	private Slider::Listener,
	private Button::Listener,
	private MidiKeyboardStateListener,
	private MidiInputCallback
{
public:
    VirtualsynthAudioProcessorEditor (VirtualsynthAudioProcessor&);
    ~VirtualsynthAudioProcessorEditor();
    Slider wavetablePosition1;
	Slider wavetablePosition2;
	Slider volume1;
	Slider volume2;
	Slider spread1;
	Slider spread2;
	ArrowButton upSemitone1;
	ArrowButton downSemitone1;
	ArrowButton upSemitone2;
	ArrowButton downSemitone2;
	ArrowButton upVoices1;
	ArrowButton downVoices1;
	ArrowButton upVoices2;
	ArrowButton downVoices2;
    Slider filterFrequency1;
    Slider filterResonance1;
    Slider filterFrequency2;
    Slider filterResonance2;
	Slider LFOFrequency1;
	Slider LFOAmplitude1;
	Slider LFOPhase1;
    Slider LFOFrequency2;
    Slider LFOAmplitude2;
    Slider LFOPhase2;
    TextButton LFOSelector;
    PopupMenu LFOSelectionMenu;
	TextButton LFOShapeButton1;
	TextButton LFOShapeButton2;
	PopupMenu LFOShapeMenu1;
	PopupMenu LFOShapeMenu2;
	Slider envAttack1;
	Slider envDecay1;
	Slider envSustain1;
	Slider envAttack2;
	Slider envDecay2;
	Slider envSustain2;
	TextButton envSelector;
	PopupMenu envSelectionMenu;
    PopupMenu filterTypeMenu;
    TextButton filter1TypeButton;
    TextButton filter2TypeButton;
    TextButton filterSeries;
	TextButton WavetableSelector1;
	PopupMenu WavetableMenu1;
	TextButton WavetableSelector2;
	PopupMenu WavetableMenu2;

	Slider modMatrixKnobs[44];
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> f1CutoffAttach;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> f1ResAttach;
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> f2CutoffAttach;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> f2ResAttach;
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> lfo1FreqAttach;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> lfo1AmpAttach;
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> lfo2FreqAttach;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> lfo2AmpAttach;


    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

	void sliderValueChanged (Slider* slider) override;
	void buttonClicked(Button* button) override;

	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override;
	void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
	void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

	bool isAddingFromMidiInput;
	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
    VirtualsynthAudioProcessor& processor;
    
    int LFOSelected = 1;
	int envSelected = 1;
	int osc1CurrentVoices = 1;
	int osc1CurrentSemitones = 0;
	int osc2CurrentVoices = 1;
	int osc2CurrentSemitones = 0;

	const int oscillatorLeft = 10;
	const int firstOscillatorTop = 40;
	const int oscillatorHeight = 160;
	const int oscillatorWidth = 450;
	const int knobWidth = 100;
	const int knobHeight = 100;
	const int arrowButtonWidth = 20;
	const int arrowButtonHeight = 40;
	const int semitoneTextHeight = 20;
	const int textButtonHeight = 25;
	const int textButtonWidth = 70;
	const int modKnobWidth = 50;
	const int modKnobHeight = 50;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualsynthAudioProcessorEditor)
};
