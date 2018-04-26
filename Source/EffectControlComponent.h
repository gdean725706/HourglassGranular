/*
  ==============================================================================

    EffectControlComponent.h
    Created: 25 Apr 2018 9:30:26pm
    Author:  George Dean

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class EffectControlComponent    : public Component, public Slider::Listener
{
public:
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

	EffectControlComponent(HourglassGranularAudioProcessor* p, AudioProcessorValueTreeState& vts) :
		m_processor(p),
		m_valueTreeState(vts)
	{

		m_randomPanningSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_randomPanningSlider->setRange(0.0, 1.0, 0.01);
		m_randomPanningSlider->setTextBoxIsEditable(true);
		m_randomPanningSlider->addListener(this);
		addAndMakeVisible(m_randomPanningSlider);
		m_randomPanLabel = new Label("lblRandPan", "random pan");
		m_randomPanLabel->attachToComponent(m_randomPanningSlider, false);
		m_randomPanLabel->setJustificationType(Justification::centred);
		m_randomPanAttachment.reset(new SliderAttachment(m_valueTreeState, "randomPan", *m_randomPanningSlider));

		m_feedbackSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_feedbackSlider->setRange(0.0, 1.0, 0.01);
		m_feedbackSlider->setTextBoxIsEditable(true);
		m_feedbackSlider->addListener(this);
		//addAndMakeVisible(m_feedbackSlider);
		m_feedbackLabel = new Label("lblFeedback", "feedback");
		m_feedbackLabel->attachToComponent(m_feedbackSlider, false);
		m_feedbackLabel->setJustificationType(Justification::centred);

		m_phaserDepthSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_phaserDepthSlider->setRange(0.0, 1.0, 0.001);
		m_phaserDepthSlider->setTextBoxIsEditable(true);
		m_phaserDepthSlider->addListener(this);
		addAndMakeVisible(m_phaserDepthSlider);
		m_phaseDepthLabel = new Label("lblPhaserDepth", "phaser depth");
		m_phaseDepthLabel->attachToComponent(m_phaserDepthSlider, false);
		m_phaseDepthLabel->setJustificationType(Justification::centred);
		m_phaserDepthAttach.reset(new SliderAttachment(m_valueTreeState, "phaserDepth", *m_phaserDepthSlider));

		m_phaseSpeedSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_phaseSpeedSlider->setRange(0.02, 10.0, 0.01);
		m_phaseSpeedSlider->setTextBoxIsEditable(true);
		m_phaseSpeedSlider->addListener(this);
		addAndMakeVisible(m_phaseSpeedSlider);
		m_phaseSpeedLabel = new Label("lblPhaserSpeed", "phaser speed");
		m_phaseSpeedLabel->attachToComponent(m_phaseSpeedSlider, false);
		m_phaseSpeedLabel->setJustificationType(Justification::centred);
		m_phaseSpeedAttach.reset(new SliderAttachment(m_valueTreeState, "phaserSpeed", *m_phaseSpeedSlider));

		m_phaseQSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_phaseQSlider->setRange(0.5, 2.5, 0.01);
		m_phaseQSlider->setTextBoxIsEditable(true);
		m_phaseQSlider->addListener(this);
		addAndMakeVisible(m_phaseQSlider);
		m_phaseQLabel = new Label("lblPhaserQ", "phaser q");
		m_phaseQLabel->attachToComponent(m_phaseQSlider, false);
		m_phaseQLabel->setJustificationType(Justification::centred);
		m_phaseQAttach.reset(new SliderAttachment(m_valueTreeState, "phaserQ", *m_phaseQSlider));

	}

    ~EffectControlComponent()
    {
    }

    void paint (Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background


    }

    void resized() override
    {
		auto bounds = getLocalBounds();
		auto labelBar = bounds.removeFromTop(15);

		m_randomPanningSlider->setBounds(bounds.removeFromLeft(100));
		m_feedbackSlider->setBounds(bounds.removeFromLeft(100));
		m_phaserDepthSlider->setBounds(bounds.removeFromRight(	75));
		m_phaseSpeedSlider->setBounds(bounds.removeFromRight(	75));
		m_phaseQSlider->setBounds(bounds.removeFromRight(		75));

		//m_phaseDepthLabel->setBounds(labelBar.removeFromRight(100));
		//m_phaseSpeedLabel->setBounds(labelBar.removeFromRight(100));
		//m_phaseQLabel->setBounds(labelBar.removeFromRight(100));

		m_randomPanLabel->setBounds(labelBar.removeFromLeft(100));
		m_feedbackLabel->setBounds(labelBar.removeFromLeft(100));
		

    }
	void sliderValueChanged(Slider* slider) override
	{
		if (m_grainProcessor != 0)
		{
			if (slider == m_randomPanningSlider)
			{
			}
			else if (slider == m_feedbackSlider)
			{

			}
			else if (slider == m_phaserDepthSlider)
			{
				m_processor->setAllPassFreq(m_phaserDepthSlider->getValue());
			}
			else if (slider == m_phaseSpeedSlider)
			{
				m_processor->setAllPassQ(m_phaseSpeedSlider->getValue());
			}
		}
	}
	void assignGrainProcessor(JuicyClouds* processor)
	{
		m_grainProcessor = processor;
	}

private:

	ScopedPointer<Slider> m_randomPanningSlider;
	ScopedPointer<Label> m_randomPanLabel;
	ScopedPointer<Slider> m_feedbackSlider;
	ScopedPointer<Label> m_feedbackLabel;

	ScopedPointer<Slider> m_phaserDepthSlider;
	ScopedPointer<Slider> m_phaseSpeedSlider;
	ScopedPointer<Slider> m_phaseQSlider;
	ScopedPointer<Label> m_phaseDepthLabel;
	ScopedPointer<Label> m_phaseSpeedLabel;
	ScopedPointer<Label> m_phaseQLabel;

	HourglassGranularAudioProcessor* m_processor;
	JuicyClouds* m_grainProcessor;

	AudioProcessorValueTreeState& m_valueTreeState;

	std::unique_ptr<SliderAttachment> m_randomPanAttachment;
	std::unique_ptr<SliderAttachment> m_phaseSpeedAttach;
	std::unique_ptr<SliderAttachment> m_phaserDepthAttach;
	std::unique_ptr<SliderAttachment> m_phaseQAttach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectControlComponent)
};
