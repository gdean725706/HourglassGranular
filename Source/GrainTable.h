/*
  ==============================================================================

    GrainTable.h
    Created: 15 Jan 2018 11:22:06am
    Author:  George Dean

  ==============================================================================
*/

#pragma once
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "math.h"

typedef std::vector<float> AudioVector;

template <class T>
T scaleRange(T input, T inputStart, T inputEnd, T outputStart, T outputEnd)
{
	return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}

class GrainTable
{
	AudioVector m_data;
	bool m_ready;

	//http://paulbourke.net/miscellaneous/interpolation/
	double CubicInterpolate(
		double y0, double y1,
		double y2, double y3,
		double mu)
	{
		double a0, a1, a2, a3, mu2;

		mu2 = mu*mu;
		a0 = y3 - y2 - y0 + y1;
		a1 = y0 - y1 - a0;
		a2 = y2 - y0;
		a3 = y1;

		return(a0*mu*mu2 + a1*mu2 + a2*mu + a3);
	}

	void ready()
	{
		m_ready = true;
	}

public:
	GrainTable(int size) :
		m_data(size)
	{
		//fillSine();
		fillHann();
	}

	void printData()
	{
		for (auto v : m_data)
		{
			DBG(v);
		}
	}

	void fillSine(float frequency=1)
	{
		int size = m_data.size() - 1;

		for (int n = 0; n < size; ++n)
		{
			m_data[n] = sinf((6.28318530718 * n * frequency) / size);
		}
		ready();
	}

	void fillSineWindow()
	{
		int size = m_data.size() - 1;

		for (int n = 0; n < size; ++n)
		{
			m_data[n] = sinf((6.28318530718 * n * 0.5 * 1.0) / size);
		}
		ready();
	}

	void fillTriangleWindow()
	{
		int size = m_data.size() - 1;
		
		auto f = 1.0 / (size - 1);
		for (int n = 0; n < size; ++n)
		{
			if (n <= size / 2)
			{
				//m_blendTable[n] = (((size - n) * f) - 1.0);
				m_data[n] = scaleRange((((size - n) * f) - 1.0), 0.0, -0.5, 0.0, 1.0);
			}
			else
			{
				//m_blendTable[n] = ((n*f) - 1.0);
				m_data[n] = scaleRange(((n * f) - 1.0), 0.0, -0.5, 0.0, 1.0);
			}
		}
		ready();
	}

	void fillHann()
	{
		int size = m_data.size() - 1;

		for (int n = 0; n < size; ++n)
		{
			m_data[n] = 0.5f * (1.0f - cos((6.28318530718 * n) / (float)size));
		}
		ready();
	}
	
	void rangeMap(double& value)
	{
		int size = getSize();

		while (value < 0) value += size;
		while (value > size) value -= size;
	}
	
	float getSample(float position)
	{
		int size = m_data.size();

		// whole num check to skip cubic interp
		if (position == (int)position)
		{
			double p = position;;
			rangeMap(p);

			return m_data[p];
		}

		// cubic interp
		double lower, remainder;
		remainder = modf(position, &lower);

		double y0 = lower - 1;
		double y1 = lower;
		double y2 = lower + 1;
		double y3 = lower + 2;

		rangeMap(y0);
		rangeMap(y1);
		rangeMap(y2);
		rangeMap(y3);

		double y0Val(m_data[y0]), y1Val(m_data[y1]), y2Val(m_data[y2]), y3Val(m_data[y3]);
		auto v = CubicInterpolate(y0Val, y1Val, y2Val, y3Val, remainder);
		return v;
	}

	bool isReady()
	{
		return m_ready;
	}
	int getSize()
	{
		if (m_ready)
		{
			return m_data.size() - 1;
		}
		else return 0;
	}

	void setSize(int size)
	{
		m_ready = false;
		m_data.resize(size);
		m_ready = true;
	}

	void setSample(int index, float value)
	{
		if (index < 0 || index >= m_data.size()) return;
		m_data[index] = value;
	}

};