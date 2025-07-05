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
    auto lineW = 1.2f;
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
    g.strokePath(
        arc,
        juce::PathStrokeType(
            lineW,
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

        if (int(sliderPos) == i) {
            g.setColour(Colours::orange3);
        }
        else if (highlight >= i) {
            g.setColour(Colours::lilac);
        }
        else {
            g.setColour(Colours::transparentOrange3);
        }

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

//==============================================================================
SliceDto::SliceDto() {
    isOn = false;
    isEnabled = false;
    length = 0;
    progress = 0;
}

//==============================================================================
SliceDto::~SliceDto() {

}

//==============================================================================
SliceEditor::SliceEditor() {
    isOn = false;
    isEnabled = false;
    length = 0;
    progress = 0;

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
    const float extraControlsFactor = 0.15f;

    this->bounds = bounds;

    bounds = bounds.reduced(6);

    // auto extraControlsBounds = bounds.removeFromRight(bounds.getWidth() * extraControlsFactor);

    auto enabledBounds = bounds;
    enabledBounds = enabledBounds.removeFromLeft(enabledBounds.getWidth() * enabledFactor);
    enabledBounds = enabledBounds.removeFromBottom(enabledBounds.getHeight() * enabledFactor);
    enabledBounds = enabledBounds.reduced(10);
    enabledButton.setBounds(enabledBounds);

    auto probabilityBounds = bounds;
    probabilityBounds = probabilityBounds.removeFromLeft(probabilityBounds.getWidth() * probabilityFactor);
    probabilityBounds = probabilityBounds.removeFromTop(probabilityBounds.getHeight() * probabilityFactor);

    probabilitySlider.setBounds(probabilityBounds);

    auto plusSixteenbounds = bounds;
    plusSixteenbounds = plusSixteenbounds.removeFromRight(plusSixteenbounds.getWidth() * plusSixteenFactor);
    plusSixteenbounds = plusSixteenbounds.removeFromTop(plusSixteenbounds.getWidth() * plusSixteenFactor);

    // TODO: Set PlusSixteen Bounds

    auto lengthBounds = bounds;
    lengthBounds = lengthBounds.removeFromRight(lengthBounds.getWidth() * lengthFactor);
    lengthBounds = lengthBounds.removeFromBottom(lengthBounds.getHeight() * lengthFactor);

    lengthSlider.setBounds(lengthBounds);
}

//==============================================================================
void SliceEditor::loadParameters(SliceDto& dto) {
    if (isOn != dto.isOn) {
        needsRepaint = true;
        isOn = dto.isOn;
    }
    if (isOn != dto.isEnabled) {
        needsRepaint = true;
        isEnabled = dto.isEnabled;
    }

    if (isOn != dto.length) {
        needsRepaint = true;
        length = dto.length;
    }

    if (isOn != dto.progress) {
        needsRepaint = true;
        progress = dto.progress;
    }
}

//==============================================================================
void SliceEditor::paint(juce::Graphics& g) {
    /*if (!needsRepaint) {
        return;
    }*/

    DrawArc(&probabilitySlider, g);
    DrawLabels(ParameterOptions::lengthOptions, 16, progress, &lengthSlider, g);

    if (isOn) {
        g.setColour(Colours::champagne);
        g.drawRect(bounds);
    }

    needsRepaint = false;
}

//==============================================================================
StripDto::StripDto() {
    isOn = false;
    isEnabled = false;
    isBypassed = false;
    group = 0;

    sliceDtos = new SliceDto[NUM_SLICES];
}

//==============================================================================
StripDto::~StripDto() {

}

//==============================================================================
StripEditor::StripEditor() {
    isOn = false;
    isEnabled = false;
    isBypassed = false;
    group = 0;

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
    const float plusSixteenFactor = 1.0f - probabilityFactor;
    const float extraControlsFactor = 0.15f;

    //this->bounds = bounds;

    // bounds = bounds.reduced(2);

    auto sliceBounds = bounds.removeFromTop(bounds.getHeight() * 0.66f);

    bounds = bounds.reduced(2);
    this->bounds = bounds;
    bounds = bounds.reduced(4);

    // auto extraControlsBounds = bounds.removeFromRight(bounds.getWidth() * extraControlsFactor);

    

    bounds = bounds.reduced(4);

    auto enabledBounds = bounds;
    enabledBounds = enabledBounds.removeFromLeft(enabledBounds.getWidth() * enabledFactor);
    enabledBounds = enabledBounds.removeFromBottom(enabledBounds.getHeight() * enabledFactor);
    enabledBounds = enabledBounds.reduced(10);
    enabledButton.setBounds(enabledBounds);

    auto bypassedBounds = bounds;
    bypassedBounds = bypassedBounds.removeFromLeft(bypassedBounds.getWidth() * enabledFactor);
    bypassedBounds = bypassedBounds.removeFromBottom(bypassedBounds.getHeight() * enabledFactor);
    bypassedBounds = bypassedBounds.reduced(10);
    bypassedBounds.setPosition(bypassedBounds.getX() + enabledBounds.getWidth(), bypassedBounds.getY());
    bypassedButton.setBounds(bypassedBounds);

    auto probabilityBounds = bounds;
    probabilityBounds = probabilityBounds.removeFromLeft(probabilityBounds.getWidth() * probabilityFactor);
    probabilityBounds = probabilityBounds.removeFromTop(probabilityBounds.getHeight() * probabilityFactor);

    probabilitySlider.setBounds(probabilityBounds);

    auto plusSixteenbounds = bounds;
    plusSixteenbounds = plusSixteenbounds.removeFromRight(plusSixteenbounds.getWidth() * plusSixteenFactor);
    plusSixteenbounds = plusSixteenbounds.removeFromTop(plusSixteenbounds.getWidth() * plusSixteenFactor);

    // TODO: Set PlusSixteen Bounds

    auto groupBounds = bounds;
    groupBounds = groupBounds.removeFromRight(groupBounds.getWidth() * groupFactor);
    groupBounds = groupBounds.removeFromBottom(groupBounds.getHeight() * groupFactor);
    // groupBounds = groupBounds.reduced(6);
    groupBounds.setPosition(groupBounds.getX() + 10, groupBounds.getY() - 10);

    groupSlider.setBounds(groupBounds);




    // _________________________________________________

    auto sliceHeight = sliceBounds.getHeight() / NUM_SLICES;
    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].resized(sliceBounds.removeFromTop(sliceHeight));
    }
}

