/*
  ==============================================================================

    JuicyClouds.h
    Created: 15 Jan 2018 9:13:40am
    Author:  George Dean

  ==============================================================================
*/

#pragma once
#include <vector>
#include "Grain.h"
#include "GrainTable.h"

typedef std::vector<Grain> GrainVector;

class JuicyClouds
{
	float m_sampleRate;
	GrainVector m_childGrains;
	int m_samplesPerSpawn, m_samplesUntilSpawn;

	GrainTable m_hannTable, m_triwTable, m_sineTable, m_audioTable;

	bool m_running;
	float m_phasorSpeed;

	Phasor m_playbackPhasor;

	float m_grainSizeMultiplier, m_startPosition, m_pitchRandomness, m_startRandomness;
	float m_masterPitch;
	float m_blendAmount;
	float m_panningRandomness;

	float m_bpm;
	float m_division;
	float m_pitchOffset;

public:
	JuicyClouds(float sampleRate = 44100.0f);
	void setSampleRate(float samplerate);

	void process(float* leftChannel, float* rightChannel, int blockSize);

	void setPlayState(bool state);

	void setAudioSize(int size);
	void setAudioSample(int index, float value);
	void calculatePhasorSpeed();

	void setGrainSizeMultiplier(float amount);
	void setStartPosition(float position);
	void setPitchRandomness(float amount);
	void setStartRandomness(float amount);
	void setPitch(float pitch);
	void setWindowBlendAmount(float amount);
	void setPanningRandomness(float amount);

	void calculateSamplesPerStep();
	void setBPM(float bpm);
	void setDivision(float div);

	void setGrainPitch(float midiValue);
};