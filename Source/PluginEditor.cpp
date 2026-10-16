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
GateEditor::GateEditor() {
    needsRepaint = false;
}

//==============================================================================
GateEditor::~GateEditor() {
    
}

//==============================================================================
void GateEditor::init(
    int gateNumber,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    initOptionSlider (
        ParameterNames::gateLength[gateNumber], 
        gateLengthSlider, 
        ParameterOptions::lengthOptions,
        16,
        gateLengthSliderAttachment, 
        vts, 
        editor
    );

    initOptionSlider (
        ParameterNames::gateLengthMultiplier[gateNumber], 
        gateLengthMultiplierSlider, 
        ParameterOptions::lengthOptions,
        16,
        gateLengthMultiplierSliderAttachment, 
        vts, 
        editor
    );

    initOptionSlider (
        ParameterNames::gateInterval[gateNumber], 
        gateIntervalSlider, 
        ParameterOptions::lengthOptions,
        16,
        gateIntervalSliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::gatePortion[gateNumber], 
        gatePortionSlider, 
        gatePortionSliderAttachment, 
        vts, 
        editor
    );
}

//==============================================================================
void GateEditor::paint(juce::Graphics& g) {

}

//==============================================================================
void GateEditor::resized(juce::Rectangle<int> bounds) {
    const int columns = 4;
    bounds = bounds.reduced(6);

    float columnWidth = bounds.getWidth() / (float) columns;
    float topRowHeight = bounds.getHeight() * 0.35f;
    float bottomRowHeight = bounds.getHeight() * 0.6f;

    auto gateLengthBounds = bounds.removeFromLeft(columnWidth);
    gateLengthBounds =
        gateLengthBounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    gateLengthSlider.setBounds(gateLengthBounds);

    auto gateLengthMultiplierBounds = bounds.removeFromLeft(columnWidth);
    gateLengthMultiplierBounds =
        gateLengthMultiplierBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    gateLengthMultiplierSlider.setBounds(gateLengthMultiplierBounds);

    auto gateIntervalBounds = bounds.removeFromLeft(columnWidth);
    gateIntervalBounds =
        gateIntervalBounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    gateIntervalSlider.setBounds(gateIntervalBounds);

    auto gatePortionBounds = bounds.removeFromLeft(columnWidth);
    gatePortionBounds =
        gatePortionBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    gatePortionSlider.setBounds(gatePortionBounds);
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

    initSlider (
        ParameterNames::stripRepeatProbability[stripNumber], 
        repeatProbabilitySlider, 
        repeatProbabilitySliderAttachment, 
        vts, 
        editor
    );

    initOptionSlider(
        ParameterNames::stripRepeatLength[stripNumber],
        repeatLengthSlider,
        ParameterOptions::lengthOptions,
        16,
        repeatLengthSliderAttachment,
        vts,
        editor
    );

    initSlider (
        ParameterNames::stripOffset[stripNumber], 
        offsetSlider, 
        offsetSliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::stripRange[stripNumber], 
        rangeSlider, 
        rangeSliderAttachment, 
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

    this->bounds = bounds;
    bounds = bounds.reduced(2);
    bounds = bounds.reduced(4);
    bounds = bounds.reduced(4);

    int rows = 6;
    float rowHeight = bounds.getHeight() / (float) rows;
    float columnWidth = bounds.getWidth() * 0.7f;
    auto rowBounds = bounds;

    // Row 1
    auto enabledGroupBounds = rowBounds.removeFromBottom(rowHeight);

    auto groupBounds = enabledGroupBounds.removeFromLeft(columnWidth);
    groupSlider.setBounds(groupBounds);

    auto enabledBounds = enabledGroupBounds;
    enabledButton.setBounds(enabledBounds);

    // Row 2
    auto offsetRangeBounds = rowBounds.removeFromBottom(rowHeight);

    auto offsetBounds = offsetRangeBounds.removeFromLeft(columnWidth);
    offsetSlider.setBounds(offsetBounds);

    auto rangeBounds = offsetRangeBounds;
    rangeSlider.setBounds(rangeBounds);

    // Row 3
    auto choiceBounds = rowBounds.removeFromBottom(rowHeight);
    choiceSlider.setBounds(choiceBounds);

    // Row 4
    auto repeatBounds = rowBounds.removeFromBottom(rowHeight);

    auto repeatProbabilityBounds = repeatBounds.removeFromLeft(columnWidth);
    repeatProbabilitySlider.setBounds(repeatProbabilityBounds);

    auto repeatLengthBounds = repeatBounds;
    repeatLengthSlider.setBounds(repeatLengthBounds);

    // Row 5
    auto probabilityChokeBounds = rowBounds.removeFromBottom(rowHeight);

    auto probabilityBounds = probabilityChokeBounds.removeFromLeft(columnWidth);
    probabilitySlider.setBounds(probabilityBounds);

    auto chokeBounds = probabilityChokeBounds;
    chokeSlider.setBounds(chokeBounds);

    // Row 6
    // TODO: Put something else here
    auto gateDensityBounds = rowBounds.removeFromBottom(rowHeight);
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
            // TODO:
            /*
                - RepeatProbability
                - RepeatLength
                - Offset
                - Range

                ?????
            */
    }
}

