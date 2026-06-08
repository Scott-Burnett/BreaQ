#pragma once

#include <../JuceLibraryCode/JuceHeader.h>
#include "PluginProcessor.h"
#include "BreaQLookAndFeel.h"
#include "ParameterNames.h"

//==============================================================================
/*
*/
class StripEditor {
public:
    StripEditor();
    ~StripEditor();
    
    void init (
        int, 
        juce::AudioProcessorValueTreeState&, 
        juce::AudioProcessorEditor&
    );
    void paint(juce::Graphics&);
    void resized(juce::Rectangle<int>);

    void loadParameters(Strip*);

private:
    bool isOn;
    
    bool needsRepaint;
    juce::Rectangle<int> bounds;

    // Parameters
    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;
    
    int group;
    OptionSlider groupSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupSliderAttachment;
    
    int choice;
    OptionSlider choiceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> choiceSliderAttachment;
    
    bool isEnabled;
    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;

    float choke;
    juce::Slider chokeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chokeSliderAttachment;

    float repeatProbability;
    juce::Slider repeatProbabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> repeatProbabilitySliderAttachment;

    int repeatLength;
    OptionSlider repeatLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> repeatLengthSliderAttachment;

    float offset;
    juce::Slider offsetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetSliderAttachment;

    float range;
    juce::Slider rangeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rangeSliderAttachment;
};

//==============================================================================
/*
*/
class GroupEditor {
public:
    GroupEditor();
    ~GroupEditor();

    void init (
        int,
        juce::AudioProcessorValueTreeState&,
        juce::AudioProcessorEditor&
    );

    void paint(juce::Graphics&);
    void resized(juce::Rectangle<int>);

    void loadParameters(Group*);

private:
    bool isOn;   
    int step;
    int numSteps;
    int steps[MAX_STEPS];
    
    // Painting
    bool needsRepaint;
    juce::Rectangle<int> bounds;
    juce::Rectangle<int> sequenceBounds;
    
    // Parameters
    bool isEnabled;
    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;
    
    bool loop;
    juce::ToggleButton loopButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> loopButtonAttachment;

    int tjopLength;
    OptionSlider tjopLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tjopLengthSliderAttachment;

    int tjopLengthMultiplier;
    OptionSlider tjopLengthMultiplierSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tjopLengthMultiplierSliderAttachment;

    int intervalLength;
    OptionSlider intervalLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intervalLengthSliderAttachment;

    int intervalLengthMultiplier;
    OptionSlider intervalLengthMultiplierSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intervalLengthMultiplierSliderAttachment;

    int sequenceLength;
    OptionSlider sequenceLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sequenceLengthSliderAttachment;

    int sequenceLengthMultiplier;
    OptionSlider sequenceLengthMultiplierSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sequenceLengthMultiplierSliderAttachment;

    float density;
    juce::Slider densitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> densitySliderAttachment;
};

//==============================================================================
/*
*/
class BreaQAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer {
public:
    BreaQAudioProcessorEditor(
        BreaQAudioProcessor&, 
        juce::AudioProcessorValueTreeState&
    );
    ~BreaQAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

    void LoadState(Group*, Strip*);

private:
    bool open;
    BreaQAudioProcessor& audioProcessor;
    BreaQLookAndFeel breaQLookAndFeel;

    GroupEditor* groups;
    StripEditor* strips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessorEditor)
};
