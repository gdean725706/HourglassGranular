/*
  ==============================================================================

    GrainChannelComponent.h
    Created: 16 Apr 2018 4:03:04pm
    Author:  George Dean

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "JuicyClouds.h"

class WavetableWidget : public Component
{
	std::vector<float> m_hannWindow, m_triWindow, m_sineWindow;
	int m_resolutionFactor;
	bool m_filled;
	float m_blendAmount;

public:
	WavetableWidget() :
		m_hannWindow(64),
		m_triWindow(64),
		m_sineWindow(64),
		m_filled(false),
		m_blendAmount(0)
	{
		fillHann(m_hannWindow);
		fillTriangle(m_triWindow);
		fillSine(m_sineWindow);
	}
	~WavetableWidget()
	{

	}

	void paint(Graphics& g) override
	{
		g.fillAll(getLookAndFeel().findColour(Slider::backgroundColourId));

		g.setColour(getLookAndFeel().findColour(Slider::rotarySliderFillColourId));
		Path windowPath;

		int inc = m_hannWindow.size() / getWidth();

		for (int i = 0; i < m_hannWindow.size() / inc; i++)
		{
			float v(0);

			if (m_blendAmount <= 0.25f)
			{
				float hann = m_hannWindow[i * inc];
				float sine = m_sineWindow[i*inc];
				v = lerp(hann, sine, m_blendAmount * 4.0f);
			}
			else if (m_blendAmount > 0.25f && m_blendAmount <= 0.5f)
			{
				float sine = m_sineWindow[i*inc];
				float triw = m_triWindow[i*inc];
				v = lerp(sine, triw, (m_blendAmount * 4.0f) - 1);
			}
			else if (m_blendAmount > 0.5f && m_blendAmount <= 0.75f)
			{
				float triw = m_triWindow[i*inc];
				v = lerp(triw, 1.0f, (m_blendAmount * 4.0f) - 2);
			}
			else
			{
				v = 1.0f;
			}

			//float v = lerp(m_windowTable[i * inc], m_blendTable[i * inc], m_blendAmount);

			Point<float> p(i, getHeight() - (v * (getHeight() * 0.98f)));
			if (i == 0)
			{
				windowPath.startNewSubPath(p);
			}
			else 
			{
				windowPath.lineTo(p);
			}

			//g.fillPath(i, getHeight() - (m_windowTable[i] * (getHeight() * 0.99f)), 1, 1, 1);
		}
	
		g.strokePath(windowPath, PathStrokeType(2.0f));
	}

	void fillWaveshape(std::vector<float> wavetable)
	{
		m_hannWindow.clear();
		m_hannWindow = wavetable;


		repaint();
	}

	void fillHann(std::vector<float>& table)
	{
		for (int n = 0; n < table.size(); ++n)
		{
			table[n] = 0.5f * (1.0f - cos((6.28318530718 * n) / (float)table.size()));
		}
	}

	void fillSine(std::vector<float>& table)
	{
		for (int n = 0; n < table.size(); ++n)
		{
			table[n] = (sinf((6.28318530718 * n * 0.5 * 1.0) / (float)table.size()));
		}
	}

	void fillTriangle(std::vector<float>& table)
	{
		auto size = table.size() - 1;
		auto f = (1.0 / (size));
		for (int n = 0; n < size; ++n)
		{
			if (n <= size / 2)
			{
				//m_blendTable[n] = (((size - n) * f) - 1.0);
				table[n] = scaleRange((((size - n) * f) - 1.0), 0.0, -0.5, 0.0, 1.0);
			}
			else
			{
				//m_blendTable[n] = ((n*f) - 1.0);
				table[n] = scaleRange(((n * f) - 1.0), 0.0, -0.5, 0.0, 1.0);
			}
		}
	}

	void setBlendAmount(float amount)
	{
		m_blendAmount = amount;
		repaint();
	}

	void resized() override
	{

	}

};

//==============================================================================
/*
*/
class GrainChannelComponent : public GroupComponent, public Button::Listener, public Slider::Listener, public ChangeListener, public Label::Listener
{
	JuicyClouds* m_grainProcessor;
	
