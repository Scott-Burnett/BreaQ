#pragma once

#include <../JuceLibraryCode/JuceHeader.h>
#include "PluginProcessor.h"
#include "BreaQLookAndFeel.h"
#include "ParameterNames.h"

//==============================================================================
/* Remove Completely
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
    
    bool needsRepaint;
    juce::Rectangle<int> bounds;
    
    SliceEditor* slices;
    
    // Remove
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

    // New Stuff
    // Choke
    // Variations here?

    float choke;
    juce::Slider chokeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chokeSliderAttachment;

    int variant;
    OptionSlider variantsSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> variantsSliderAttachment;
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
    // Remove
    int length;
    OptionSlider lengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lengthSliderAttachment;
    
    // Remove
    int plusSixteen;
    OptionSlider plusSixteenSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> plusSixteenSliderAttachment;
    
    bool isEnabled;
    juce::ToggleButton enabledButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledButtonAttachment;
    
    bool loop;
    juce::ToggleButton loopButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> loopButtonAttachment;

    // new stuff:
    // :: TJOP
    /*
        - length (1 -16)
        - Multiplier (1 - 16) ??
    */
    int tjopLength;
    OptionSlider tjopLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tjopLengthSliderAttachment;

    int tjopLengthMultiplier;
    OptionSlider tjopLengthMultiplierSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tjopLengthMultiplierSliderAttachment;

    // :: INTERVAL ?? (Bad name)
    /*
        - length (1 -16)
        - Multiplier (1 - 16) ??
    */
    int intervalLength;
    OptionSlider intervalLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intervalLengthSliderAttachment;

    int intervalLengthMultiplier;
    OptionSlider intervalLengthMultiplierSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intervalLengthMultiplierSliderAttachment;

    // :: Global
    /*
        - length (1 -16)
        - Multiplier (1 - 16) ??

        - Density

        - Loop

        - enabled
    */
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
