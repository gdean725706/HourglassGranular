/*
  ==============================================================================

    BiQuad.cpp
    Created: 26 Apr 2018 10:27:10am
    Author:  George Dean

  ==============================================================================
*/

#include "BiQuad.h"


/*
w0 = 2*pi*f0/Fs
cos(w0)
sin(w0)

alpha = sin(w0)/(2*Q) (case: Q)
*/

BiQuad::BiQuad(float sampleRate) :
	Fs(sampleRate), f0(0), Q(0),
	w0(0), alpha(0),
	a0(0), a1(0), a2(0), b0(0), b1(0), b2(0)
{

}

void BiQuad::setSampleRate(float sampleRate)
{
	Fs = sampleRate;
}

/*
APF:        H(s) = (s^2 - s/Q + 1) / (s^2 + s/Q + 1)

b0 =   1 - alpha
b1 =  -2*cos(w0)
b2 =   1 + alpha
a0 =   1 + alpha
a1 =  -2*cos(w0)
a2 =   1 - alpha
*/

void BiQuad::createAllPass(float frequency, float Q)
{
	f0 = frequency;
	w0 = (2.0f * PI) * (f0 / Fs);
	alpha = std::sin(w0) / (2.0*Q);
	float cw = std::cos(w0); // 1 less cos calc ;D
	b0 = 1 - alpha;
	b1 = -2 * cw;
	b2 = 1 + alpha;
	a0 = 1 + alpha;
	a1 = -2 * cw;
	a2 = 1 - alpha;

	//debugPrintCoeffs();
}

/*
Direct Form 1:
y[n] = (b0/a0)*x[n] + (b1/a0)*x[n-1] + (b2/a0)*x[n-2]
- (a1/a0)*y[n-1] - (a2/a0)*y[n-2]
*/
void BiQuad::process(float* const samples, const int numSamples)
{
	for (int i = 0; i < numSamples; ++i)
	{
		const float x0 = samples[i];
		float y0(0);
		y0 = (b0 / a0)*x0 + (b1 / a0)*x1 + (b2 / a0)*x2 - (a1 / a0)*y1 - (a2 / a0)*y2;
		x2 = x1;
		x1 = x0;
		y2 = y1;
		y1 = y0;
		samples[i] += y0;
	}
}

void BiQuad::debugPrintCoeffs()
{
	//DBG(a0);
	//DBG(a1);
	//DBG(a2);
	//DBG(b0);
	//DBG(b2);
	//DBG(b1);
}