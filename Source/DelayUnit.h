#pragma once

class DelayUnit
{
	float* m_delayBuffer;
	int m_maxDelay;
	int m_writePos;

public:
	DelayUnit(int delaySize) :
		m_maxDelay(delaySize),
		m_writePos(0)
	{
		m_delayBuffer = new float[m_maxDelay];
		clearBuffer();
		
	}
	~DelayUnit()
	{
		delete[] m_delayBuffer;
	}

	void clearBuffer()
	{
		for (int i = 0; i < m_maxDelay; ++i)
		{
			m_delayBuffer[i] = 0;
		}
	}

	void writeSample(float sample)
	{
		m_delayBuffer[m_writePos] = sample;
	}

	void tick()
	{
		// Ring buffer
		m_writePos += 1;

		
		while (m_writePos >= m_maxDelay)
		{
			m_writePos -= m_maxDelay;
		}

		while (m_writePos < 0)
		{
			m_writePos += m_maxDelay;
		}
	}

	float getDelay(int delayTime)
	{
		// Sample position to delay from
		int pos = m_writePos - delayTime;

		// Wrap around
		while (pos < 0)
		{
			pos += m_maxDelay;
		}

		// Keep within range
		while (pos > m_maxDelay)
		{
			pos -= m_maxDelay;
		}

		return m_delayBuffer[pos];
	}

	void setMaxDelay(int maxDelay)
	{
		m_maxDelay = maxDelay;
	}

};