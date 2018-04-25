/*
  ==============================================================================

	EnvelopeGenerator.h
	Created: 25 Apr 2018 11:08:05pm
	Author:  George Dean

  ==============================================================================
*/

#pragma once

class EnvelopeGenerator {
public:
	enum State {
		OFF = 0,
		ATTACK,
		DECAY,
		SUSTAIN,
		RELEASE
	};
private:
	float m_sampleRate;
	float m_attack, m_decay, m_sustain, m_release;
	State m_currentState;
	float m_currentValue;
	float m_attackInc, m_decayInc, m_releaseInc;

public:
	EnvelopeGenerator(float SampleRate, float attack = 0.5f, float decay = 0.5f, float sustain = 0.5f, float release = 0.5f) :
		m_sampleRate(SampleRate),
		m_attack(attack),
		m_decay(decay),
		m_sustain(sustain),
		m_release(release),
		m_currentState(OFF),
		m_attackInc(0),
		m_decayInc(0)
	{
		calculateAttack();
		calculateDecay();
		calculateRelease();

	}

	~EnvelopeGenerator()
	{
	}

	void trigger() 
	{
		if (m_currentState == SUSTAIN)
		{
			m_currentState = SUSTAIN;
			return;
		}
		m_currentState = ATTACK;
		m_currentValue = 0;
	}

	void release()
	{

		if (m_currentState == ATTACK || m_currentState == DECAY)
		{

			m_releaseInc = m_currentValue / (m_release * m_sampleRate);

		}

		m_currentState = RELEASE;

	}

	void setSampleRate(float samplerate)
	{
		m_sampleRate = samplerate;
		calculateAttack();
		calculateDecay();
		calculateRelease();
	}

	void setAttack(float attack)
	{
		m_attack = attack;
		calculateAttack();

	}
	void setDecay(float decay)
	{
		m_decay = decay;
		calculateDecay();
	}

	void setSustain(float sustain) 
	{
		m_sustain = sustain;
	}

	void setRelease(float release) 
	{
		m_release = release;
		calculateRelease();
	}

	void calculateAttack() 
	{

		m_attackInc = 1.0f / (m_attack * m_sampleRate);

	}

	void calculateDecay() 
	{

		m_decayInc = (1.0f - m_sustain) / (m_decay * m_sampleRate);

	}

	void calculateRelease() 
	{

		m_releaseInc = m_sustain / (m_release * m_sampleRate);

	}

	float getValue() 
	{

		switch (m_currentState)
		{
		case ATTACK:
			m_currentValue += m_attackInc;
			if (m_currentValue >= 1.0f)
			{
				m_currentState = DECAY;
			}
			break;
		case DECAY:
			m_currentValue -= m_decayInc;
			if (m_currentValue <= m_sustain)
			{
				m_currentState = SUSTAIN;
			}
			break;
		case RELEASE:
			m_currentValue -= m_releaseInc;
			if (m_currentValue <= 0.0f) 
			{
				m_currentState = OFF;
				m_currentValue = 0.0f;
			}
			break;
		default:
			break;
		}

		return m_currentValue;
	}

};