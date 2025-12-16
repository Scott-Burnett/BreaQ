#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <../JuceLibraryCode/JuceHeader.h>
#include "ParameterNames.h"

//==============================================================================
static void initSlider (
    std::string sliderName,
    juce::Slider& slider,
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& sliderAttachment,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
     editor.addAndMakeVisible(slider);
     slider.setSliderStyle (
         juce::Slider::SliderStyle::RotaryVerticalDrag
     );

     slider.setTextBoxStyle (
         juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
     );

     slider.hideTextBox(false);
     sliderAttachment.reset (
         new juce::AudioProcessorValueTreeState::SliderAttachment (
             vts, sliderName, slider
     ));
}

//==============================================================================
static void initOptionSlider(
    std::string sliderName,
    OptionSlider& slider,
    const juce::StringArray& labels,
    int numLabels,
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& sliderAttachment,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    initSlider(
        sliderName,
        slider,
        sliderAttachment,
        vts,
        editor
    );

    slider.init(labels, numLabels);
}

//==============================================================================
static void initButton(
    std::string buttonName,
    juce::ToggleButton& button,
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& buttonAttachment,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    editor.addAndMakeVisible(button);

    buttonAttachment.reset(
        new juce::AudioProcessorValueTreeState::ButtonAttachment(
            vts, buttonName, button
        ));
}

//==============================================================================
StripEditor::StripEditor() {
    isOn = false;
    isEnabled = false;
    group = 0;
    choice = 0;

    needsRepaint = false;
}

//==============================================================================
StripEditor::~StripEditor() {
    
}

//==============================================================================
void StripEditor::init (
    int stripNumber,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    initSlider (
        ParameterNames::stripProbability[stripNumber], 
        probabilitySlider, 
        probabilitySliderAttachment, 
        vts, 
        editor
    );

    initOptionSlider (
        ParameterNames::stripGroup[stripNumber], 
        groupSlider, 
        ParameterOptions::groupOptions,
        NUM_GROUPS,
        groupSliderAttachment, 
        vts, 
        editor
    );

    initOptionSlider(
        ParameterNames::stripChoice[stripNumber],
        choiceSlider,
        ParameterOptions::choiceOptions,
        NUM_CHOICES,
        choiceSliderAttachment,
        vts,
        editor
    );

    initButton (
        ParameterNames::stripEnabled[stripNumber], 
        enabledButton, 
        enabledButtonAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::stripChoke[stripNumber], 
        chokeSlider, 
        chokeSliderAttachment, 
        vts, 
        editor
    );

    initOptionSlider(
        ParameterNames::stripVariants[stripNumber],
        variantsSlider,
        ParameterOptions::variantsOptions,
        16,
        variantsSliderAttachment,
        vts,
        editor
    );
}

//==============================================================================
void StripEditor::resized (juce::Rectangle<int> bounds) {
    const float probabilityFactor = 0.6f;
    const float enabledFactor = 0.375f;
    const float groupFactor = 0.5f;
    const float choiceFactor = 0.5f;
    const float plusSixteenFactor = 1.0f - probabilityFactor;

    // auto sliceBounds = bounds.removeFromTop(bounds.getHeight() * 0.66f);

    this->bounds = bounds;
    bounds = bounds.reduced(2);
    bounds = bounds.reduced(4);
    bounds = bounds.reduced(4);

    int rows = 6;
    float rowHeight = bounds.getHeight() / (float) rows;
    auto rowBounds = bounds;

    auto enabledBounds = rowBounds.removeFromBottom(rowHeight);
    enabledButton.setBounds(enabledBounds);

    auto probabilityBounds = rowBounds.removeFromBottom(rowHeight);
    probabilitySlider.setBounds(probabilityBounds);

    auto groupBounds = rowBounds.removeFromBottom(rowHeight);;
    groupSlider.setBounds(groupBounds);

    auto choiceBounds = rowBounds.removeFromBottom(rowHeight);
    choiceSlider.setBounds(choiceBounds);

    auto variantsBounds = rowBounds.removeFromBottom(rowHeight);
    variantsSlider.setBounds(variantsBounds);

    auto chokeBounds = rowBounds.removeFromBottom(rowHeight);
    chokeSlider.setBounds(chokeBounds);
}

