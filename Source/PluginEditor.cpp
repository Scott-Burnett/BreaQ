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
static void DrawArc(
    juce::Slider* slider,
    juce::Graphics& g
) {
    const float rotaryStartAngle = slider->getRotaryParameters().startAngleRadians;
    const float rotaryEndAngle = slider->getRotaryParameters().endAngleRadians;

    auto bounds = slider->getBounds();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto lineW = 4.0f;
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path arc;
    arc.addCentredArc(
        bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true
    );

    g.setColour(Colours::transparentOrange3);
    g.strokePath (
        arc,
        juce::PathStrokeType(
            lineW,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    auto ang = juce::jmap((float) slider->getValue(), 
        0.0f, 1.0f, 
        rotaryStartAngle, rotaryEndAngle
    );

    juce::Path filledArc;
    filledArc.addCentredArc(
        bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        ang,
        true
    );

    g.setGradientFill ({
        Colours::pink, bounds.getTopLeft().toFloat(),
        Colours::orange3, bounds.getBottomRight().toFloat(),
        false
    });

    g.strokePath (
        filledArc,
        juce::PathStrokeType(
            1.2f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );
}

//==============================================================================
static void DrawLabels(
    const juce::StringArray labels,
    int numLabels,
    int highlight,
    bool doubleHighlightAtFull,
    juce::Slider *slider,
    juce::Graphics& g
) {
    auto bounds = slider->getBounds();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    float rotaryStartAngle = slider->getRotaryParameters().startAngleRadians;
    float rotaryEndAngle = slider->getRotaryParameters().endAngleRadians;

    float sliderPos = slider->getValue();

    float pos = 0;
    float inc = 1.0f / (float)(numLabels - 1);

    for (int i = 0; i < numLabels; i++, pos += inc) {
        auto ang = juce::jmap(pos, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
        auto c = bounds.getCentre().getPointOnCircumference(radius + 1.0f, ang);

        juce::Rectangle<float> r;
        juce::String str = labels[i];

        r.setSize(g.getCurrentFont().getStringWidthFloat(str), g.getCurrentFont().getStringWidthFloat(str));
        r.setCentre(c);

        if (doubleHighlightAtFull &&
            (int) sliderPos == i &&
            highlight == i) {
            g.setColour(Colours::aquamarine);
        }
        else if ((int)sliderPos == i) {
            g.setColour(Colours::orange3);
        }
        else if (highlight >= i) {
            g.setGradientFill({
                Colours::pink, bounds.getTopLeft().toFloat(),
                Colours::orange3, bounds.getBottomRight().toFloat(),
                false
                });
        }
        else {
            g.setColour(Colours::transparentOrange3);
        }

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

//==============================================================================
SliceEditor::SliceEditor() {
    isOn = false;
    isEnabled = false;
    length = 0;
    plusSixteen = 0;
    progress = -1;
    plusSixteenProgress = -1l;

    needsRepaint = false;
}

//==============================================================================
SliceEditor::~SliceEditor() {
    
}

//==============================================================================
void SliceEditor::init (
    int sliceNumber,
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorEditor& editor
) {
    initSlider (
        ParameterNames::sliceProbability[sliceNumber], 
        probabilitySlider, 
        probabilitySliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::sliceLength[sliceNumber], 
        lengthSlider, 
        lengthSliderAttachment, 
        vts, 
        editor
    );

    initSlider (
        ParameterNames::slicePlusSixteen[sliceNumber], 
        plusSixteenSlider, 
        plusSixteenSliderAttachment, 
        vts, 
        editor
    );

    initButton (
        ParameterNames::sliceEnabled[sliceNumber], 
        enabledButton, 
        enabledButtonAttachment, 
        vts, 
        editor
    );
}

//==============================================================================
void SliceEditor::resized(juce::Rectangle<int> bounds) {
    const float probabilityFactor = 0.6f;
    const float enabledFactor = 0.4f;
    const float lengthFactor = 0.6f;
    const float plusSixteenFactor = 1.0f - probabilityFactor;

    this->bounds = bounds;

    bounds = bounds.reduced(6);

    auto enabledBounds = bounds;
    enabledBounds = 
        enabledBounds
        .removeFromLeft(enabledBounds.getWidth() * enabledFactor)
        .removeFromBottom(enabledBounds.getHeight() * enabledFactor)
        .reduced(10);
    enabledButton.setBounds(enabledBounds);

    auto probabilityBounds = bounds;
    probabilityBounds = 
        probabilityBounds
        .removeFromLeft(probabilityBounds.getWidth() * probabilityFactor)
        .removeFromTop(probabilityBounds.getHeight() * probabilityFactor);
    probabilitySlider.setBounds(probabilityBounds);

    auto plusSixteenbounds = bounds;
    plusSixteenbounds = 
        plusSixteenbounds
        .removeFromRight(plusSixteenbounds.getWidth() * plusSixteenFactor)
        .removeFromTop(plusSixteenbounds.getWidth() * plusSixteenFactor)
        .reduced(4);
    plusSixteenSlider.setBounds(plusSixteenbounds);

    auto lengthBounds = bounds;
    lengthBounds = 
        lengthBounds
        .removeFromRight(lengthBounds.getWidth() * lengthFactor)
        .removeFromBottom(lengthBounds.getHeight() * lengthFactor);

    lengthSlider.setBounds(lengthBounds);
}

//==============================================================================
void SliceEditor::loadParameters(Slice* slice) {
    if (isOn != slice->isOn ||
        isEnabled != slice->enabled ||
        length != slice->length ||
        plusSixteen != slice->plusSixteen ||
        progress != slice->progress ||
        plusSixteenProgress != slice->plusSixteenProgress) {
            needsRepaint = true;
            isOn = slice->isOn;
            isEnabled = slice->enabled;
            length = slice->length;
            plusSixteen = slice->plusSixteen;
            progress = slice->progress;
            plusSixteenProgress = slice->plusSixteenProgress;
    }
}

//==============================================================================
void SliceEditor::paint(juce::Graphics& g) {
    /*if (!needsRepaint) {
        return;
    }*/
    long plusSixteenLength = (long)plusSixteen * 16l;
    bool isPlusSixteen =
        plusSixteen > 0 &&
        plusSixteenProgress < plusSixteenLength;

    int psp =
        isPlusSixteen ||
        plusSixteenProgress == plusSixteenLength
        ? (int) (plusSixteenProgress / 16l) 
        : -1l;

    int p = isPlusSixteen
        ? (int) (plusSixteenProgress % 16l)
        : progress;


    DrawArc(&probabilitySlider, g);
    DrawLabels(ParameterOptions::lengthOptions, 16, p, !isPlusSixteen, &lengthSlider, g);
    DrawLabels(ParameterOptions::plusSixteenOptions, 9, psp, true, &plusSixteenSlider, g);

    if (isOn) {
        g.setColour(Colours::orange3);
        g.drawRect(bounds, 4);
    }

    needsRepaint = false;
}

//==============================================================================
StripEditor::StripEditor() {
    isOn = false;
    isEnabled = false;
    isBypassed = false;
    group = 0;
    choice = 0;

    needsRepaint = false;

    slices = new SliceEditor[NUM_SLICES];
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

    initSlider (
        ParameterNames::stripGroup[stripNumber], 
        groupSlider, 
        groupSliderAttachment, 
        vts, 
        editor
    );

    initSlider(
        ParameterNames::stripChoice[stripNumber],
        choiceSlider,
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

    initButton (
        ParameterNames::stripBypassed[stripNumber], 
        bypassedButton, 
        bypassedButtonAttachment, 
        vts, 
        editor
    );

    for (int i = 0, offset = stripNumber * NUM_SLICES; i < NUM_SLICES; i++) {
        slices[i].init(i + offset, vts, editor);
    }
}

//==============================================================================
void StripEditor::resized (juce::Rectangle<int> bounds) {
    const float probabilityFactor = 0.6f;
    const float enabledFactor = 0.375f;
    const float groupFactor = 0.5f;
    const float choiceFactor = 0.5f;
    const float plusSixteenFactor = 1.0f - probabilityFactor;

    auto sliceBounds = bounds.removeFromTop(bounds.getHeight() * 0.66f);

    this->bounds = bounds;
    bounds = bounds.reduced(2);
    bounds = bounds.reduced(4);
    bounds = bounds.reduced(4);

    auto enabledBounds = bounds;
    enabledBounds = 
        enabledBounds
        .removeFromLeft(enabledBounds.getWidth() * enabledFactor)
        .removeFromBottom(enabledBounds.getHeight() * enabledFactor)
        .reduced(10);
    enabledButton.setBounds(enabledBounds);

    auto bypassedBounds = bounds;
    bypassedBounds = 
        bypassedBounds
        .removeFromLeft(bypassedBounds.getWidth() * enabledFactor)
        .removeFromBottom(bypassedBounds.getHeight() * enabledFactor)
        .reduced(10);
    bypassedBounds.setPosition(bypassedBounds.getX() + enabledBounds.getWidth() - 7, bypassedBounds.getY());
    bypassedButton.setBounds(bypassedBounds);

    auto probabilityBounds = bounds;
    probabilityBounds = 
        probabilityBounds
        .removeFromLeft(probabilityBounds.getWidth() * probabilityFactor)
        .removeFromTop(probabilityBounds.getHeight() * probabilityFactor);
    probabilitySlider.setBounds(probabilityBounds);

    auto groupBounds = bounds;
    groupBounds = 
        groupBounds
        .removeFromRight(groupBounds.getWidth() * groupFactor)
        .removeFromTop(groupBounds.getHeight() * groupFactor)
        .reduced(8);
    groupBounds.setPosition(groupBounds.getX() - 10, groupBounds.getY() - 10);
    groupSlider.setBounds(groupBounds);

    auto choiceBounds = bounds;
    choiceBounds =
        choiceBounds
        .removeFromRight(choiceBounds.getWidth() * choiceFactor)
        .removeFromBottom(choiceBounds.getHeight() * choiceFactor);
    choiceBounds.setPosition(choiceBounds.getX() + 8, choiceBounds.getY() - 10);
    choiceSlider.setBounds(choiceBounds);

    // _________________________________________________

    auto sliceHeight = sliceBounds.getHeight() / NUM_SLICES;
    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].resized(sliceBounds.removeFromTop(sliceHeight));
    }
}

//==============================================================================
void StripEditor::paint(juce::Graphics& g) {
    // Paint Slices
    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].paint(g);
    }

    /*if (!needsRepaint) {
        return;
    }*/

    DrawArc(&probabilitySlider, g);
    DrawLabels(ParameterOptions::groupOptions, NUM_GROUPS, -1, false, &groupSlider, g);
    DrawLabels(ParameterOptions::choiceOptions, NUM_CHOICES, -1, false, &choiceSlider, g);

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

    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].loadParameters(&strip->slices[i]);
    }
}

//==============================================================================
GroupEditor::GroupEditor() {
    isOn = false;
    isEnabled = false;
    length = 0;
    plusSixteen = 0;
    progress = -1;
    plusSixteenProgress = -1l;

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
    initSlider(
        ParameterNames::groupLength[groupNumber],
        lengthSlider,
        lengthSliderAttachment,
        vts,
        editor
    );

    initSlider(
        ParameterNames::groupPlusSixteen[groupNumber],
        plusSixteenSlider,
        plusSixteenSliderAttachment,
        vts,
        editor
    );

    initButton(
        ParameterNames::groupEnabled[groupNumber],
        enabledButton,
        enabledButtonAttachment,
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
    auto blockWidth = this->sequenceBounds.getWidth() / MAX_STEPS;

    auto workingBounds = sequenceBounds;

    /*g.setColour(Colours::transparentOrange3);
    for (int row = 0; row < NUM_STRIPS; row++) {
        auto blockRect = workingBounds.removeFromBottom(blockHeight);
        for (int column = 0; column < 4; column++) {
            auto nextBlock = blockRect.removeFromLeft(blockWidth);

            g.fillRect(nextBlock);
        }

        workingBounds.removeFromLeft(blockWidth * 18);
    }*/

    int row = 0;
    for (int col = 0; col < numSteps; col++) {
        auto nextBlock = workingBounds.removeFromLeft(blockWidth);
        if (steps[col] < 0) {
            continue;
        }

        int h = steps[col];
        if (h > 0) {
            bounds.removeFromBottom(blockHeight * (h - 1));
        }

        if (h < NUM_STRIPS) {
            bounds.removeFromTop(blockHeight * (NUM_STRIPS - h));
        }

        g.fillRect(nextBlock);
    }

    // Draw Sequence
    // for (int i = 0; i < numSteps)


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
    float topRowHeight = bounds.getHeight() * 0.4f;
    float bottomRowHeight = bounds.getHeight() * 0.6f;

    auto enabledBounds = bounds.removeFromRight(columnWidth);
    enabledBounds =
        enabledBounds
        .reduced(5)
    ;
    enabledButton.setBounds(enabledBounds);

    auto plusSixteenbounds = bounds.removeFromRight(columnWidth);
    plusSixteenbounds =
        plusSixteenbounds
        .removeFromTop(topRowHeight)
        .expanded(8)
        .translated(0, +10)
    ;
    plusSixteenSlider.setBounds(plusSixteenbounds);

    auto lengthBounds = bounds.removeFromRight(columnWidth);
    lengthBounds =
        lengthBounds
        .removeFromBottom(bottomRowHeight)
        .expanded(8)
        .translated(0, +2)
    ;
    lengthSlider.setBounds(lengthBounds);
}

//==============================================================================
void GroupEditor::loadParameters(Group* group) {
    if (isOn != group->isOn ||
        isEnabled != group->enabled ||
        length != group->length ||
        plusSixteen != group->plusSixteen ||
        progress != group->progress ||
        plusSixteenProgress != group->plusSixteenProgress ||
        step != group->step ||
        numSteps != group->numSteps) {
            needsRepaint = true;
            isOn = group->isOn;
            isEnabled = group->enabled;
            length = group->length;
            plusSixteen = group->plusSixteen;
            progress = group->progress;
            plusSixteenProgress = group->plusSixteenProgress;
            step = group->step;
            numSteps = group->numSteps;
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
}

//==============================================================================
BreaQAudioProcessorEditor::~BreaQAudioProcessorEditor() {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void BreaQAudioProcessorEditor::paint (juce::Graphics& g) {
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

    auto stripBounds = bounds;

    auto stripWidth = stripBounds.getWidth() / NUM_STRIPS;
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].resized(bounds.removeFromLeft(stripWidth));
    }
}

//==============================================================================
void BreaQAudioProcessorEditor::LoadState(Group* groupProcessors, Strip* stripProcessors) {
    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].loadParameters(&groupProcessors[i]);
    }

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].loadParameters(&stripProcessors[i]);
    }

    repaint();
}