	// UI Components
	ScopedPointer<TextButton> m_loadSampleButton;

	ScopedPointer<Slider> m_startPositionSlider;
	ScopedPointer<Slider> m_grainSizeSlider;
	ScopedPointer<Slider> m_pitchRandomnessSlider;
	ScopedPointer<Slider> m_startRandomnessSlider;
	ScopedPointer<Slider> m_mainPitchSlider;
	ScopedPointer<Slider> m_windowBlendSlider;

	AudioFormatManager m_formatManager;
	
	AudioThumbnailCache m_thumbnailCache;
	AudioThumbnail m_sampleThumbnail;
	
	ScopedPointer<WavetableWidget> m_waveWidget;

public:
	GrainChannelComponent(int width, int height) :
		GroupComponent("grainChannel", "Grain Channel"),
		m_grainProcessor(0),
		m_thumbnailCache(5),
		m_sampleThumbnail(512, m_formatManager, m_thumbnailCache)
		
    {
		auto* laf = &getLookAndFeel();
		laf->setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
		laf->setColour(Slider::textBoxTextColourId, getLookAndFeel().findColour(Slider::rotarySliderFillColourId));
		laf->setColour(Label::backgroundWhenEditingColourId, Colours::grey);
		//
		//Colour col = laf->findColour(Slider::thumbColourId);
		//laf->setColour(Slider::thumbColourId, laf->findColour(Slider::backgroundColourId));
		//laf->setColour(Slider::backgroundColourId, col);

		// UI
		m_loadSampleButton = new TextButton("Load Sample");
		addAndMakeVisible(m_loadSampleButton);
		m_loadSampleButton->addListener(this);

		m_startPositionSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_startPositionSlider->setName("Start Position");
		m_startPositionSlider->setRange(0.0, 1.0, 0.001);
		m_startPositionSlider->addListener(this);
		m_startPositionSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_startPositionSlider);

