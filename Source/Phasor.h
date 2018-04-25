/*
  ==============================================================================

    Phasor.h
    Created: 29 Jan 2018 9:51:40am
    Author:  George

  ==============================================================================
*/

#pragma once

class Phasor
{
	float m_phase, m_freq, m_delta, m_sampleRate;

	void clampPhase()
	{
		while (m_phase >= 1.0f)
		{
			m_phase -= 1.0f;
		}

		while (m_phase < 0)
		{
			m_phase += 1.0f;
		}	
	}

public:
	Phasor(float sampleRate = 44100, float freq = 1):
		m_phase(0),
		m_freq(freq),
		m_sampleRate(sampleRate),
		m_delta(m_freq/m_sampleRate)
	{

	}

	float getPhase()
	{
		return m_phase;
	}

	void setPhase(float phase)
	{
		m_phase = phase;
	}

	void setFrequency(float freq)
	{
		m_freq = freq;
		m_delta = m_freq / m_sampleRate;
		clampPhase();
	}

	float getFrequency()
	{
		return m_freq;
	}

	void tick()
	{
		m_phase += m_delta;
		clampPhase();
	}

	void setSampleRate(float sampleRate)
	{
		m_sampleRate = sampleRate > 0 ? sampleRate : m_sampleRate;
		m_delta = m_freq / m_sampleRate;
	}

};