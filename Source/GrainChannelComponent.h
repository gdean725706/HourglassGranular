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
				float hann = m_hannWindow[i * inc];
				v = lerp(1.0f, hann, (m_blendAmount * 4.0f) - 3);
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
class GrainChannelComponent : 
	public GroupComponent, public Button::Listener, public Slider::Listener,
	public ChangeListener, public Label::Listener, public ValueTree::Listener
{


public:
	
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

	GrainChannelComponent(AudioProcessorValueTreeState& vts) :
		GroupComponent("grainChannel", "Grain Channel"),
		m_grainProcessor(0),
		m_thumbnailCache(5),
		m_sampleThumbnail(512, m_formatManager, m_thumbnailCache),
		m_valueTreeState(vts)
    {
		auto* laf = &getLookAndFeel();
		laf->setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
		laf->setColour(Slider::textBoxTextColourId, getLookAndFeel().findColour(Slider::rotarySliderFillColourId));
		laf->setColour(Label::backgroundWhenEditingColourId, Colours::grey);
		laf->setColour(Label::textColourId, laf->findColour(Slider::rotarySliderFillColourId));
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
		m_startPosLabel = new Label("lblStartPos", "start");
		m_startPosLabel->setJustificationType(Justification::centred);
		addAndMakeVisible(m_startPosLabel);
		m_startPosLabel->attachToComponent(m_startPositionSlider, false);
		m_startPositionAttachment.reset(new SliderAttachment(m_valueTreeState, "grainStartPosition", *m_startPositionSlider));

		m_grainSizeSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_grainSizeSlider->setName("Grain Size");
		m_grainSizeSlider->setRange(0.01, 2.0, 0.001);
		m_grainSizeSlider->addListener(this);
		m_grainSizeSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_grainSizeSlider);
		m_grainSizeLabel = new Label("lblGrainSize", "size");
		m_grainSizeLabel->setJustificationType(Justification::centred);
		addAndMakeVisible(m_grainSizeLabel);
		m_grainSizeLabel->attachToComponent(m_grainSizeSlider, false);
		m_grainSizeAttachment.reset(new SliderAttachment(m_valueTreeState, "grainSizeMultiplier", *m_grainSizeSlider));

		m_mainPitchSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_mainPitchSlider->setName("Pitch");
		m_mainPitchSlider->setRange(-12.0f, 12.0f, 0.1f);
		m_mainPitchSlider->addListener(this);
		m_mainPitchSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_mainPitchSlider);
		m_mainPitchLabel = new Label("lblMainPitch", "pitch");
		m_mainPitchLabel->setJustificationType(Justification::centred);
		addAndMakeVisible(m_mainPitchLabel);
		m_mainPitchLabel->attachToComponent(m_mainPitchSlider, false);
		m_mainPitchAttachment.reset(new SliderAttachment(m_valueTreeState, "pitchShiftFrequency", *m_mainPitchSlider));

		m_pitchRandomnessSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_pitchRandomnessSlider->setName("Random Pitch");
		m_pitchRandomnessSlider->setRange(0.0, 1.0, 0.001);
		m_pitchRandomnessSlider->addListener(this);
		m_pitchRandomnessSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_pitchRandomnessSlider);
		m_pitchRandomnessLabel = new Label("lblPitchRnd", "random pitch");
		m_pitchRandomnessLabel->setJustificationType(Justification::centred);
		addAndMakeVisible(m_pitchRandomnessLabel);
		m_pitchRandomnessLabel->attachToComponent(m_pitchRandomnessSlider, false);
		m_randomPitchAttachment.reset(new SliderAttachment(m_valueTreeState, "pitchRandomness", *m_pitchRandomnessSlider));

		m_startRandomnessSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_startRandomnessSlider->setName("Start Randomness");
		m_startRandomnessSlider->setRange(0.0, 1.0, 0.001);
		m_startRandomnessSlider->addListener(this);
		m_startRandomnessSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_startRandomnessSlider);
		m_startRandomnessLabel = new Label("lblStartRnd", "random start");
		m_startRandomnessLabel->setJustificationType(Justification::centred);
		addAndMakeVisible(m_startRandomnessLabel);
		m_startRandomnessLabel->attachToComponent(m_startRandomnessSlider, false);
		m_startRandomAttachment.reset(new SliderAttachment(m_valueTreeState, "startRandomness", *m_startRandomnessSlider));

		m_windowBlendSlider = new Slider(Slider::SliderStyle::LinearVertical, Slider::TextEntryBoxPosition::NoTextBox);
		m_windowBlendSlider->setName("Window Blend");
		m_windowBlendSlider->setRange(0.0, 1.0, 0.001);
		m_windowBlendSlider->addListener(this);
		m_windowBlendSlider->setTextBoxIsEditable(true);
		addAndMakeVisible(m_windowBlendSlider);
		m_windowBlendLabel = new Label("lblWindowBlend", "blend");
		m_windowBlendLabel->setJustificationType(Justification::centred);
		addAndMakeVisible(m_windowBlendLabel);
		m_windowBlendLabel->attachToComponent(m_windowBlendSlider, false);
		m_blendAttachment.reset(new SliderAttachment(m_valueTreeState, "windowBlend", *m_windowBlendSlider));


		// Audio Thumbnail
		m_formatManager.registerBasicFormats();
		m_sampleThumbnail.addChangeListener(this);

		// Window shape widget
		m_waveWidget = new WavetableWidget();
		addAndMakeVisible(m_waveWidget);

		m_valueTreeState.state.addListener(this);
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

        //g.setColour (Colours::grey);
        //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        //g.setColour (Colours::white);
        //g.setFont (14.0f);
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
			//auto x = thumb.getRelativePoint(((float)m_startPositionSlider->getValue() * 0.5f), 1.0f).x;
			//g.drawRect(x, thumb.getY() + thumb.getHeight(), 1, thumb.getHeight() - 5, 1);
			//g.drawRect((thumb.getRelativePoint((m_startPositionSlider->getValue() * 0.5f), 1.0f). 
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

		auto sliderBar = bounds.removeFromTop(getHeight() * 0.65f);
		auto sliderBarWidth = getWidth() * 0.75f;
		auto sliderRight = sliderBar.removeFromRight(sliderBarWidth);
		auto labelbar = sliderRight.removeFromTop(sliderRight.getHeight() * 0.15f);

		m_startPositionSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *	0.2f));
		m_grainSizeSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *		0.2f));
		m_mainPitchSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *		0.2f));
		m_pitchRandomnessSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *	0.2f));
		m_startRandomnessSlider->setBounds(sliderRight.removeFromLeft(sliderBarWidth *	0.2f));

		m_startPosLabel->setBounds(labelbar.removeFromLeft(sliderBarWidth * 0.2f));
		m_grainSizeLabel->setBounds(labelbar.removeFromLeft(sliderBarWidth * 0.2f));
		m_mainPitchLabel->setBounds(labelbar.removeFromLeft(sliderBarWidth * 0.2f));
		m_pitchRandomnessLabel->setBounds(labelbar.removeFromLeft(sliderBarWidth * 0.2f));
		m_startRandomnessLabel->setBounds(labelbar.removeFromLeft(sliderBarWidth * 0.2f));

		m_loadSampleButton->setBounds(5, getHeight() * 0.5f, 100, 25);
		m_windowBlendSlider->setBounds(54, 5, 64, 64);
		//m_windowBlendLabel->setBounds(64, 69, 64, 20);
		m_waveWidget->setBounds(5, 5, 64, 64);



    }

	void browseSample()
	{
		if (m_grainProcessor == 0) return;

		m_grainProcessor->setPlayState(false);

		//do loading...

		FileChooser fileSelector("Load sample", File(), "*.wav");

		if (fileSelector.browseForFileToOpen())
		{
			File selectedFile = fileSelector.getResult();
			
			//ValueTree v(Identifier("SampleFile"));
			//v.setProperty(Identifier("Path"), selectedFile.getFullPathName(), nullptr);

			//m_valueTreeState.state.addChild(v, 0, nullptr);
			//
			m_valueTreeState.state.setProperty(Identifier("SampleFile"), selectedFile.getFullPathName(), nullptr);


			m_grainProcessor->setPlayState(true);

			//loadSample(selectedFile);
		}


	}

	void loadSample(File file)
	{
		ScopedPointer<AudioFormatReader> reader = m_formatManager.createReaderFor(file);
		AudioSampleBuffer buffer(reader->numChannels, reader->lengthInSamples);

		reader->read(&buffer, 0, reader->lengthInSamples, 0, true, false);
		m_grainProcessor->setAudioSize(buffer.getNumSamples());
		for (int i = 0; i < buffer.getNumSamples(); ++i)
		{
			m_grainProcessor->setAudioSample(i, buffer.getSample(0, i));
		}

		m_sampleThumbnail.setSource(new FileInputSource(file));

		m_grainProcessor->setPlayState(true);
	}


	void buttonClicked(Button* button) override
	{
		if (button == m_loadSampleButton)
		{
			browseSample();
		}
	}

	void sliderValueChanged(Slider* slider) override
	{
		if (m_grainProcessor == 0) return; 

		else if (slider == m_windowBlendSlider)
		{
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



	void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged,
		const Identifier& property) override
	{

		//DBG(s);
	
		if (property == Identifier("SampleFile"))
			{
				String s = m_valueTreeState.state.getProperty(property);
				File f(s.trim());
				if (f.exists())
				{
					loadSample(f);
				}
				else
				{
					AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "hourglass", ("Could not locate sample " + property.toString()), "OK", this);
				}
			}
		//}
	}

	void valueTreeChildRemoved(ValueTree& parentTree,
		ValueTree& childWhichHasBeenRemoved,
		int indexFromWhichChildWasRemoved) override
	{

	}

	void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved,
		int oldIndex, int newIndex) override
	{

	}

	void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override
	{

	}
	void valueTreeChildAdded(ValueTree& parentTree,
		ValueTree& childWhichHasBeenAdded) override
	{

	}

