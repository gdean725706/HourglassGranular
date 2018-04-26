/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
HourglassGranularAudioProcessorEditor::HourglassGranularAudioProcessorEditor(HourglassGranularAudioProcessor& p, AudioProcessorValueTreeState& vts)
	:
	AudioProcessorEditor(&p),
	processor(p),
	m_grainChannel1(vts),
	m_effectComponent(&p, vts),
	m_valueTreeState(vts)
{
	m_grainChannel1.assignGrainProcessor(processor.getGranularProcessor());
	m_effectComponent.assignGrainProcessor(processor.getGranularProcessor());

	// Use light colour scheme for a change
	if (auto v4 = dynamic_cast<LookAndFeel_V4*> (&getLookAndFeel()))
	{
		v4->setColourScheme(LookAndFeel_V4::getLightColourScheme());
	}
	addAndMakeVisible(m_grainChannel1);

	auto* laf = &getLookAndFeel();
	laf->setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
	laf->setColour(Slider::textBoxTextColourId, getLookAndFeel().findColour(Slider::rotarySliderFillColourId));
	laf->setColour(Label::backgroundWhenEditingColourId, Colours::grey);
	laf->setColour(Label::textColourId, laf->findColour(Slider::rotarySliderFillColourId));

	m_masterTempoSlider = new Slider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxAbove);
	m_masterTempoSlider->setName("Master Tempo");
	m_masterTempoSlider->setRange(60.0, 200.0, 0.5);
	m_masterTempoSlider->setTextBoxIsEditable(true);
	m_masterTempoSlider->addListener(this);
	m_masterTempoSlider->setTextValueSuffix("BPM");
	m_masterTempoSlider->setValue(120.0f, NotificationType::dontSendNotification);
	m_masterTempoSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
	addAndMakeVisible(m_masterTempoSlider);
	m_masterTempoAttachment.reset(new SliderAttachment(m_valueTreeState, "masterTempo", *m_masterTempoSlider));

	m_tempoDivision = new Slider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxAbove);
	m_tempoDivision->setName("Tempo Division");
	m_tempoDivision->setRange(0.25, 8.0, 0.25);
	m_tempoDivision->setTextBoxIsEditable(true);
	m_tempoDivision->addListener(this);
	m_tempoDivision->setTextValueSuffix("DIV");
	m_tempoDivision->setValue(1.0f, NotificationType::dontSendNotification);
	m_tempoDivision->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, 100, 20);
	addAndMakeVisible(m_tempoDivision);
	m_tempoDivisionAttachment.reset(new SliderAttachment(m_valueTreeState, "tempoDivision", *m_tempoDivision));

	addAndMakeVisible(m_effectComponent);

    setSize (600, 350);
	setResizable(true, true);
}

HourglassGranularAudioProcessorEditor::~HourglassGranularAudioProcessorEditor()
{

}

//==============================================================================
void HourglassGranularAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	// Draw header
	g.setColour(getLookAndFeel().findColour(Slider::rotarySliderFillColourId));
	auto font = g.getCurrentFont();
	font.setSizeAndStyle(36.0f, 1, 1.0f, -0.1f);
	g.setFont(font);
	auto bounds = getLocalBounds().removeFromTop(50);
	auto strWidth = font.getStringWidth("hourglass");
	g.drawText("hourglass", bounds.removeFromLeft(strWidth + 10).reduced(5), Justification::topLeft, false);
	font.setSizeAndStyle(18.0f, 0, 1.0f, -0.1f);
	g.setFont(font);
	strWidth = font.getStringWidth("quantised granular engine");
	g.drawText("quantised granular engine", bounds.removeFromLeft(strWidth + 10).reduced(5), Justification::bottomLeft, false);
}

void HourglassGranularAudioProcessorEditor::resized()
{	
	auto windowSize = getLocalBounds();
	auto settingsBar = windowSize.removeFromTop(50);
	m_masterTempoSlider->setBounds(settingsBar.removeFromRight(100));
	m_tempoDivision->setBounds(settingsBar.removeFromRight(100));
	m_grainChannel1.setBounds(windowSize.removeFromTop(200).reduced(5));

	m_effectComponent.setBounds(windowSize.removeFromTop(100).reduced(5));

}


void HourglassGranularAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider == m_masterTempoSlider)
	{
	}
	else if (slider == m_tempoDivision)
	{
	}
}




