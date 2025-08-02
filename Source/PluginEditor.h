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
    int progress;
    long plusSixteenProgress;

    bool needsRepaint;
    juce::Rectangle<int> bounds;

    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;

    juce::Slider lengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthSliderAttachment;

    juce::Slider plusSixteenSlider;
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
    bool isBypassed;
    int group;
    int choice;

    bool needsRepaint;
    juce::Rectangle<int> bounds;

    SliceEditor* slices;

    juce::Slider probabilitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilitySliderAttachment;

    juce::Slider groupSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupSliderAttachment;

    juce::Slider choiceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> choiceSliderAttachment;

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
class SequenceEditor {
public:
    SequenceEditor();
    ~SequenceEditor();

    void paint(juce::Graphics&);
    void resized(juce::Rectangle<int>);

private:
    Step *steps;

    bool needsRepaint;
    juce::Rectangle<int> bounds;
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
    int length;
    int plusSixteen;
    int progress;
    long plusSixteenProgress;

    int step;
    int numSteps;
    int steps[MAX_STEPS];

    bool needsRepaint;
    juce::Rectangle<int> bounds;

    juce::Rectangle<int> sequenceBounds;

    juce::Slider lengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthSliderAttachment;

    juce::Slider plusSixteenSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> plusSixteenSliderAttachment;

    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;
};

////==============================================================================
///*
//*/
//class GlobalEditor {
//    GlobalEditor();
//    ~GlobalEditor();
//
//    void init(
//        int,
//        juce::AudioProcessorValueTreeState&,
//        juce::AudioProcessorEditor&
//    );
//
//    void paint(juce::Graphics&);
//    void resized(juce::Rectangle<int>);
//
//    void loadParameters(GlobalDto&);
//
//    int sixteenthCount;
//    int quarterCount;
//    int wholeCount;
//
//    bool needsRepaint;
//    juce::Rectangle<int> bounds;
//};

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

    // GlobalEditor* global;
    GroupEditor* groups;
    StripEditor* strips;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessorEditor)
};
