/*
  ==============================================================================

    PhaseShifter.h
    Created: 26 Apr 2018 1:26:52pm
    Author:  George Dean

  ==============================================================================
*/

#pragma once

#include "BiQuad.h"
#include "Phasor.h"
#include "JuceHeader.h"

class PhaseShifter : AudioProcessorValueTreeState::Listener
{
public:
	PhaseShifter(AudioProcessorValueTreeState& parameters, float sampleRate = 44100.0f) :
		m_sampleRate(sampleRate)
	{
		parameters.createAndAddParameter(
			"phaserDepth",
			"Phase Shift Depth",
			"phase depth",
			NormalisableRange<float>(0.0, 1.0, 0.001),
			0.0f,
			nullptr, nullptr);

		parameters.createAndAddParameter(
			"phaserSpeed",
			"Phase Shift Speed",
			"phase speed",
			NormalisableRange<float>(0.02, 10.0, 0.01),
			0.0f,
			nullptr, nullptr);

		parameters.createAndAddParameter(
			"phaserQ",
			"Phase Q",
			"phase q",
			NormalisableRange<float>(0.5, 2.5, 0.01),
			1.0f,
			nullptr, nullptr);

		m_phasorIntensity = parameters.getRawParameterValue("phaserDepth");
		m_phasorSpeed = parameters.getRawParameterValue("phaserSpeed");
		m_phaseQ = parameters.getRawParameterValue("phaserQ");

		m_allpass1.createAllPass(1000.0f, 1.0f);
		m_allpass2.createAllPass(1000.0f, 1.0f);
		m_allpass3.createAllPass(1000.0f, 1.0f);
		m_allpass4.createAllPass(1000.0f, 1.0f);
		m_allpass5.createAllPass(1000.0f, 1.0f);
		m_allpass6.createAllPass(1000.0f, 1.0f);

		m_phasor.setFrequency(0.3f);
	}

	void prepareToPlay(double sampleRate)
	{
		m_allpass1.setSampleRate(sampleRate);
		m_allpass2.setSampleRate(sampleRate);
		m_allpass3.setSampleRate(sampleRate);
		m_allpass4.setSampleRate(sampleRate);
		m_allpass5.setSampleRate(sampleRate);
		m_allpass6.setSampleRate(sampleRate);
		m_phasor.setSampleRate(sampleRate);
	}

	void process(float* leftChannel, float* rightChannel, int blockSize)
	{
		if (*m_phasorIntensity == 0) return;

		for (int i = 0; i < blockSize; ++i)
		{
			float phase = m_phasor.getPhase();
			float t = abs(phase - 0.5) * abs(phase - 0.5);
			t = scaleRange(t, 0.0f, 0.25f, 800.0f, 6000.0f) * *m_phasorIntensity;
			//t *= 5000;

			m_allpass1.createAllPass(t, *m_phaseQ);
			m_allpass2.createAllPass(t, *m_phaseQ);
			m_allpass3.createAllPass(t, *m_phaseQ);
			m_allpass4.createAllPass(t, *m_phaseQ);
			m_allpass5.createAllPass(t, *m_phaseQ);
			m_allpass6.createAllPass(t, *m_phaseQ);

			leftChannel[i] += m_allpass1.processSingle(leftChannel[i]);
			rightChannel[i] += m_allpass2.processSingle(rightChannel[i]);
			leftChannel[i] += m_allpass3.processSingle(leftChannel[i]);
			rightChannel[i] += m_allpass4.processSingle(rightChannel[i]);
			leftChannel[i] += m_allpass5.processSingle(leftChannel[i]);
			rightChannel[i] += m_allpass6.processSingle(rightChannel[i]);

			m_phasor.tick();
		}
	}

	void parameterChanged(const String &parameterID, float newValue)
	{
		if (parameterID == "phaserSpeed")
		{
			m_phasor.setFrequency(newValue);
		}
	}
private:
	float m_sampleRate;

	BiQuad m_allpass1, m_allpass2, m_allpass3, m_allpass4, m_allpass5, m_allpass6;
	Phasor m_phasor;
	
	float* m_phasorSpeed = nullptr, *m_phasorIntensity = nullptr, *m_phaseQ = nullptr;
};
