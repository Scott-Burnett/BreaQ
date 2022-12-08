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
        juce::AudioProcessorValueTreeState& vts
    ) : 
        AudioProcessorEditor (&p), 
        audioProcessor (p) {
    addAndMakeVisible(crossOverFrequencySlider);
    crossOverFrequencySlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    crossOverFrequencySlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    crossOverFrequencySlider.hideTextBox(false);
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
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    crossOverWidthSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    crossOverWidthSlider.hideTextBox(false);
    crossOverWidthAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "crossOverWidth", crossOverWidthSlider
        )
    );

    addAndMakeVisible(crossOverWidthLabel);
    crossOverWidthLabel.setText(
        "Cross Over Width", juce::dontSendNotification
    );

    addAndMakeVisible(lowPassOrderSlider);
    lowPassOrderSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassOrderSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassOrderSlider.hideTextBox(false);
    lowPassOrderAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassRollOff", lowPassOrderSlider
        )
    );

    addAndMakeVisible(highPassOrderSlider);
    highPassOrderSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassOrderSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassOrderSlider.hideTextBox(false);
    highPassOrderAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassRollOff", highPassOrderSlider
        )
    );

    setSize (800, 600);
    juce::LookAndFeel::setDefaultLookAndFeel(&breaQLookAndFeel);
}

BreaQAudioProcessorEditor::~BreaQAudioProcessorEditor() {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void BreaQAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void BreaQAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    auto topPanel = bounds.removeFromTop(bounds.getHeight() * 0.33f);

    auto leftPanelBounds = bounds.removeFromLeft(bounds.getWidth() * 0.33f);
    auto rightPanelBounds = bounds.removeFromRight(bounds.getWidth() * 0.5f);
    
    auto peripheralControlsWidth = bounds.getWidth() * 0.3f;
    auto leftPeripheralControlsPanel = bounds.removeFromLeft(peripheralControlsWidth);
    auto rightPeripheralControlsPanel = bounds.removeFromRight(peripheralControlsWidth);

    lowPassOrderSlider.setBounds(leftPeripheralControlsPanel.removeFromTop(peripheralControlsWidth));
    highPassOrderSlider.setBounds(rightPeripheralControlsPanel.removeFromTop(peripheralControlsWidth));

    auto crossOverFrequencyControlBounds = 
        bounds.removeFromTop(bounds.getHeight() * 0.5f);
    auto crossOverFrequencyLabelBounds = 
        crossOverFrequencyControlBounds.removeFromBottom(bounds.getHeight() * 0.2f);
    crossOverFrequencySlider.setBounds(crossOverFrequencyControlBounds);
    crossOverFrequencyLabel.setBounds(crossOverFrequencyLabelBounds);

    auto crossOverWidthControlBounds = bounds;
    auto crossOverWidthLabelBounds =
        crossOverWidthControlBounds.removeFromBottom(bounds.getHeight() * 0.2f);
    crossOverWidthSlider.setBounds(crossOverWidthControlBounds);
    crossOverWidthLabel.setBounds(crossOverWidthLabelBounds); 
}
