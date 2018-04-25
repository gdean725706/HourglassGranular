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

//==============================================================================
/**
*/
class HourglassGranularAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener
{
public:
    HourglassGranularAudioProcessorEditor (HourglassGranularAudioProcessor&);
    ~HourglassGranularAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void sliderValueChanged(Slider* slider) override;
private:
	GrainChannelComponent m_grainChannel1;
	ScopedPointer<Slider> m_masterTempoSlider;
	ScopedPointer<Slider> m_tempoDivision;


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HourglassGranularAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HourglassGranularAudioProcessorEditor)
};
