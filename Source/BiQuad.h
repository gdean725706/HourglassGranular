/*
  ==============================================================================

    BiQuad.h
    Created: 26 Apr 2018 10:27:10am
    Author:  George Dean

  ==============================================================================
*/

#pragma once
#include <complex>


/*
Pirkle, W. (2012).
Designing audio effect plug-ins in c++ : with digital audio signal processing theory.
pp 157, 181

http://musicdsp.org/files/Audio-EQ-Cookbook.txt
*/

class BiQuad
{
public:
	BiQuad(float sampleRate = 44100.0f);

	void setSampleRate(float sampleRate);

	void createAllPass(float frequency, float Q);

	void process(float* const samples, const int numSamples);

	float processSingle(const float sample);

	void debugPrintCoeffs();

private:
	// User defined
	float Fs; // Sample Rate
	float f0; // Corner Frequency
	float Q;

	// intermediate vars
	float w0, alpha; 

	// coeffs
	float a0, a1, a2, b0, b1, b2;

	// delayed samples
	float x1, x2, y1, y2;

	typedef std::complex<double> Complex;

	const float PI = 3.14159265359f;
};