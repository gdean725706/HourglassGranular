/*
  ==============================================================================

    PitchShifter.h
    Created: 12 Feb 2018 3:45:01pm
    Author:  George Dean

  ==============================================================================
*/

#pragma once
//fout = fin * (1 - pfreq * (dw)/1000)
#include "JuceHeader.h"
#include "Phasor.h"
#include "GrainTable.h"
#include "DelayUnit.h"
template <class T>
T wrap(T value)
{
	while (value > 1.0)
	{
		value -= 1;
	}
	while (value < 0.0)
	{
		value += 1;
	}

	return value;
}
//http://msp.ucsd.edu/techniques/v0.11/book-html/node125.html
//http://www.katjaas.nl/pitchshift/pitchshift.html
class PitchShifter : AudioProcessorValueTreeState::Listener
{
public:
	PitchShifter(AudioProcessorValueTreeState& parameters) :
		m_transpose(44100.0f/2048.0f),
		m_delayTime(2048),
		m_leftDelay(2048),
		m_rightDelay(2048),
		m_window(512),
		m_phasor(44100.0f, 1.0f)
	{

		parameters.createAndAddParameter(
			"pitchShiftFrequency",
			"Pitch Shift Frequency",
			"pitch",
			NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
			0.0f,
			nullptr, nullptr);
		parameters.addParameterListener("pitchShiftFrequency", this);

		m_frequency = parameters.getRawParameterValue("pitchShiftFrequency");

		m_window.fillHann();
	}

	void prepareToPlay(double sampleRate, int maxExpectedSamplesPerBlock)
	{
		m_phasor.setSampleRate(sampleRate);
		m_transpose = sampleRate / 2048.0f;
	}

	void releaseResources()
	{
		m_leftDelay.clearBuffer();
		m_rightDelay.clearBuffer();
	}

	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
	{
		if (*m_frequency == 0.0f) return;

		float* leftChannel = buffer.getWritePointer(0, 0);
		float* rightChannel = buffer.getWritePointer(1, 0);

		for (int i = 0; i < buffer.getNumSamples(); ++i)
		{
			// Write to delay
			m_leftDelay.writeSample(leftChannel[i]);
			m_rightDelay.writeSample(rightChannel[i]);

			// phase and inv phase for each tape head and overlapping hann windows
			float phase = m_phasor.getPhase();
			float offsetPhase = wrap(phase + 0.5);

			// fetch modded delay
			float leftDelay = m_leftDelay.getDelay(phase * m_delayTime);
			float rightDelay = m_rightDelay.getDelay(phase * m_delayTime);
			
			// fetch inv delay
			float invleftDelay = m_leftDelay.getDelay(offsetPhase * m_delayTime);
			float invrightDelay = m_rightDelay.getDelay(offsetPhase * m_delayTime);

			// get windows
			float window = m_window.getSample(phase * m_window.getSize());
			float offsetWindow = m_window.getSample(offsetPhase * m_window.getSize());

			// Sum everything & mix in dry
			leftChannel[i] = (leftDelay * window + invleftDelay * offsetWindow);
			rightChannel[i] = (rightDelay * window + invrightDelay * offsetWindow);

			m_phasor.tick();
			m_leftDelay.tick();
			m_rightDelay.tick();
		}
	}

	void parameterChanged(const String &parameterID, float newValue)
	{
		if (parameterID == "pitchShiftFrequency")
		{
			setFrequency(newValue);
		}
	}

	void setFrequency(float frequency)
	{
		*m_frequency = frequency;
		m_phasor.setFrequency((((pow(2.0f, frequency / 12.0f) - 1)*-1.0f)*m_transpose));
	}

private:
	float* m_frequency = nullptr;


	int m_transpose, m_delayTime;
	DelayUnit m_leftDelay, m_rightDelay;
	GrainTable m_window;
	Phasor m_phasor;


};