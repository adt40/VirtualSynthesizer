#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
VirtualsynthAudioProcessorEditor::VirtualsynthAudioProcessorEditor(VirtualsynthAudioProcessor& p)
	: AudioProcessorEditor(&p),
	processor(p),
	keyboardComponent(p.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
	isAddingFromMidiInput(false),
	upSemitone1("Up Semitone 1", 0.75, Colour::fromRGB(50, 50, 50)),
	downSemitone1("Down Semitone 1", 0.25, Colour::fromRGB(50, 50, 50)),
	upSemitone2("Up Semitone 2", 0.75, Colour::fromRGB(50, 50, 50)),
	downSemitone2("Down Semitone 2", 0.25, Colour::fromRGB(50, 50, 50)),
	upVoices1("Up Voices 1", 0.75, Colour::fromRGB(50, 50, 50)),
	downVoices1("Down Voices 1", 0.25, Colour::fromRGB(50, 50, 50)),
	upVoices2("Up Voices 2", 0.75, Colour::fromRGB(50, 50, 50)),
	downVoices2("Down Voices 2", 0.25, Colour::fromRGB(50, 50, 50))
{
    
    setSize (1100, 750);
    
    f1CutoffAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, FILT1CUTOFF_ID, filterFrequency1);
    f1ResAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, FILT1RES_ID, filterResonance1);
    
    f2CutoffAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, FILT2CUTOFF_ID, filterFrequency2);
    f2ResAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, FILT2RES_ID, filterResonance2);
    
    lfo1FreqAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, LFO1FREQ_ID, LFOFrequency1);
    lfo1AmpAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, LFO1AMP_ID, LFOAmplitude1);
    
    lfo2FreqAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, LFO2FREQ_ID, LFOFrequency2);
    lfo2AmpAttach = new AudioProcessorValueTreeState::SliderAttachment(processor.stateTree, LFO2AMP_ID, LFOAmplitude2);

	//Osc1 wavetable
	wavetablePosition1.setSliderStyle(Slider::RotaryVerticalDrag);
	wavetablePosition1.setRange(0.0, 0.99, 0.01);
	wavetablePosition1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	wavetablePosition1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	wavetablePosition1.setValue(0.0);
	wavetablePosition1.addListener(this);
	this->addAndMakeVisible(wavetablePosition1);
	
	//Osc2 wavetable
	wavetablePosition2.setSliderStyle(Slider::RotaryVerticalDrag);
	wavetablePosition2.setRange(0.0, 0.99, 0.01);
	wavetablePosition2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	wavetablePosition2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	wavetablePosition2.setValue(0.0);
	wavetablePosition2.addListener(this);
	this->addAndMakeVisible(wavetablePosition2);

	//Osc1 volume
	volume1.setSliderStyle(Slider::RotaryVerticalDrag);
	volume1.setRange(0.0, 1.0, 0.01);
	volume1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	volume1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	volume1.setValue(1.0);
	volume1.addListener(this);
	this->addAndMakeVisible(volume1);

	//Osc2 volume
	volume2.setSliderStyle(Slider::RotaryVerticalDrag);
	volume2.setRange(0.0, 1.0, 0.01);
	volume2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	volume2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	volume2.setValue(0.0);
	volume2.addListener(this);
	this->addAndMakeVisible(volume2);

	//Osc1 spread
	spread1.setSliderStyle(Slider::RotaryVerticalDrag);
	spread1.setRange(0.0, 1.0, 0.01);
	spread1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	spread1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	spread1.setValue(0.0);
	spread1.addListener(this);
	this->addAndMakeVisible(spread1);

	//Osc2 spread
	spread2.setSliderStyle(Slider::RotaryVerticalDrag);
	spread2.setRange(0.0, 1.0, 0.01);
	spread2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	spread2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	spread2.setValue(0.0);
	spread2.addListener(this);
	this->addAndMakeVisible(spread2);

	//Osc1 semitones
	upSemitone1.addListener(this);
	this->addAndMakeVisible(upSemitone1);

	downSemitone1.addListener(this);
	this->addAndMakeVisible(downSemitone1);

	//Osc2 semitones
	upSemitone2.addListener(this);
	this->addAndMakeVisible(upSemitone2);

	downSemitone2.addListener(this);
	this->addAndMakeVisible(downSemitone2);

	//Osc1 voices
	upVoices1.addListener(this);
	this->addAndMakeVisible(upVoices1);

	downVoices1.addListener(this);
	this->addAndMakeVisible(downVoices1);

	//Osc2 voices
	upVoices2.addListener(this);
	this->addAndMakeVisible(upVoices2);

	downVoices2.addListener(this);
	this->addAndMakeVisible(downVoices2);
    
    //Filter1 controls
    filterFrequency1.setSliderStyle(Slider::RotaryVerticalDrag);
    filterFrequency1.setRange(20.0, 20000, 1.0);
    filterFrequency1.setSkewFactorFromMidPoint(500);
    filterFrequency1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    filterFrequency1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    filterFrequency1.setValue(20000);
    filterFrequency1.addListener(this);
    this->addAndMakeVisible(filterFrequency1);
    
    filterResonance1.setSliderStyle(Slider::RotaryVerticalDrag);
    filterResonance1.setRange(0.01, 1.0, 0.01);
    filterResonance1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    filterResonance1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    filterResonance1.setValue(0.01);
    filterResonance1.addListener(this);
    this->addAndMakeVisible(filterResonance1);
    
    //Filter2 controls
    filterFrequency2.setSliderStyle(Slider::RotaryVerticalDrag);
    filterFrequency2.setRange(20.0, 20000, 1.0);
    filterFrequency2.setSkewFactorFromMidPoint(500);
    filterFrequency2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    filterFrequency2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    filterFrequency2.setValue(20000);
    filterFrequency2.addListener(this);
    this->addAndMakeVisible(filterFrequency2);
    
    filterResonance2.setSliderStyle(Slider::RotaryVerticalDrag);
    filterResonance2.setRange(0.01, 1.0, 0.01);
    filterResonance2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    filterResonance2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    filterResonance2.setValue(0.01);
    filterResonance2.addListener(this);
    this->addAndMakeVisible(filterResonance2);
    
    filterSeries.setButtonText("Series");
    filterSeries.setToggleState(true, dontSendNotification);
    filterSeries.setClickingTogglesState(true);
    filterSeries.addListener(this);
    this->addAndMakeVisible(filterSeries);
    
    //LFO
    
	LFOFrequency1.setSliderStyle(Slider::RotaryVerticalDrag);
	LFOFrequency1.setRange(0.05, 5, 0.01);
	LFOFrequency1.setSkewFactorFromMidPoint(1);
	LFOFrequency1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	LFOFrequency1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	LFOFrequency1.setValue(1.0);
	LFOFrequency1.addListener(this);
	this->addAndMakeVisible(LFOFrequency1);

	LFOAmplitude1.setSliderStyle(Slider::RotaryVerticalDrag);
	LFOAmplitude1.setRange(0.0, 1.0, 0.01);
	LFOAmplitude1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	LFOAmplitude1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	LFOAmplitude1.setValue(1.0);
	LFOAmplitude1.addListener(this);
	this->addAndMakeVisible(LFOAmplitude1);

	LFOPhase1.setSliderStyle(Slider::RotaryVerticalDrag);
	LFOPhase1.setRange(0.0, 0.99, 0.01);
	LFOPhase1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	LFOPhase1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	LFOPhase1.setValue(0.0);
	LFOPhase1.addListener(this);
	this->addAndMakeVisible(LFOPhase1);
    
	LFOShapeButton1.setButtonText("sin");
	LFOShapeButton1.addListener(this);
	this->addAndMakeVisible(LFOShapeButton1);
	LFOShapeMenu1.addItem(1, "sin");
	LFOShapeMenu1.addItem(2, "saw");
	LFOShapeMenu1.addItem(3, "square");
	LFOShapeMenu1.addItem(4, "triangle");

    //LFO2
    
    LFOFrequency2.setSliderStyle(Slider::RotaryVerticalDrag);
    LFOFrequency2.setRange(0.05, 5, 0.01);
    LFOFrequency2.setSkewFactorFromMidPoint(1);
    LFOFrequency2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    LFOFrequency2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    LFOFrequency2.setValue(1.0);
    LFOFrequency2.addListener(this);
    this->addChildComponent(LFOFrequency2);
    
    LFOAmplitude2.setSliderStyle(Slider::RotaryVerticalDrag);
    LFOAmplitude2.setRange(0.0, 1.0, 0.01);
    LFOAmplitude2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    LFOAmplitude2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    LFOAmplitude2.setValue(1.0);
    LFOAmplitude2.addListener(this);
    this->addChildComponent(LFOAmplitude2);
    
    LFOPhase2.setSliderStyle(Slider::RotaryVerticalDrag);
    LFOPhase2.setRange(0.0, 0.99, 0.01);
    LFOPhase2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
    LFOPhase2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    LFOPhase2.setValue(0.0);
    LFOPhase2.addListener(this);
    this->addChildComponent(LFOPhase2);

	LFOShapeButton2.setButtonText("sin");
	LFOShapeButton2.addListener(this);
	this->addChildComponent(LFOShapeButton2);
	LFOShapeMenu2.addItem(1, "sin");
	LFOShapeMenu2.addItem(2, "saw");
	LFOShapeMenu2.addItem(3, "square");
	LFOShapeMenu2.addItem(4, "triangle");
    
    LFOSelector.setButtonText("LFO1");
    LFOSelector.addListener(this);
    this->addAndMakeVisible(LFOSelector);
    
    LFOSelectionMenu.addItem(1, "LFO1");
    LFOSelectionMenu.addItem(2, "LFO2");

	//env

	envAttack1.setSliderStyle(Slider::RotaryVerticalDrag);
	envAttack1.setRange(0.005, 4, 0.01);
	envAttack1.setSkewFactorFromMidPoint(1);
	envAttack1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	envAttack1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	envAttack1.setValue(0.01);
	envAttack1.addListener(this);
	this->addAndMakeVisible(envAttack1);

	envDecay1.setSliderStyle(Slider::RotaryVerticalDrag);
	envDecay1.setRange(0.005, 4, 0.01);
	envDecay1.setSkewFactorFromMidPoint(1);
	envDecay1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	envDecay1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	envDecay1.setValue(0.01);
	envDecay1.addListener(this);
	this->addAndMakeVisible(envDecay1);

	envSustain1.setSliderStyle(Slider::RotaryVerticalDrag);
	envSustain1.setRange(0.001, 1, 0.001);
	envSustain1.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	envSustain1.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	envSustain1.setValue(1);
	envSustain1.addListener(this);
	this->addAndMakeVisible(envSustain1);


	envAttack2.setSliderStyle(Slider::RotaryVerticalDrag);
	envAttack2.setRange(0.005, 4, 0.01);
	envAttack2.setSkewFactorFromMidPoint(1);
	envAttack2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	envAttack2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	envAttack2.setValue(0.01);
	envAttack2.addListener(this);
	this->addAndMakeVisible(envAttack2);

	envDecay2.setSliderStyle(Slider::RotaryVerticalDrag);
	envDecay2.setRange(0.005, 4, 0.01);
	envDecay2.setSkewFactorFromMidPoint(1);
	envDecay2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	envDecay2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	envDecay2.setValue(0.01);
	envDecay2.addListener(this);
	this->addAndMakeVisible(envDecay2);

	envSustain2.setSliderStyle(Slider::RotaryVerticalDrag);
	envSustain2.setRange(0.001, 1, 0.001);
	envSustain2.setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
	envSustain2.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	envSustain2.setValue(1.0);
	envSustain2.addListener(this);
	this->addAndMakeVisible(envSustain2);

	envSelector.setButtonText("ENV1");
	envSelector.addListener(this);
	this->addAndMakeVisible(envSelector);

	envSelectionMenu.addItem(1, "ENV1");
	envSelectionMenu.addItem(2, "ENV2");


    
    filter1TypeButton.setButtonText("lo-pass");
    filter1TypeButton.addListener(this);
    this->addAndMakeVisible(filter1TypeButton);
    
    filter2TypeButton.setButtonText("lo-pass");
    filter2TypeButton.addListener(this);
    this->addAndMakeVisible(filter2TypeButton);
    
    filterTypeMenu.addItem(1, "lo-pass");
    filterTypeMenu.addItem(2, "band-pass");
    filterTypeMenu.addItem(3, "hi-pass");
	
	for (int i = 0; i < 40; i++) {
		modMatrixKnobs[i].setSliderStyle(Slider::RotaryVerticalDrag);
		modMatrixKnobs[i].setRange(0.0, 1.0, 0.0001);
		if (i % 10 == 6 || i % 10 == 8) {
			modMatrixKnobs[i].setSkewFactorFromMidPoint(500.0/(20000.0 - 20.0));
		}
		modMatrixKnobs[i].setRotaryParameters((5 * double_Pi) / 4, (11 * double_Pi) / 4, true);
		modMatrixKnobs[i].setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
		modMatrixKnobs[i].setValue(0.0);
		modMatrixKnobs[i].addListener(this);
		this->addAndMakeVisible(modMatrixKnobs[i]);
	}
	
	//wavetable selection menu
	WavetableSelector1.setButtonText("Basic Shapes");
	WavetableSelector1.addListener(this);
	this->addAndMakeVisible(WavetableSelector1);

	WavetableMenu1.addItem(1, "Basic Shapes");
	WavetableMenu1.addItem(2, "Sync Saw");
	WavetableMenu1.addItem(3, "PWM Square");

	WavetableSelector2.setButtonText("Basic Shapes");
	WavetableSelector2.addListener(this);
	this->addAndMakeVisible(WavetableSelector2);

	WavetableMenu2.addItem(1, "Basic Shapes");
	WavetableMenu2.addItem(2, "Sync Saw");
	WavetableMenu2.addItem(3, "PWM Square");

	this->addAndMakeVisible(keyboardComponent);
	keyboardState.addListener(this);
}

