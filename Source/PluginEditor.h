/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GrainChannelComponent.h"
#include "EffectControlComponent.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/**
*/
class HourglassGranularAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener, public Button::Listener
{
public:
    HourglassGranularAudioProcessorEditor (HourglassGranularAudioProcessor& p, AudioProcessorValueTreeState& vts);
    ~HourglassGranularAudioProcessorEditor();

    //==============================================================================

	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

	void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider* slider) override; 
	void buttonClicked(Button* button) override;
private:
	GrainChannelComponent m_grainChannel1;
	ScopedPointer<Slider> m_masterTempoSlider;
	ScopedPointer<Slider> m_tempoDivision;
	EffectControlComponent m_effectComponent;

	std::unique_ptr<SliderAttachment> m_masterTempoAttachment;
	std::unique_ptr<SliderAttachment> m_tempoDivisionAttachment;

	ScopedPointer<ToggleButton> m_midiTrigger;
	std::unique_ptr<ButtonAttachment> m_midiTrigAttach;

	AudioProcessorValueTreeState& m_valueTreeState;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HourglassGranularAudioProcessor& processor;

	CustomLookAndFeel m_customLook;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HourglassGranularAudioProcessorEditor)
};
