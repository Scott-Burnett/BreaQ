/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BreaQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BreaQAudioProcessorEditor (BreaQAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~BreaQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    BreaQAudioProcessor& audioProcessor;

    juce::Slider crossOverFrequencySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> 
        crossOverFrequencyAttachment;
    juce::Label crossOverFrequencyLabel;

    juce::Slider crossOverWidthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        crossOverWidthAttachment;
    juce::Label crossOverWidthLabel;

    juce::Slider lowPassOrderSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassOrderAttachment;

    juce::Slider highPassOrderSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassOrderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessorEditor)
};
