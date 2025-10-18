#pragma once

#include <../JuceLibraryCode/JuceHeader.h>
#include "PluginProcessor.h"
#include "BreaQLookAndFeel.h"
#include "ParameterNames.h"

//==============================================================================
/*
*/
class SliceEditor {
public:
    SliceEditor();
    ~SliceEditor();

    void init(
        int, 
        juce::AudioProcessorValueTreeState&, 
        juce::AudioProcessorEditor&
    );
    void paint(juce::Graphics&);
    void resized(juce::Rectangle<int>);
    
    void loadParameters(Slice*);

private:
    bool isOn;
    bool isEnabled;
    int length;
    int plusSixteen;

    bool needsRepaint;
    juce::Rectangle<int> bounds;

    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;

    OptionSlider lengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthSliderAttachment;

    OptionSlider plusSixteenSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> plusSixteenSliderAttachment;

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
    bool isEnabled;
    int group;
    int choice;

    bool needsRepaint;
    juce::Rectangle<int> bounds;

    SliceEditor* slices;

    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;

    OptionSlider groupSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupSliderAttachment;

    OptionSlider choiceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> choiceSliderAttachment;

    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;
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
    bool isEnabled;
    bool loop;
    int length;
    int plusSixteen;

    int step;
    int numSteps;
    int steps[MAX_STEPS];

    bool needsRepaint;
    juce::Rectangle<int> bounds;

    juce::Rectangle<int> sequenceBounds;

    OptionSlider lengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthSliderAttachment;

    OptionSlider plusSixteenSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> plusSixteenSliderAttachment;

    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;

    juce::ToggleButton loopButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> loopButtonAttachment;
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