//==============================================================================
void StripEditor::paint(juce::Graphics& g) {
    /*if (!needsRepaint) {
        return;
    }*/

    g.setColour(Colours::transparentBackground2);
    g.fillRect(bounds);

    if (isOn) {
        g.setColour(Colours::champagne);
    }
    else {
        g.setColour(Colours::transparentOrange4);
    }

    g.drawRect(bounds, 4);

    needsRepaint = false;
}

//==============================================================================
void StripEditor::loadParameters(Strip* strip) {
    if (isOn != strip->isOn ||
        isEnabled != strip->enabled ||
        group != strip->group ||
        choice != strip->choice) {
            needsRepaint = true;
            isOn = strip->isOn;
            isEnabled = strip->enabled;
            group = strip->group;
            choice = strip->choice;
    }
}

//==============================================================================
GroupEditor::GroupEditor() {
    isOn = false;
    isEnabled = false;
    loop = false;
    /*length = 0;
    plusSixteen = 0;*/

    needsRepaint = false;

    step = 0;
    numSteps = 0;
    for (int i = 0; i < MAX_STEPS; i++) {
        steps[i] = -1;
    }
}

//==============================================================================
GroupEditor::~GroupEditor() {

}

//==============================================================================
void GroupEditor::init(
    int groupNumber,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    // initOptionSlider(
    //     ParameterNames::groupLength[groupNumber],
    //     lengthSlider,
    //     ParameterOptions::lengthOptions,
    //     16,
    //     lengthSliderAttachment,
    //     vts,
    //     editor
    // );

    // initOptionSlider(
    //     ParameterNames::groupPlusSixteen[groupNumber],
    //     plusSixteenSlider,
    //     ParameterOptions::plusSixteenOptions,
    //     9,
    //     plusSixteenSliderAttachment,
    //     vts,
    //     editor
    // );

    initButton(
        ParameterNames::groupEnabled[groupNumber],
        enabledButton,
        enabledButtonAttachment,
        vts,
        editor
    );

    initButton(
        ParameterNames::groupLoop[groupNumber],
        loopButton,
        loopButtonAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupTjopLength[groupNumber],
        tjopLengthSlider,
        ParameterOptions::lengthOptions,
        16,
        tjopLengthSliderAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupTjopLengthMultiplier[groupNumber],
        tjopLengthMultiplierSlider,
        ParameterOptions::lengthOptions,
        16,
        tjopLengthMultiplierSliderAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupIntervalLength[groupNumber],
        intervalLengthSlider,
        ParameterOptions::lengthOptions,
        16,
        intervalLengthSliderAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupIntervalLengthMultiplier[groupNumber],
        intervalLengthMultiplierSlider,
        ParameterOptions::lengthOptions,
        16,
        intervalLengthMultiplierSliderAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupSequenceLength[groupNumber],
        sequenceLengthSlider,
        ParameterOptions::lengthOptions,
        16,
        sequenceLengthSliderAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupSequenceLengthMultiplier[groupNumber],
        sequenceLengthMultiplierSlider,
        ParameterOptions::lengthOptions,
        16,
        sequenceLengthMultiplierSliderAttachment,
        vts,
        editor
    );

    initSlider (
        ParameterNames::groupDensity[groupNumber], 
        densitySlider, 
        densitySliderAttachment, 
        vts, 
        editor
    );
}

