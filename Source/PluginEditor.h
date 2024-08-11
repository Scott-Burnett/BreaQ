#pragma once

#include <JuceLibraryCode/JuceHeader.h>
#include "PluginProcessor.h"
#include "BreaQLookAndFeel.h"
#include "SpectrumAnalyzer.h"
#include "ADSRVisualizer.h"

//==============================================================================
/*
*/
static class Colours {
public:
    static const juce::Colour aquamarine;
    static const juce::Colour teaGreen;
    static const juce::Colour springBud;
    static const juce::Colour champagne;
    static const juce::Colour citron;
    static const juce::Colour transparentCitron;
    static const juce::Colour terraCotta;
    static const juce::Colour background1;
    static const juce::Colour background2;
    static const juce::Colour lilac;
    static const juce::Colour transparentLilac;
    static const juce::Colour pink;
    static const juce::Colour transparentPink;
    static const juce::Colour grid1;
    static const juce::Colour grid2;
    static const juce::Colour grid3;
    static const juce::Colour orange2;
    static const juce::Colour orange1;
    static const juce::Colour orange3;
    static const juce::Colour lowPassFillColour;
    static const juce::Colour highPassFillColour;
};

//==============================================================================
/*
*/
class StripDto {
public:
    float probability;
    float group;
    bool enabled;
    bool bypassed;
};

//==============================================================================
/*
*/
class SliceDto {
public:
    float probability;
    int length;
    int progress;
    bool isOn;
    bool enabled;
};

//==============================================================================
/*
*/
class SliceEditor {
public:
    SliceEditor();
    ~SliceEditor();

    void init(int, juce::AudioProcessorValueTreeState&, juce::AudioProcessorEditor&);
    void paint(juce::Graphics&);
    void resized(juce::Rectangle<int>);
    void loadParameters(SliceDto&);

private:
    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;

    juce::Slider lengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthSliderAttachment;

    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;
};

//==============================================================================
/*
*/
class StripEditor {
public:
    StripEditor();
    ~StripEditor();
    
    void init(int, juce::AudioProcessorValueTreeState&, juce::AudioProcessorEditor&);
    void paint(juce::Graphics&);
    void resized(juce::Rectangle<int>);
    void loadParameters(StripDto&);

private:
    const int numSlices = 4;
    SliceEditor* slices;

    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;

    juce::Slider groupSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupSliderAttachment;

    juce::Slider noteSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteSliderAttachment;

    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;

    juce::ToggleButton bypassedButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassedButtonAttachment;
};

//==============================================================================
/*
*/
class BreaQAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    BreaQAudioProcessorEditor(
        BreaQAudioProcessor&, 
        juce::AudioProcessorValueTreeState&, 
        SpectrumAnalyzer&
    );
    ~BreaQAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BreaQAudioProcessor& audioProcessor;
    BreaQLookAndFeel breaQLookAndFeel;

    const int numStrips = 32;
    StripEditor* strips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessorEditor)
};
