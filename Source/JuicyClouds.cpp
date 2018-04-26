/*
  ==============================================================================

	JuicyClouds.cpp
	Created: 15 Jan 2018 9:13:40am
	Author:  George Dean

  ==============================================================================
*/

#include "JuicyClouds.h"

JuicyClouds::JuicyClouds(AudioProcessorValueTreeState& parameters, float sampleRate) :
	m_parameters(parameters),
	m_sampleRate(sampleRate),
	m_childGrains(64),
	m_samplesPerSpawn(4410),
	m_samplesUntilSpawn(0),
	m_hannTable(512),
	m_triwTable(512),
	m_sineTable(512),
	m_audioTable((int)sampleRate),
	m_running(false),
	m_phasorSpeed(1),
	m_grainSizeMultiplier(0.05f),
	m_startPosition(0),
	m_pitchRandomness(0),
	m_startRandomness(0),
	m_masterPitch(0),
	m_blendAmount(0),
	m_panningRandomness(0),
	m_bpm(120.0f),
	m_division(1),
	m_pitchOffset(0)
{
	//m_windowTable.printData();
	m_audioTable.fillSine(440);
	m_triwTable.fillTriangleWindow();
	m_sineTable.fillSineWindow();
	m_playbackPhasor.setFrequency(0.01);
	setGrainPitch(1);
	calculateSamplesPerStep();

}



void JuicyClouds::setSampleRate(float sampleRate)
{
	if (sampleRate <= 0) sampleRate = 44100.0f;
	m_sampleRate = sampleRate;
	m_playbackPhasor.setSampleRate(sampleRate);
}

void JuicyClouds::process(float* leftChannel, float* rightChannel, int blockSize)
{
	if (m_running)
	{
		float start((m_startPosition * 2.0f) * m_audioTable.getSize());
		// iterate over children
		for (int n = 0; n < blockSize; ++n)
		{
			if (m_samplesUntilSpawn <= 0)
			{
				//spawn here
				bool spawned(false);

				for (int child = 0; child < m_childGrains.size(); ++child)
				{
					if (m_startPosition > 0.5f)
					{
						start = m_playbackPhasor.getPhase() * (m_audioTable.getSize() * ((m_startPosition * 2.0f) - 1.0f));
					}

					if (m_childGrains[child].alive() == false && spawned == false)
					{
						m_childGrains[child].spawn(&m_audioTable,
							&m_hannTable,
							&m_triwTable,
							&m_sineTable,
							m_sampleRate,
							m_sampleRate * m_grainSizeMultiplier, // duration samples
							start, //m_playbackPhasor.getPhase() * m_audioTable.getSize() , // start position (phasor this?)
							m_masterPitch + m_pitchOffset, // pitch
							m_startRandomness, // random start amount
							m_pitchRandomness,// random pitch amount
							0.3f, // Gain
							m_blendAmount,
							m_panningRandomness); // Blend amount
						
						spawned = true;
						m_samplesUntilSpawn = m_samplesPerSpawn;
					}
				}
			} 

			for (int child = 0; child < m_childGrains.size(); ++child)
			{
				if (m_childGrains[child].alive() == true)
				{
					m_childGrains[child].updatePitch(m_phasorSpeed + m_masterPitch);
					m_childGrains[child].process(leftChannel, rightChannel, n);
				}
				m_playbackPhasor.tick();
			}
			--m_samplesUntilSpawn;
		}
	}
}

void JuicyClouds::setPlayState(bool state)
{
	m_running = state;
}

void JuicyClouds::setAudioSize(int size)
{
	m_audioTable.setSize(size);
	calculatePhasorSpeed();
}

void JuicyClouds::setAudioSample(int index, float value)
{
	m_audioTable.setSample(index, value);
	calculatePhasorSpeed();
}

void JuicyClouds::calculatePhasorSpeed()
{
	m_phasorSpeed = m_sampleRate / m_audioTable.getSize();
}

void JuicyClouds::setGrainSizeMultiplier(float multiplier)
{
	m_grainSizeMultiplier = multiplier;
}

void JuicyClouds::setStartPosition(float position)
{
	m_startPosition = position;
}

void JuicyClouds::setPitchRandomness(float amount)
{
	m_pitchRandomness = amount;
}

void JuicyClouds::setStartRandomness(float amount)
{
	m_startRandomness = amount;
}

void JuicyClouds::setPitch(float pitch)
{
	m_pitchOffset = pitch;
}

void JuicyClouds::setWindowBlendAmount(float amount)
{
	m_blendAmount = amount;
}

void JuicyClouds::setPanningRandomness(float amount)
{
	m_panningRandomness = amount;
}

void JuicyClouds::calculateSamplesPerStep()
{
	m_samplesPerSpawn = (m_sampleRate * 60.0f / m_bpm) / m_division;
	DBG(m_samplesPerSpawn);
}

void JuicyClouds::setBPM(float bpm)
{
	m_bpm = bpm;
	calculateSamplesPerStep();
}

void JuicyClouds::setDivision(float div)
{
	m_division = div;
	calculateSamplesPerStep();
}

void JuicyClouds::setGrainPitch(float midiValue)
{
	float phasorSpeed = m_sampleRate / m_audioTable.getSize();

	phasorSpeed *= exp2f((midiValue - 69) / 11);

	m_masterPitch = phasorSpeed;

}