//==============================================================================
GroupEditor::GroupEditor() {
    isOn = false;
    isEnabled = false;
    loop = false;

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
        ParameterNames::groupDivisionLength[groupNumber],
        divisionLengthSlider,
        ParameterOptions::lengthOptions,
        16,
        divisionLengthSliderAttachment,
        vts,
        editor
    );

    initOptionSlider(
        ParameterNames::groupDivisionLengthMultiplier[groupNumber],
        divisionLengthMultiplierSlider,
        ParameterOptions::lengthOptions,
        16,
        divisionLengthMultiplierSliderAttachment,
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
    
    auto numSixteens = numSteps == 0
            ? 1
            : (numSteps + 15) / 16;
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

    auto divisionLengthMultiplierBounds = bounds.removeFromRight(columnWidth);
    divisionLengthMultiplierBounds =
        divisionLengthMultiplierBounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    divisionLengthMultiplierSlider.setBounds(divisionLengthMultiplierBounds);

    auto divisionLengthBounds = bounds.removeFromRight(columnWidth);
    divisionLengthBounds =
        divisionLengthBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    divisionLengthSlider.setBounds(divisionLengthBounds);
}

//==============================================================================
void GroupEditor::loadParameters(Group* group) {
    if (isOn != group->isOn ||
        isEnabled != group->enabled ||
        loop != group->loop ||
        step != group->step ||
        numSteps != group->numSteps) {
            needsRepaint = true;
            isOn = group->isOn;
            isEnabled = group->enabled;
            step = group->step;
            numSteps = group->numSteps;

            divisionLength = group->divisionLength;
            divisionLengthMultiplier = group->divisionLengthMultiplier;
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

    gates = new GateEditor[NUM_GATES];
    for (int i = 0; i < NUM_GATES; i++) {
        gates[i].init(i, vts, *this);
    }

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

    // Do I really need to repaint gates?? 
    for (int i = 0; i < NUM_GATES; i++) {
        gates[i].paint(g);
    }

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

    auto topColumnWidth = topBounds.getWidth() * 0.12f;
    auto globalControlsBounds = topBounds.removeFromRight(topColumnWidth);

    auto gateBounds = topBounds.removeFromRight(topColumnWidth);

    auto gateHeight = gateBounds.getHeight() / NUM_GATES;
    for (int i = 0; i < NUM_GATES; i++) {
        gates[i].resized(gateBounds.removeFromTop(gateHeight));
    }

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

    // Assuming we dont need to load state for Gates

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