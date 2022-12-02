/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BreaQAudioProcessorEditor::BreaQAudioProcessorEditor (
        BreaQAudioProcessor& p, 
        juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(crossOverFrequencySlider);
    crossOverFrequencySlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag);
    crossOverFrequencyAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "crossOverFrequency", crossOverFrequencySlider
        )
    );

    addAndMakeVisible(crossOverFrequencyLabel);
    crossOverFrequencyLabel.setText(
        "Cross Over Frequency", juce::dontSendNotification
    );
    
    addAndMakeVisible(crossOverWidthSlider);
    crossOverWidthSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag);
    crossOverWidthAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "crossOverWidth", crossOverWidthSlider
        )
    );

    addAndMakeVisible(crossOverWidthLabel);
    crossOverWidthLabel.setText(
        "Cross Over Width", juce::dontSendNotification
    );
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
}

BreaQAudioProcessorEditor::~BreaQAudioProcessorEditor()
{
}

//==============================================================================
void BreaQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void BreaQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