//==============================================================================
void GroupEditor::paint(juce::Graphics& g) {
    /*if (!needsRepaint) {
        return;
    }*/
    auto blockHeight = this->sequenceBounds.getHeight() / NUM_STRIPS;
    auto bombis = this->sequenceBounds.getWidth();
    // auto blockWidth = this->sequenceBounds.getWidth() / MAX_STEPS;
    
    auto numSixteens = (numSteps / 16);
    auto blockWidth = this->sequenceBounds.getWidth() / (numSixteens * 16);


    auto workingBounds = sequenceBounds;

    g.setColour(Colours::transparentOrange3);

    int row = 0;
    for (int col = 0; col < numSteps; col++) {
        auto nextBlock = workingBounds.removeFromLeft(blockWidth);

        bool isMod4 = col % 4 == 0;

        // Draw the full column
        if (step == col &&
            isEnabled) {
            g.setColour(Colours::transparentOrange3);
        }
        else if (isMod4) {
            g.setColour(Colours::background2);
        }
        /*else if (col % 2) {
            g.setColour(Colours::background1);
        }*/
        else {
            g.setColour(Colours::transparentBackground2);
        }
        g.fillRect(nextBlock);

        // Now Draw the block (if there is one)
        if (steps[col] < 0) {
            continue;
        }

        int h = steps[col];
        nextBlock = nextBlock
            .removeFromBottom(blockHeight * (h + 1))
            .removeFromTop(blockHeight)
        ;

        g.setColour(Colours::transparentOrange3);
        if (step == col) {
            g.setColour(Colours::orange3);
            if (isMod4) {
                g.setColour(Colours::terraCotta);
            }
        }
        g.fillRect(nextBlock);
    }


    g.setColour(Colours::background2);
    g.fillRect(workingBounds);

    g.setColour(Colours::transparentBackground2);
    g.fillRect(bounds);

    g.setColour(Colours::transparentOrange4);
    g.drawRect(bounds);

    needsRepaint = false;
}

//==============================================================================
void GroupEditor::resized(juce::Rectangle<int> bounds) {
    this->sequenceBounds = bounds.removeFromLeft(bounds.getWidth() * 0.75f);
    this->bounds = bounds;

    // const float probabilityFactor = 0.6f;
    const float enabledFactor = 0.1f;
    const float lengthFactor = 0.6f;
    const float plusSixteenFactor = 0.4f;

    const int columns = 8;
    bounds = bounds.reduced(6);

    float columnWidth = bounds.getWidth() / (float) columns;
    float topRowHeight = bounds.getHeight() * 0.35f;
    float bottomRowHeight = bounds.getHeight() * 0.6f;

    auto buttonBounds = bounds.removeFromRight(columnWidth);
    auto enabledBounds =
        buttonBounds.removeFromTop(buttonBounds.getHeight() * 0.5f)
        .reduced(10)
    ;
    enabledButton.setBounds(enabledBounds);

    auto loopBounds =
        buttonBounds
        .reduced(10)
        ;
    loopButton.setBounds(loopBounds);

    auto densityBounds = bounds.removeFromRight(columnWidth);
    densityBounds =
        densityBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    densitySlider.setBounds(densityBounds);

    auto sequenceLengthMultiplierBounds = bounds.removeFromRight(columnWidth);
    sequenceLengthMultiplierBounds =
        sequenceLengthMultiplierBounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    sequenceLengthMultiplierSlider.setBounds(sequenceLengthMultiplierBounds);

    auto sequenceLengthBounds = bounds.removeFromRight(columnWidth);
    sequenceLengthBounds =
        sequenceLengthBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    sequenceLengthSlider.setBounds(sequenceLengthBounds);

    auto intervalLengthMultiplierBounds = bounds.removeFromRight(columnWidth);
    intervalLengthMultiplierBounds =
        intervalLengthMultiplierBounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    intervalLengthMultiplierSlider.setBounds(intervalLengthMultiplierBounds);

    auto intervalLengthBounds = bounds.removeFromRight(columnWidth);
    intervalLengthBounds =
        intervalLengthBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    intervalLengthSlider.setBounds(intervalLengthBounds);

    auto tjopLengthMultiplierBounds = bounds.removeFromRight(columnWidth);
    tjopLengthMultiplierBounds =
        tjopLengthMultiplierBounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    tjopLengthMultiplierSlider.setBounds(tjopLengthMultiplierBounds);

    auto tjopLengthBounds = bounds.removeFromRight(columnWidth);
    tjopLengthBounds =
        tjopLengthBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    tjopLengthSlider.setBounds(tjopLengthBounds);
}

