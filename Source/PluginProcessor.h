/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "JuicyClouds.h"
#include "EnvelopeGenerator.h"
#include "BiQuad.h"
#include "PitchShifter.h"

//==============================================================================
/**
*/
class HourglassGranularAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    HourglassGranularAudioProcessor();
    ~HourglassGranularAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
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

	JuicyClouds* getGranularProcessor();

	void setAllPassFreq(float freq); 
	void setAllPassQ(float q);


private:
	AudioProcessorValueTreeState m_parameters;
	JuicyClouds m_clouds;

	EnvelopeGenerator m_ampEnv;

	BiQuad m_allpassLeft, m_allpassRight;
	BiQuad m_allpassLeft2, m_allpassRight2;
	float m_apFreq, m_apQ;

	PitchShifter m_pitchShifter;

	//File* m_audioFile;

	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HourglassGranularAudioProcessor)
};
