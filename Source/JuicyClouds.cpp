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
	m_pitchRandomness(0),
	m_masterPitch(0),
	m_pitchOffset(0)
{

	parameters.createAndAddParameter(
		"grainSizeMultiplier",
		"Grain Size Multiplier",
		"grain size",
		NormalisableRange<float>(0.01, 2.0, 0.001),
		1.0,
		nullptr, nullptr);

	parameters.createAndAddParameter(
		"grainStartPosition",
		"Grain Start Position",
		"start",
		NormalisableRange<float>(0.0, 1.0, 0.001),
		0,
		nullptr, nullptr);

	parameters.createAndAddParameter(
		"startRandomness",
		"Random Start",
		"random start",
		NormalisableRange<float>(0.0, 1.0, 0.001),
		0,
		nullptr, nullptr);

	parameters.createAndAddParameter(
		"pitchRandomness",
		"Random Pitch",
		"random pitch",
		NormalisableRange<float>(0.0, 1.0, 0.001),
		0.0f,
		nullptr,
		nullptr);

	parameters.createAndAddParameter(
		"windowBlend",
		"Window Blend",
		"blend",
		NormalisableRange<float>(0.0, 1.0, 0.001),
		0,
		nullptr, nullptr);

	parameters.createAndAddParameter(
		"randomPan",
		"Random Pan",
		"random pan",
		NormalisableRange<float>(0.0, 1.0, 0.001),
		0,
		nullptr, nullptr);

	parameters.createAndAddParameter(
		"masterTempo",
		"Master Tempo",
		"bpm",
		NormalisableRange<float>(60.0f, 200.0f, 0.5f),
		120.0f,
		nullptr, nullptr);

	parameters.createAndAddParameter(
		"tempoDivision",
		"Tempo Division",
		"div",
		NormalisableRange<float>(0.25, 16.0, 0.25),
		1.0f,
		nullptr, nullptr);

	parameters.addParameterListener("masterTempo", this);
	parameters.addParameterListener("tempoDivision", this);

	//parameters.state = ValueTree(Identifier("HourglassGranular"));

	m_grainSizeMultiplier = parameters.getRawParameterValue("grainSizeMultiplier");
	m_startPosition = parameters.getRawParameterValue("grainStartPosition");
	m_startRandomness = parameters.getRawParameterValue("startRandomness");
	m_pitchRandomness = parameters.getRawParameterValue("pitchRandomness");
	m_blendAmount = parameters.getRawParameterValue("windowBlend");
	m_panningRandomness = parameters.getRawParameterValue("randomPan");

	m_bpm = parameters.getRawParameterValue("masterTempo");
	m_division = parameters.getRawParameterValue("tempoDivision");

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

void JuicyClouds::process(float* leftChannel, float* rightChannel, int blockSize, AudioPlayHead* playhead)
{
	if (m_running)
	{
		float start((*m_startPosition * 2.0f) * m_audioTable.getSize());

		if (playhead != nullptr)
		{
			AudioPlayHead::CurrentPositionInfo info;
			playhead->getCurrentPosition(info);

			if (info.bpm != 0)
				setBPM(info.bpm);

		}

		// Iterate thru grains
		for (int n = 0; n < blockSize; ++n)
		{
			if (m_samplesUntilSpawn <= 0)
			{
				//spawn here
				bool spawned(false);

				for (int child = 0; child < m_childGrains.size(); ++child)
				{
					Random r;
					float rStart = *m_startPosition * (r.nextFloat() * *m_startRandomness);

					if (*m_startPosition > 0.5f)
					{
						start = m_playbackPhasor.getPhase() * (m_audioTable.getSize() * ((rStart * 2.0f) - 1.0f));
					}

					if (m_childGrains[child].alive() == false && spawned == false)
					{
						m_childGrains[child].spawn(&m_audioTable,
							&m_hannTable,
							&m_triwTable,
							&m_sineTable,
							m_sampleRate,
							m_sampleRate * *m_grainSizeMultiplier, // duration samples
							start, //m_playbackPhasor.getPhase() * m_audioTable.getSize() , // start position (phasor this?)
							m_masterPitch, // pitch
							*m_startRandomness, // random start amount
							*m_pitchRandomness,// random pitch amount
							0.3f, // Gain
							*m_blendAmount,
							*m_panningRandomness); // Blend amount
						
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


void JuicyClouds::setPitch(float pitch)
{
	m_pitchOffset = pitch;
}

void JuicyClouds::calculateSamplesPerStep()
{
	m_samplesPerSpawn = (m_sampleRate * 60.0f / *m_bpm) / *m_division;
	//DBG(m_samplesPerSpawn);
}

void JuicyClouds::setBPM(float bpm)
{
	*m_bpm = bpm;
	calculateSamplesPerStep();
}

void JuicyClouds::setDivision(float div)
{
	*m_division = div;
	calculateSamplesPerStep();
}

void JuicyClouds::setGrainPitch(float midiValue)
{
	float phasorSpeed = m_sampleRate / m_audioTable.getSize();

	phasorSpeed *= exp2f((midiValue - 69) / 11);

	m_masterPitch = phasorSpeed;

}

void JuicyClouds::parameterChanged(const String& parameterID, float newValue)
{
	if (parameterID == "masterTempo")
	{
		calculateSamplesPerStep();
	}
	else if (parameterID == "tempoDivision")
	{
		calculateSamplesPerStep();
	}
}