private:
	JuicyClouds* m_grainProcessor;

	// UI Components
	ScopedPointer<TextButton> m_loadSampleButton;

	ScopedPointer<Slider> m_startPositionSlider;
	std::unique_ptr<SliderAttachment> m_startPositionAttachment;

	ScopedPointer<Slider> m_grainSizeSlider;
	std::unique_ptr<SliderAttachment> m_grainSizeAttachment;

	ScopedPointer<Slider> m_pitchRandomnessSlider;
	std::unique_ptr<SliderAttachment> m_randomPitchAttachment;

	ScopedPointer<Slider> m_startRandomnessSlider;
	std::unique_ptr<SliderAttachment> m_startRandomAttachment;

	ScopedPointer<Slider> m_mainPitchSlider;
	std::unique_ptr<SliderAttachment> m_mainPitchAttachment;

	ScopedPointer<Slider> m_windowBlendSlider;
	std::unique_ptr<SliderAttachment> m_blendAttachment;

	ScopedPointer<Label> m_startPosLabel;
	ScopedPointer<Label> m_grainSizeLabel;
	ScopedPointer<Label> m_pitchRandomnessLabel;
	ScopedPointer<Label> m_startRandomnessLabel;
	ScopedPointer<Label> m_mainPitchLabel;
	ScopedPointer<Label> m_windowBlendLabel;

	AudioFormatManager m_formatManager;

	AudioThumbnailCache m_thumbnailCache;
	AudioThumbnail m_sampleThumbnail;

	ScopedPointer<WavetableWidget> m_waveWidget;

	AudioProcessorValueTreeState& m_valueTreeState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainChannelComponent)
};