//==============================================================================
void GroupEditor::loadParameters(Group* group) {
    if (isOn != group->isOn ||
        isEnabled != group->enabled ||
        loop != group->loop ||
        // length != group->length ||
        // plusSixteen != group->plusSixteen ||
        step != group->step ||
        numSteps != group->numSteps) {
            needsRepaint = true;
            isOn = group->isOn;
            isEnabled = group->enabled;
            // length = group->length;
            // plusSixteen = group->plusSixteen;
            step = group->step;
            numSteps = group->numSteps;

            tjopLength = group->tjopLength;
            tjopLengthMultiplier = group->tjopLengthMultiplier;
            intervalLength = group->intervalLength;
            intervalLengthMultiplier = group->intervalLengthMultiplier;
            sequenceLength = group->sequenceLength;
            sequenceLengthMultiplier = group->sequenceLengthMultiplier;

            for (int i = 0; i < MAX_STEPS; i++) {
                steps[i] = group->sequence[i].hasValue
                    ? group->sequence[i].strip->stripId
                    : -1;
            }
    }
}

//==============================================================================
BreaQAudioProcessorEditor::BreaQAudioProcessorEditor (
    BreaQAudioProcessor& p, 
    juce::AudioProcessorValueTreeState& vts
) : 
    AudioProcessorEditor (&p), 
    audioProcessor (p) 
{
    juce::LookAndFeel::setDefaultLookAndFeel(&breaQLookAndFeel);

    groups = new GroupEditor[NUM_GROUPS];
    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].init(i, vts, *this);
    }

    strips = new StripEditor[NUM_STRIPS];
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].init(i, vts, *this);
    }

    setSize(2000, 1000);
    startTimer(30);
    open = true;
}

//==============================================================================
BreaQAudioProcessorEditor::~BreaQAudioProcessorEditor() {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    open = false;
}

//==============================================================================
void BreaQAudioProcessorEditor::paint (juce::Graphics& g) {
    /*if (!needsRepaint) {
        return;
    }*/
    g.fillAll(Colours::background1);

    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].paint(g);
    }

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].paint(g);
    }
}

//==============================================================================
void BreaQAudioProcessorEditor::timerCallback() {
    // repaint();
}

//==============================================================================
void BreaQAudioProcessorEditor::resized () {
    auto bounds = getLocalBounds();

    const int mainMargin = 20;
    bounds.removeFromTop(mainMargin);
    bounds.removeFromRight(mainMargin);
    bounds.removeFromBottom(mainMargin);
    bounds.removeFromLeft(mainMargin);

    auto titleBounds = bounds.removeFromTop(bounds.getHeight() * 0.05f);
    auto topBounds = bounds.removeFromTop(bounds.getHeight() * 0.5f);
    auto globalControlsBounds = topBounds.removeFromRight(topBounds.getWidth() * 0.12f);

    auto groupBounds = topBounds;

    auto groupHeight = groupBounds.getHeight() / NUM_GROUPS;
    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].resized(groupBounds.removeFromTop(groupHeight));
    }

    bounds.removeFromTop(mainMargin);

    auto stripBounds = bounds;

    auto stripWidth = stripBounds.getWidth() / NUM_STRIPS;
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].resized(bounds.removeFromLeft(stripWidth));
    }
}

//==============================================================================
void BreaQAudioProcessorEditor::LoadState(Group* groupProcessors, Strip* stripProcessors) {
    if (!open) {
        return;
    }
    for (int i = 0; i < NUM_GROUPS; i++) {
        if (&groups[i] == nullptr) {
            continue;
        }
        groups[i].loadParameters(&groupProcessors[i]);
    }

    for (int i = 0; i < NUM_STRIPS; i++) {
        if (&strips[i] == nullptr) {
            continue;
        }
        strips[i].loadParameters(&stripProcessors[i]);
    }

    repaint();
}