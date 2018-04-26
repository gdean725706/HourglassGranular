/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
HourglassGranularAudioProcessor::HourglassGranularAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
#endif
	m_parameters(*this, nullptr),
	m_clouds(m_parameters),
	m_ampEnv(44100.0f, 0.0f, 1.0f, 1.0f, 1.0f),
	m_allpassLeft(44100.0f),
	m_allpassRight(44100.0f),
	m_allpassLeft2(44100.0f),
	m_allpassRight2(44100.0f),
	m_apFreq(1000.0f),
	m_apQ(1.0f),
	m_pitchShifter(m_parameters)
{
	m_ampEnv.setAttack(0.01f);
	m_ampEnv.setDecay(0.1f);
	m_ampEnv.setRelease(0.1f);
	m_ampEnv.setSustain(1.0f);

	m_allpassLeft.createAllPass(1000.0f, 1.0f);
	m_allpassRight.createAllPass(1000.0f, 1.0f);
	m_allpassLeft2.createAllPass(2000.0f, 1.0f);
	m_allpassRight2.createAllPass(2000.0f, 1.0f);


	m_parameters.state = ValueTree(Identifier("HourglassGranular"));

}

HourglassGranularAudioProcessor::~HourglassGranularAudioProcessor()
{
}

//==============================================================================
const String HourglassGranularAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HourglassGranularAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HourglassGranularAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HourglassGranularAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HourglassGranularAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HourglassGranularAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HourglassGranularAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HourglassGranularAudioProcessor::setCurrentProgram (int index)
{
}

const String HourglassGranularAudioProcessor::getProgramName (int index)
{
    return {};
}

void HourglassGranularAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void HourglassGranularAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	m_clouds.setSampleRate(sampleRate);
	m_ampEnv.setSampleRate(sampleRate);
	m_allpassLeft.setSampleRate(sampleRate);
	m_allpassRight.setSampleRate(sampleRate);
	m_allpassLeft2.setSampleRate(sampleRate);
	m_allpassRight2.setSampleRate(sampleRate);

	m_pitchShifter.prepareToPlay(sampleRate, samplesPerBlock);
}

void HourglassGranularAudioProcessor::releaseResources()
{
	m_pitchShifter.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HourglassGranularAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HourglassGranularAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	MidiMessage msg;
	int time = 0;
	for (MidiBuffer::Iterator it(midiMessages); it.getNextEvent(msg, time); ) 
	{
		if (msg.isNoteOn())
		{
			m_ampEnv.trigger();

			int midiNote = msg.getNoteNumber();
			m_clouds.setGrainPitch(midiNote);

		}
		else if (msg.isNoteOff()) 
		{
			m_ampEnv.release();
		}
	}

	m_clouds.process(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());

	m_allpassLeft.process(buffer.getWritePointer(0), buffer.getNumSamples());
	m_allpassRight.process(buffer.getWritePointer(1), buffer.getNumSamples());
	m_allpassLeft2.process(buffer.getWritePointer(0), buffer.getNumSamples());
	m_allpassRight2.process(buffer.getWritePointer(1), buffer.getNumSamples());
	
	m_pitchShifter.processBlock(buffer, midiMessages);

	return;

	for (int n = 0; n < buffer.getNumSamples(); ++n)
	{
		float* left = buffer.getWritePointer(0);
		float *right = buffer.getWritePointer(1);
		
		left[n] *= m_ampEnv.getValue();
		right[n] *= m_ampEnv.getValue();
	}

}

//==============================================================================
bool HourglassGranularAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* HourglassGranularAudioProcessor::createEditor()
{
    return new HourglassGranularAudioProcessorEditor (*this, m_parameters);
}

//==============================================================================
void HourglassGranularAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	auto state = m_parameters.state;
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void HourglassGranularAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(m_parameters.state.getType()))
			m_parameters.state = ValueTree::fromXml(*xmlState);
}

JuicyClouds* HourglassGranularAudioProcessor::getGranularProcessor()
{
	return &m_clouds;
} 

void HourglassGranularAudioProcessor::setAllPassFreq(float freq)
{
	m_apFreq = freq;
	m_allpassLeft.createAllPass(m_apFreq, m_apQ);
	m_allpassRight.createAllPass(m_apFreq, m_apQ);
	m_allpassLeft2.createAllPass(m_apFreq + 500, m_apQ);
	m_allpassRight2.createAllPass(m_apFreq + 500, m_apQ);
}

void HourglassGranularAudioProcessor::setAllPassQ(float freq)
{
	m_apQ = freq;
	m_allpassLeft.createAllPass(m_apFreq, m_apQ);
	m_allpassRight.createAllPass(m_apFreq, m_apQ);
	m_allpassLeft2.createAllPass(m_apFreq + 500, m_apQ);
	m_allpassRight2.createAllPass(m_apFreq + 500, m_apQ);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HourglassGranularAudioProcessor();
}