//==============================================================================
void StripEditor::paint(juce::Graphics& g) {
    //// Paint Slices
    //for (int i = 0; i < NUM_SLICES; i++) {
    //    slices[i].paint(g);
    //}

    /*if (!needsRepaint) {
        return;
    }*/

    DrawArc(&probabilitySlider, g);
    DrawLabels(ParameterOptions::groupOptions, 4, -1, &groupSlider, g);

    if (isOn) {
        g.setColour(Colours::champagne);
        
    }
    else {
        g.setColour(Colours::transparentOrange4);
    }

    g.drawRect(bounds, 4);

    // Paint Slices
    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].paint(g);
    }

    needsRepaint = false;
}

//==============================================================================
void StripEditor::loadParameters(StripDto& dto) {
    if (isOn != dto.isOn) {
        needsRepaint = true;
        isOn = dto.isOn;
    }

    if (isEnabled != dto.isEnabled) {
        needsRepaint = true;
        isEnabled = dto.isEnabled;
    }

    if (isBypassed != dto.isBypassed) {
        needsRepaint = true;
        isBypassed = dto.isBypassed;
    }

    if (group != dto.group) {
        needsRepaint = true;
        group = dto.group;
    }

    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].loadParameters(dto.sliceDtos[i]);
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

    auto titleBounds = bounds.removeFromTop(bounds.getHeight() * 0.15f);
    auto groupBounds = bounds.removeFromTop(bounds.getHeight() * 0.4f);
    auto stripBounds = bounds;

    auto stripWidth = stripBounds.getWidth() / NUM_STRIPS;
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].resized(bounds.removeFromLeft(stripWidth));
    }
}

//==============================================================================
void BreaQAudioProcessorEditor::LoadState(GroupDto* groupDtos, StripDto* stripDtos) {
    // TODO: Group Editors
    /*for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].loadParameters(groupDtos[i]);
    }*/

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].loadParameters(stripDtos[i]);
    }

    repaint();
}