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

class PhaseShifter
{
public:
	PhaseShifter(float sampleRate) :
		m_sampleRate(sampleRate)
	{

	}

	void process(float* leftChannel, float* rightChannel, int blockSize)
	{

	}

private:
	float m_sampleRate;
	
	float m_phasorSpeed, m_phasorIntensity;
};