		m_grainSizeSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_grainSizeSlider->setName("Grain Size");
		m_grainSizeSlider->setRange(0.01, 2.0, 0.001);
		m_grainSizeSlider->addListener(this);
		m_grainSizeSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_grainSizeSlider);

		m_mainPitchSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_mainPitchSlider->setName("Pitch");
		m_mainPitchSlider->setRange(-0.2, 0.2);
		m_mainPitchSlider->addListener(this);
		m_mainPitchSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_mainPitchSlider);

		m_pitchRandomnessSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_pitchRandomnessSlider->setName("Random Pitch");
		m_pitchRandomnessSlider->setRange(0.0, 1.0, 0.001);
		m_pitchRandomnessSlider->addListener(this);
		m_pitchRandomnessSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_pitchRandomnessSlider);

		m_startRandomnessSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_startRandomnessSlider->setName("Start Randomness");
		m_startRandomnessSlider->setRange(0.0, 1.0, 0.001);
		m_startRandomnessSlider->addListener(this);
		m_startRandomnessSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_startRandomnessSlider);

		m_windowBlendSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_windowBlendSlider->setName("Window Blend");
		m_windowBlendSlider->setRange(0.0, 1.0, 0.001);
		m_windowBlendSlider->addListener(this);
		m_windowBlendSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_windowBlendSlider);


		// Audio Thumbnail
		m_formatManager.registerBasicFormats();
		m_sampleThumbnail.addChangeListener(this);

		// Window shape widget
		m_waveWidget = new WavetableWidget();
		addAndMakeVisible(m_waveWidget);
    }

    ~GrainChannelComponent()
    {
    }

	void assignGrainProcessor(JuicyClouds* processor)
	{
		m_grainProcessor = processor;
	}

    void paint (Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

        g.setColour (Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (Colours::white);
        g.setFont (14.0f);
        //g.drawText ("GrainChannelComponent", getLocalBounds(),
        //            Justification::centred, true);   // draw some placeholder text

		// Draw waveform
		auto thumb = getLocalBounds();
		thumb.removeFromTop(getHeight() * 0.65f);
		g.setColour(getLookAndFeel().findColour(Slider::backgroundColourId));
		g.fillRect(thumb.reduced(3));

		if (m_sampleThumbnail.getTotalLength() > 0.0)
		{
			g.setColour(getLookAndFeel().findColour(Slider::rotarySliderFillColourId));
			m_sampleThumbnail.drawChannel(g, thumb.reduced(3), 0.0f, m_sampleThumbnail.getTotalLength(), 0, 1.0f);
		}
		else
		{
			g.setColour(Colours::grey);
			g.drawText("No Sample Loaded", thumb, Justification::centred, false);
		}

    }

    void resized() override
    {
		auto bounds = getLocalBounds();

		auto sliderBar = bounds.removeFromTop(getHeight() * 0.5f);
		auto sliderBarWidth = getWidth() * 0.75f;
		auto sliderRight = sliderBar.removeFromRight(sliderBarWidth);

		m_startPositionSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *	0.2f));
		m_grainSizeSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *		0.2f));
		m_mainPitchSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *		0.2f));
		m_pitchRandomnessSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *	0.2f));
		m_startRandomnessSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *	0.2f));

		auto leftSlot = bounds.removeFromLeft(getWidth() * 0.25f);
		auto buttonWidth = leftSlot.removeFromRight(leftSlot.getWidth() * 0.5f);
		m_loadSampleButton->setBounds(leftSlot.removeFromTop(getHeight() * 0.15f).reduced(3));
		m_windowBlendSlider->setBounds(69, 5, 64, 64);
		m_waveWidget->setBounds(5, 5, 64, 64);



    }

	void loadSample()
	{
		if (m_grainProcessor == 0) return;

		m_grainProcessor->setPlayState(false);

		//do loading...

		FileChooser fileSelector("Load sample", File(), "*.wav");

		if (fileSelector.browseForFileToOpen())
		{
			File selectedFile = fileSelector.getResult();
			ScopedPointer<AudioFormatReader> reader = m_formatManager.createReaderFor(selectedFile);
			AudioSampleBuffer buffer(reader->numChannels, reader->lengthInSamples);

			reader->read(&buffer, 0, reader->lengthInSamples, 0, true, false);
			m_grainProcessor->setAudioSize(buffer.getNumSamples());
			for (int i = 0; i < buffer.getNumSamples(); ++i)
			{
				m_grainProcessor->setAudioSample(i, buffer.getSample(0, i));
			}

			m_sampleThumbnail.setSource(new FileInputSource(selectedFile));
		}

		m_grainProcessor->setPlayState(true);
	}


	void buttonClicked(Button* button) override
	{
		if (button == m_loadSampleButton)
		{
			loadSample();
		}
	}

	void sliderValueChanged(Slider* slider) override
	{
		if (m_grainProcessor == 0) return; 

		if (slider == m_grainSizeSlider)
		{
			m_grainProcessor->setGrainSizeMultiplier(m_grainSizeSlider->getValue());
		}
		else if (slider == m_startPositionSlider)
		{
			m_grainProcessor->setStartPosition(m_startPositionSlider->getValue());
		}
		else if (slider == m_pitchRandomnessSlider)
		{
			m_grainProcessor->setPitchRandomness(m_pitchRandomnessSlider->getValue());
		}
		else if (slider == m_startRandomnessSlider)
		{
			m_grainProcessor->setStartRandomness(m_startPositionSlider->getValue());
		}
		else if (slider == m_mainPitchSlider)
		{
			m_grainProcessor->setPitch(m_mainPitchSlider->getValue());
		}
		else if (slider == m_windowBlendSlider)
		{
			m_grainProcessor->setWindowBlendAmount(m_windowBlendSlider->getValue());
			m_waveWidget->setBlendAmount(m_windowBlendSlider->getValue());
		}
	}

	void labelTextChanged(Label* labelThatHasChanged) override
	{

	}

	void changeListenerCallback(ChangeBroadcaster* source) override
	{
		if (source == &m_sampleThumbnail)
		{
			repaint();
		}
	}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainChannelComponent)
};
