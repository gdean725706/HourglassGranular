/*
  ==============================================================================

    Diffuser.h
    Created: 26 Apr 2018 11:53:12am
    Author:  George Dean

  ==============================================================================
*/

#pragma once
#include "BiQuad.h"
#include "DelayUnit.h"

class Diffuser
{
public:
	Diffuser(float sampleRate) :
		m_sampleRate(sampleRate)
	//	m_dL1(180), m_dL2(269), m_dL3(444)
	{

	}
private:

	//DelayUnit m_dL1, m_dL2, m_dL3, m_dL4;
	//BiQuad m_allpass1L, m_allpass2L, m_allpass3L, m_allpass4L;

	//DelayUnit m_dR1, m_dR2, m_dR3, m_dR4;
	//BiQuad m_allpass1R, m_allpass2R, m_allpass3R, m_allpass4R;

	float m_sampleRate;
};