VirtualsynthAudioProcessorEditor::~VirtualsynthAudioProcessorEditor()
{
}

//==============================================================================
void VirtualsynthAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(Colours::white);
	g.setColour(Colours::black);
	g.setFont(15.0f);
	g.drawFittedText("osc1:", oscillatorLeft, firstOscillatorTop + 30, 40, 20, Justification::centredLeft, 1);
	g.drawFittedText("osc2:", oscillatorLeft, firstOscillatorTop + oscillatorHeight + 30, 40, 20, Justification::centredLeft, 1);
	g.drawFittedText("Spread", oscillatorLeft + arrowButtonWidth + 40 + arrowButtonWidth * 2, firstOscillatorTop + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Spread", oscillatorLeft + arrowButtonWidth + 40 + arrowButtonWidth * 2, firstOscillatorTop + oscillatorHeight + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Wavetable", oscillatorLeft + knobWidth + arrowButtonWidth + 40 + arrowButtonWidth*2, firstOscillatorTop + knobHeight, knobWidth, oscillatorHeight - knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Wavetable", oscillatorLeft + knobWidth + arrowButtonWidth + 40 + arrowButtonWidth*2, firstOscillatorTop + oscillatorHeight + knobHeight, knobWidth, oscillatorHeight - knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Volume", oscillatorLeft + knobWidth + arrowButtonWidth + knobWidth + 40 + arrowButtonWidth*2, firstOscillatorTop + knobHeight, knobWidth, oscillatorHeight - knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Volume", oscillatorLeft + knobWidth + arrowButtonWidth + knobWidth + 40 + arrowButtonWidth*2, firstOscillatorTop + oscillatorHeight + knobHeight, knobWidth, oscillatorHeight - knobHeight, Justification::centredTop, 1);
    
	g.drawFittedText("Frequency", oscillatorLeft + textButtonWidth, firstOscillatorTop + oscillatorHeight * 2 + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Amplitude", oscillatorLeft + textButtonWidth + knobWidth, firstOscillatorTop + oscillatorHeight * 2 + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Phase", oscillatorLeft + textButtonWidth + knobWidth * 2, firstOscillatorTop + oscillatorHeight * 2 + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
    
	g.drawFittedText("Attack", oscillatorLeft + textButtonWidth, firstOscillatorTop + oscillatorHeight * 3 + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Decay", oscillatorLeft + textButtonWidth + knobWidth, firstOscillatorTop + oscillatorHeight * 3 + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
	g.drawFittedText("Sustain", oscillatorLeft + textButtonWidth + knobWidth * 2, firstOscillatorTop + oscillatorHeight * 3 + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);

    g.drawFittedText("Cutoff", getWidth() - oscillatorLeft - knobWidth*2 - 40, firstOscillatorTop + knobHeight -10, knobWidth, knobHeight, Justification::centredTop, 1);
    g.drawFittedText("Resonance", getWidth() - oscillatorLeft - knobWidth - 40, firstOscillatorTop + knobHeight -10, knobWidth, knobHeight, Justification::centredTop, 1);
    
    g.drawFittedText("Cutoff", getWidth() - oscillatorLeft - knobWidth*2 - 40, firstOscillatorTop + oscillatorHeight + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);
    g.drawFittedText("Resonance", getWidth() - oscillatorLeft - knobWidth - 40, firstOscillatorTop + oscillatorHeight + knobHeight, knobWidth, knobHeight, Justification::centredTop, 1);

	g.drawFittedText("LFO1", getWidth() - 12 * modKnobWidth, getHeight() - 100 - 4 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::right, 1);
	g.drawFittedText("LFO2", getWidth() - 12 * modKnobWidth, getHeight() - 100 - 3 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::right, 1);
	g.drawFittedText("ENV1", getWidth() - 12 * modKnobWidth, getHeight() - 100 - 2 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::right, 1);
	g.drawFittedText("ENV2", getWidth() - 12 * modKnobWidth, getHeight() - 100 - 1 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::right, 1);

	g.drawFittedText("OSC1\nspd", getWidth() - 11 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("OSC1\nwvt", getWidth() - 10 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("OSC1\nvol", getWidth() - 9 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("OSC2\nspd", getWidth() - 8 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("OSC2\nwvt", getWidth() - 7 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("OSC2\nvol", getWidth() - 6 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("FLT1\ncut", getWidth() - 5 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("FLT1\nres", getWidth() - 4 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("FLT1\ncut", getWidth() - 3 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);
	g.drawFittedText("FLT1\nres", getWidth() - 2 * modKnobWidth, getHeight() - 100 - 5 * modKnobHeight, modKnobWidth, modKnobHeight, Justification::centred, 2);

	g.drawFittedText("S", oscillatorLeft + 38, firstOscillatorTop + arrowButtonHeight * 2 + semitoneTextHeight + 3, arrowButtonWidth, arrowButtonHeight, Justification::centred, 1);
	g.drawFittedText("V", oscillatorLeft + arrowButtonWidth * 2 + 38, firstOscillatorTop + arrowButtonHeight * 2 + semitoneTextHeight + 3, arrowButtonWidth, arrowButtonHeight, Justification::centred, 1);
	g.drawFittedText((String)osc1CurrentSemitones, oscillatorLeft + 38, firstOscillatorTop + arrowButtonHeight, arrowButtonWidth, semitoneTextHeight, Justification::centred, 1);
	g.drawFittedText((String)osc1CurrentVoices, oscillatorLeft + arrowButtonWidth * 2 + 38, firstOscillatorTop + arrowButtonHeight, arrowButtonWidth, semitoneTextHeight, Justification::centred, 1);
	g.drawFittedText((String)osc2CurrentSemitones, oscillatorLeft + 38, firstOscillatorTop + oscillatorHeight + arrowButtonHeight, arrowButtonWidth, semitoneTextHeight, Justification::centred, 1);
	g.drawFittedText((String)osc2CurrentVoices, oscillatorLeft + arrowButtonWidth * 2 + 38, firstOscillatorTop + oscillatorHeight + arrowButtonHeight, arrowButtonWidth, semitoneTextHeight, Justification::centred, 1);
}

void VirtualsynthAudioProcessorEditor::resized()
{
    
	spread1.setBounds(oscillatorLeft + arrowButtonWidth + 40 + arrowButtonWidth * 2, firstOscillatorTop, knobWidth, knobHeight);
	spread2.setBounds(oscillatorLeft + arrowButtonWidth + 40 + arrowButtonWidth * 2, firstOscillatorTop + oscillatorHeight, knobWidth, knobHeight);
	wavetablePosition1.setBounds(oscillatorLeft + knobWidth + arrowButtonWidth + 40 + arrowButtonWidth*2, firstOscillatorTop, knobWidth, knobHeight);
	wavetablePosition2.setBounds(oscillatorLeft + knobWidth + arrowButtonWidth + 40 + arrowButtonWidth*2, firstOscillatorTop + oscillatorHeight, knobWidth, knobHeight);
	volume1.setBounds(oscillatorLeft + knobWidth + arrowButtonWidth + 40 + knobWidth + arrowButtonWidth*2, firstOscillatorTop, knobWidth, knobHeight);
	volume2.setBounds(oscillatorLeft + knobWidth + arrowButtonWidth + 40 + knobWidth + arrowButtonWidth*2, firstOscillatorTop + oscillatorHeight, knobWidth, knobHeight);

	upSemitone1.setBounds(oscillatorLeft + 40, firstOscillatorTop, arrowButtonWidth, arrowButtonHeight);
	downSemitone1.setBounds(oscillatorLeft + 40, firstOscillatorTop + arrowButtonHeight + semitoneTextHeight, arrowButtonWidth, arrowButtonHeight);
	upSemitone2.setBounds(oscillatorLeft + 40, firstOscillatorTop + oscillatorHeight, arrowButtonWidth, arrowButtonHeight);
	downSemitone2.setBounds(oscillatorLeft + 40, firstOscillatorTop + oscillatorHeight + arrowButtonHeight + semitoneTextHeight, arrowButtonWidth, arrowButtonHeight);

	upVoices1.setBounds(oscillatorLeft + arrowButtonWidth*2 + 40, firstOscillatorTop, arrowButtonWidth, arrowButtonHeight);
	downVoices1.setBounds(oscillatorLeft + arrowButtonWidth*2 + 40, firstOscillatorTop + arrowButtonHeight + semitoneTextHeight, arrowButtonWidth, arrowButtonHeight);
	upVoices2.setBounds(oscillatorLeft + arrowButtonWidth*2 + 40, firstOscillatorTop + oscillatorHeight, arrowButtonWidth, arrowButtonHeight);
	downVoices2.setBounds(oscillatorLeft + arrowButtonWidth*2 + 40, firstOscillatorTop + oscillatorHeight + arrowButtonHeight + semitoneTextHeight, arrowButtonWidth, arrowButtonHeight);
   
    if(LFOSelected == 1) {
		LFOShapeButton1.setBounds(oscillatorLeft, firstOscillatorTop + oscillatorHeight * 2 + textButtonHeight, textButtonWidth, textButtonHeight);
		LFOFrequency1.setBounds(oscillatorLeft + textButtonWidth, firstOscillatorTop + oscillatorHeight * 2, knobWidth, knobHeight);
        LFOAmplitude1.setBounds(oscillatorLeft + textButtonWidth + knobWidth, firstOscillatorTop + oscillatorHeight * 2, knobWidth, knobHeight);
        LFOPhase1.setBounds(oscillatorLeft + textButtonWidth + knobWidth * 2, firstOscillatorTop + oscillatorHeight * 2, knobWidth, knobHeight);
    } else {
		LFOShapeButton2.setBounds(oscillatorLeft, firstOscillatorTop + oscillatorHeight * 2 + textButtonHeight, textButtonWidth, textButtonHeight);
        LFOFrequency2.setBounds(oscillatorLeft + textButtonWidth, firstOscillatorTop + oscillatorHeight * 2, knobWidth, knobHeight);
        LFOAmplitude2.setBounds(oscillatorLeft + textButtonWidth + knobWidth, firstOscillatorTop + oscillatorHeight * 2, knobWidth, knobHeight);
        LFOPhase2.setBounds(oscillatorLeft + textButtonWidth + knobWidth * 2, firstOscillatorTop + oscillatorHeight * 2, knobWidth, knobHeight);
    }
    
    LFOSelector.setBounds(oscillatorLeft, firstOscillatorTop + oscillatorHeight * 2, textButtonWidth, textButtonHeight);

	if (envSelected == 1) {
		envAttack1.setBounds(oscillatorLeft + textButtonWidth, firstOscillatorTop + oscillatorHeight * 3, knobWidth, knobHeight);
		envDecay1.setBounds(oscillatorLeft + textButtonWidth + knobWidth, firstOscillatorTop + oscillatorHeight * 3, knobWidth, knobHeight);
		envSustain1.setBounds(oscillatorLeft + textButtonWidth + knobWidth * 2, firstOscillatorTop + oscillatorHeight * 3, knobWidth, knobHeight);
	}
	else {
		envAttack2.setBounds(oscillatorLeft + textButtonWidth, firstOscillatorTop + oscillatorHeight * 3, knobWidth, knobHeight);
		envDecay2.setBounds(oscillatorLeft + textButtonWidth + knobWidth, firstOscillatorTop + oscillatorHeight * 3, knobWidth, knobHeight);
		envSustain2.setBounds(oscillatorLeft + textButtonWidth + knobWidth * 2, firstOscillatorTop + oscillatorHeight * 3, knobWidth, knobHeight);
	}

	envSelector.setBounds(oscillatorLeft, firstOscillatorTop + oscillatorHeight * 3, textButtonWidth, textButtonHeight);

	keyboardComponent.setBounds(10, getHeight() - 60, getWidth() - 20, 60);
    
    filterFrequency1.setBounds(getWidth() - oscillatorLeft - knobWidth*2 - 40, firstOscillatorTop, knobWidth, knobHeight);
    filterResonance1.setBounds(getWidth() - oscillatorLeft - knobWidth - 40, firstOscillatorTop, knobWidth, knobHeight);
    filter1TypeButton.setBounds(getWidth() - oscillatorLeft - knobWidth*3 - 20, firstOscillatorTop + knobHeight * 0.4, textButtonWidth, textButtonHeight);
    
    filterFrequency2.setBounds(getWidth() - oscillatorLeft - knobWidth*2 - 40, firstOscillatorTop + oscillatorHeight + 10, knobWidth, knobHeight);
    filterResonance2.setBounds(getWidth() - oscillatorLeft - knobWidth - 40, firstOscillatorTop + oscillatorHeight + 10, knobWidth, knobHeight);
    filter2TypeButton.setBounds(getWidth() - oscillatorLeft - knobWidth*3 - 20, firstOscillatorTop + oscillatorHeight + knobHeight * 0.4, textButtonWidth, textButtonHeight);
    
    filterSeries.setBounds(getWidth() - oscillatorLeft - knobWidth * 1.8, firstOscillatorTop + oscillatorHeight - 30, textButtonWidth, textButtonHeight);

	int xPos = 11;
	int yPos = 4;
	for (int i = 0; i < 40; i++) {
		modMatrixKnobs[i].setBounds(getWidth() - xPos * modKnobWidth, getHeight() - 100 - yPos * modKnobHeight, modKnobWidth, modKnobHeight);
		xPos--;
		if (xPos <= 1) {
			xPos = 11;
			yPos--;
		}
	}

	WavetableSelector1.setBounds(oscillatorLeft + knobWidth * 3 + arrowButtonWidth + 50 + arrowButtonWidth * 2, firstOscillatorTop + knobHeight * 0.4, knobWidth, textButtonHeight);
	WavetableSelector2.setBounds(oscillatorLeft + knobWidth * 3 + arrowButtonWidth + 50 + arrowButtonWidth * 2, firstOscillatorTop + oscillatorHeight + knobHeight * 0.4, knobWidth, textButtonHeight);

}

void VirtualsynthAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider == &wavetablePosition1) {
		processor.updateWavetables(slider->getValue(), 1);
	}
	else if (slider == &wavetablePosition2) {
		processor.updateWavetables(slider->getValue(), 2);
	}
	else if (slider == &volume1) {
		processor.updateVolumes(slider->getValue(), 1);
	}
	else if (slider == &volume2) {
		processor.updateVolumes(slider->getValue(), 2);
	}
	else if (slider == &spread1) {
		processor.updateSpreads(slider->getValue(), 1);
	}
	else if (slider == &spread2) {
		processor.updateSpreads(slider->getValue(), 2);
	}
    else if(slider == &filterFrequency1) {
        processor.updateCutoffs(slider -> getValue(), 1);
    }
    else if(slider == &filterFrequency2) {
        processor.updateCutoffs(slider -> getValue(), 2);
    }
    else if(slider == &filterResonance1) {
        processor.updateResonances(slider->getValue(), 1);
    }
    else if(slider == &filterResonance2) {
        processor.updateResonances(slider->getValue(), 2);
    }
	else if (slider == &LFOFrequency1) {
		processor.updateLFOFrequency(slider->getValue(), 1);
	}
	else if (slider == &LFOAmplitude1) {
		processor.updateLFOAmplitude(slider->getValue(), 1);
	}
	else if (slider == &LFOPhase1) {
		processor.updateLFOPhase(slider->getValue(), 1);
	}
    else if (slider == &LFOFrequency2) {
        processor.updateLFOFrequency(slider->getValue(), 2);
    }
    else if (slider == &LFOAmplitude2) {
        processor.updateLFOAmplitude(slider->getValue(), 2);
    }
    else if (slider == &LFOPhase2) {
        processor.updateLFOPhase(slider->getValue(), 2);
    }
	else if (slider == &envAttack1) {
		processor.updateEnvAttack(slider->getValue(), 1);
	}
	else if (slider == &envDecay1) {
		processor.updateEnvDecay(slider->getValue(), 1);
	}
	else if (slider == &envSustain1) {
		processor.updateEnvSustain(slider->getValue(), 1);
	}
	else if (slider == &envAttack2) {
		processor.updateEnvAttack(slider->getValue(), 2);
	}
	else if (slider == &envDecay2) {
		processor.updateEnvDecay(slider->getValue(), 2);
	}
	else if (slider == &envSustain2) {
		processor.updateEnvSustain(slider->getValue(), 2);
	}
	else {
		for (int i = 0; i < 44; i++) {
			if (slider == &modMatrixKnobs[i]) {
				processor.updateModulation(i, slider->getValue());
				break;
			}
		}
	}


}

void VirtualsynthAudioProcessorEditor::buttonClicked(Button* button) {
	if (button == &upSemitone1) {
		processor.updateSemitones(1, 1);
		osc1CurrentSemitones++;
		this->repaint();
	}
	else if (button == &downSemitone1) {
		processor.updateSemitones(-1, 1);
		osc1CurrentSemitones--;
		this->repaint();
	}
	else if (button == &upSemitone2) {
		processor.updateSemitones(1, 2);
		osc2CurrentSemitones++;
		this->repaint();
	}
	else if (button == &downSemitone2) {
		processor.updateSemitones(-1, 2);
		osc2CurrentSemitones--;
		this->repaint();
	}
	else if (button == &upVoices1) {
		if (osc1CurrentVoices < 7) {
			processor.updateVoices(1, 1);
			osc1CurrentVoices++;
			this->repaint();
		}
	}
	else if (button == &downVoices1) {
		if (osc1CurrentVoices > 1) {
			processor.updateVoices(-1, 1);
			osc1CurrentVoices--;
			this->repaint();
		}
	}
	else if (button == &upVoices2) {
		if (osc2CurrentVoices < 7) {
			processor.updateVoices(1, 2);
			osc2CurrentVoices++;
			this->repaint();
		}
	}
	else if (button == &downVoices2) {
		if (osc2CurrentVoices > 1) {
			processor.updateVoices(-1, 2);
			osc2CurrentVoices--;
			this->repaint();
		}
	}
	else if (button == &LFOShapeButton1) {
		int result = LFOShapeMenu1.showAt(button);
		switch (result) {
		case 1:
			LFOShapeButton1.setButtonText("sin");
			processor.updateLFOShape(result, 1);
			break;
		case 2:
			LFOShapeButton1.setButtonText("saw");
			processor.updateLFOShape(result, 1);
			break;
		case 3:
			LFOShapeButton1.setButtonText("square");
			processor.updateLFOShape(result, 1);
			break;
		case 4:
			LFOShapeButton1.setButtonText("triangle");
			processor.updateLFOShape(result, 1);
			break;
		default:
			break;
		}
	}
	else if (button == &LFOShapeButton2) {
		int result = LFOShapeMenu2.showAt(button);
		switch (result) {
		case 1:
			LFOShapeButton2.setButtonText("sin");
			processor.updateLFOShape(result, 2);
			break;
		case 2:
			LFOShapeButton2.setButtonText("saw");
			processor.updateLFOShape(result, 2);
			break;
		case 3:
			LFOShapeButton2.setButtonText("square");
			processor.updateLFOShape(result, 2);
			break;
		case 4:
			LFOShapeButton2.setButtonText("triangle");
			processor.updateLFOShape(result, 2);
			break;
		default:
			break;
		}
	}
    else if (button == &LFOSelector) {
        int result = LFOSelectionMenu.showAt(button);
        switch(result) {
        case 1:
            LFOSelector.setButtonText("LFO1");
            LFOAmplitude1.setVisible(true);
            LFOFrequency1.setVisible(true);
            LFOPhase1.setVisible(true);
			LFOShapeButton1.setVisible(true);
            LFOFrequency2.setVisible(false);
            LFOAmplitude2.setVisible(false);
            LFOPhase2.setVisible(false);
			LFOShapeButton2.setVisible(false);
            LFOSelected = result;
            resized();
            break;
        case 2:
            LFOSelector.setButtonText("LFO2");
            LFOAmplitude1.setVisible(false);
            LFOFrequency1.setVisible(false);
            LFOPhase1.setVisible(false);
			LFOShapeButton1.setVisible(false);
            LFOFrequency2.setVisible(true);
            LFOAmplitude2.setVisible(true);
            LFOPhase2.setVisible(true);
			LFOShapeButton2.setVisible(true);
            LFOSelected = result;
            resized();
            break;
        }
    }
	else if (button == &envSelector) {
		int result = envSelectionMenu.showAt(button);
		switch (result) {
		case 1:
			envSelector.setButtonText("ENV1");
			envAttack1.setVisible(true);
			envDecay1.setVisible(true);
			envSustain1.setVisible(true);
			envAttack2.setVisible(false);
			envDecay2.setVisible(false);
			envSustain2.setVisible(false);
			envSelected = result;
			resized();
			break;
		case 2:
			envSelector.setButtonText("ENV2");
			envAttack1.setVisible(false);
			envDecay1.setVisible(false);
			envSustain1.setVisible(false);
			envAttack2.setVisible(true);
			envDecay2.setVisible(true);
			envSustain2.setVisible(true);
			envSelected = result;
			resized();
			break;
		}
	}
    else if (button == &filter1TypeButton) {
        int result = filterTypeMenu.showAt(button);
        switch (result) {
            case 1:
                filter1TypeButton.setButtonText("lo-pass");
                processor.updateFilterTypes(result, 1);
                break;
            case 2:
                filter1TypeButton.setButtonText("band-pass");
                processor.updateFilterTypes(result, 1);
                break;
            case 3:
                filter1TypeButton.setButtonText("hi-pass");
                processor.updateFilterTypes(result, 1);
                break;
            default:
                break;
        }
    }
    else if (button == &filter2TypeButton) {
        int result = filterTypeMenu.showAt(button);
        switch (result) {
            case 1:
                filter2TypeButton.setButtonText("lo-pass");
                processor.updateFilterTypes(result, 2);
                break;
            case 2:
                filter2TypeButton.setButtonText("band-pass");
                processor.updateFilterTypes(result, 2);
                break;
            case 3:
                filter2TypeButton.setButtonText("hi-pass");
                processor.updateFilterTypes(result, 2);
                break;
            default:
                break;
        }
    }
    else if (button == &filterSeries) {
        processor.updateIsSeries(filterSeries.getToggleState());
        if(filterSeries.getToggleState()) {
            filterSeries.setButtonText("Series");
            filterSeries.changeWidthToFitText();
        }
        else {
            filterSeries.setButtonText("Parallel");
            filterSeries.changeWidthToFitText();
        }
    }
	else if (button == &WavetableSelector1) {
		int result = WavetableMenu1.showAt(button);
		switch (result) {
		case 1:
			WavetableSelector1.setButtonText("Basic Shapes");
			processor.updateWavetableType("Basic Shapes", 1);
			break;
		case 2:
			WavetableSelector1.setButtonText("Sync Saw");
			processor.updateWavetableType("Sync Saw", 1);
			break;
		case 3:
			WavetableSelector1.setButtonText("PWM Square");
			processor.updateWavetableType("PWM Square", 1);
			break;
		default:
			break;
		}
	}
	else if (button == &WavetableSelector2) {
		int result = WavetableMenu2.showAt(button);
		switch (result) {
		case 1:
			WavetableSelector2.setButtonText("Basic Shapes");
			processor.updateWavetableType("Basic Shapes", 2);
			break;
		case 2:
			WavetableSelector2.setButtonText("Sync Saw");
			processor.updateWavetableType("Sync Saw", 2);
			break;
		case 3:
			WavetableSelector2.setButtonText("PWM Square");
			processor.updateWavetableType("PWM Square", 2);
			break;
		default:
			break;
		}
	}
}

void VirtualsynthAudioProcessorEditor::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message)
{
	const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
	keyboardState.processNextMidiEvent(message);
}

void VirtualsynthAudioProcessorEditor::handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
	if (!isAddingFromMidiInput)
	{
		MidiMessage m(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
		m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        
	}
    
}
void VirtualsynthAudioProcessorEditor::handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
	if (!isAddingFromMidiInput)
	{
		MidiMessage m(MidiMessage::noteOff(midiChannel, midiNoteNumber));
		m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
	}
}
