/*
  ==============================================================================

	Grain.h
	Created: 15 Jan 2018 9:13:08am
	Author:  George Dean

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "GrainTable.h"
#include "Phasor.h"

//https://devblogs.nvidia.com/lerp-faster-cuda/
template <class T>
T lerp(T v0, T v1, T t)
{
	return fma(t, v1, fma(-t, v0, v0));
}

struct WindowShapes
{
	GrainTable* hanning;
	GrainTable* blendWindow;
	GrainTable* sineWindow;
};

class Grain
{
	bool m_alive, m_running;
	int m_lifeTimeSamples, m_timeAliveSamples;
	float m_startPosition, m_currentOutput, m_gain, m_blendAmount;
	float m_panning;

	WindowShapes m_windows;

	GrainTable* m_hannWindow, *m_triWindow, *m_sineWindow, *m_audio;
	Phasor m_phasor;

public:
	Grain() :
		m_alive(false),
		m_running(false),
		m_startPosition(0),
		m_currentOutput(0),
		m_phasor(44100),
		m_gain(0.3f),
		m_blendAmount(0.0f),
		m_panning(0.5f)
	{
		m_hannWindow = 0;
		m_triWindow = 0;
		m_sineWindow = 0;
		m_audio = 0;
	}
	bool alive()
	{
		return m_alive;
	}

	void spawn(GrainTable* audio,
		GrainTable* hann,
		GrainTable* triw,
		GrainTable* sinew,
		float sampleRate,
		int durationSamples,
		float startPosition = 0,
		float pitch = 1, 
		float randomStart = 0,
		float randomPitch = 0, 
		float gain = 0.3f,
		float blendAmount = 0.0f,
		float panningRandomness = 0.0f)
	{
		m_lifeTimeSamples = durationSamples;
		m_timeAliveSamples = 0;
		m_startPosition = startPosition;
		m_phasor.setSampleRate(sampleRate);
		m_phasor.setPhase(0);
		m_gain = gain;
		m_blendAmount = blendAmount;

		Random r;
		float rng = ((r.nextFloat() * 2.0f) - 1.0f);
		float rPitch = rng * randomPitch;
		m_panning = rng * panningRandomness;
		
		m_phasor.setFrequency(pitch + rPitch);

		if (audio != nullptr && hann != nullptr)
		{
			if (audio->isReady() == true && audio->getSize() > 0 &&
				hann->isReady() == true && hann->getSize() > 0)
			{

				m_audio = audio;
				m_hannWindow = hann;
				m_triWindow = triw;
				m_sineWindow = sinew;



				m_alive = true;
				m_running = true;
			}
			else
			{
				m_alive = false;
				m_running = false;
			}
		}
		else
		{
			m_alive = false;
			m_running = false;
		}
		/*
		static int grainNum(0);
		++grainNum;

		DBG(grainNum);*/
	}

	void process(float* left, float* right, int position)
	{
		if (m_alive == false || m_running == false || m_audio == nullptr) return;

		// write to audio buffers here...
		float grainPos01 = (float)m_timeAliveSamples / (float)m_lifeTimeSamples;


		float amp(0);

		if (m_blendAmount <= 0.25f)
		{
			float hann = m_hannWindow->getSample(grainPos01 * (float)m_hannWindow->getSize());
			float sine = m_sineWindow->getSample(grainPos01 * (float)m_sineWindow->getSize());

			amp = lerp(hann, sine, m_blendAmount * 4.0f);
		}
		else if (m_blendAmount > 0.25f && m_blendAmount <= 0.5f)
		{
			float sine = m_sineWindow->getSample(grainPos01 * (float)m_sineWindow->getSize());
			float tri = m_triWindow->getSample(grainPos01 * (float)m_triWindow->getSize());

			amp = lerp(sine, tri, (m_blendAmount * 4.0f) - 0.25f);
		}
		else if (m_blendAmount > 0.5f && m_blendAmount <= 0.75f)
		{
			float tri = m_triWindow->getSample(grainPos01 * (float)m_triWindow->getSize());
			
			amp = lerp(tri, 1.0f, (m_blendAmount * 4.0f) - 0.5f);
		}
		else
		{
			amp = 1.0f;
		}



		m_currentOutput = m_audio->getSample(m_startPosition + float(m_phasor.getPhase() * m_audio->getSize())) * amp;
		m_phasor.tick();

		m_currentOutput *= m_gain;

		left[position] += m_currentOutput * std::min(1.0f - m_panning, 1.0f);
		right[position] += m_currentOutput * std::min(1.0f + m_panning, 1.0f);

		++m_timeAliveSamples;

		if (m_timeAliveSamples >= m_lifeTimeSamples)
		{
			m_running = false;
			m_alive = false;
		}

	}
};