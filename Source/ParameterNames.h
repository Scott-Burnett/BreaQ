#pragma once

#include <../JuceLibraryCode/JuceHeader.h>
// Numbers of Things
#define NUM_GROUPS 4
#define NUM_STRIPS 8
#define NUM_SLICES 2
#define NUM_CHOICES 8
#define MAX_STEPS 256
// Note Control Values
#define NOTE_TAKE_STEP 60
#define NOTE_SCHEDULE_RESET 61

//==============================================================================
/*
*/
static class ParameterNames {
public:
    // Slices
    const static std::string sliceProbability[128];
    const static std::string sliceLength[128];
    const static std::string sliceEnabled[128];
    const static std::string slicePlusSixteen[128];
    // Strips
    const static std::string stripProbability[32];
    const static std::string stripGroup[32];
    const static std::string stripChoice[32];
    const static std::string stripNote[32];
    const static std::string stripEnabled[32];
    const static std::string stripBypassed[32];
    const static std::string stripChoke[32];
    const static std::string stripVariants[32];
    // Groups
    const static std::string groupEnabled[NUM_GROUPS];
    const static std::string groupLoop[NUM_GROUPS];
    const static std::string groupLength[NUM_GROUPS];
    const static std::string groupPlusSixteen[NUM_GROUPS];
    const static std::string groupTjopLength[NUM_GROUPS];
    const static std::string groupTjopLengthMultiplier[NUM_GROUPS];
    const static std::string groupIntervalLength[NUM_GROUPS];
    const static std::string groupIntervalLengthMultiplier[NUM_GROUPS];
    const static std::string groupSequenceLength[NUM_GROUPS];
    const static std::string groupSequenceLengthMultiplier[NUM_GROUPS];
    const static std::string groupDensity[NUM_GROUPS];
};

//==============================================================================
/*
*/
static class ParameterOptions {
public:
    const static juce::StringArray toggleButtonOptions;
    const static juce::StringArray groupOptions;
    const static juce::StringArray choiceOptions;
    const static juce::StringArray lengthOptions;
    const static juce::StringArray plusSixteenOptions;
    const static juce::StringArray variantsOptions;
};

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
    static const juce::Colour transparentBackground2;
    static const juce::Colour transparentOrange3;
    static const juce::Colour transparentOrange4;
    static const juce::Colour transparentOrange1;
};