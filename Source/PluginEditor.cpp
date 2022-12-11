/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
/*void BreaQAudioProcessorEditor::initializeSlider(
        juce::AudioProcessorValueTreeState& vts,
        juce::Slider slider,
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment,
        const juce::String& parameterName) {
    addAndMakeVisible(crossOverFrequencySlider);
    crossOverFrequencySlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    crossOverFrequencySlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    crossOverFrequencySlider.hideTextBox(false);
    crossOverFrequencyAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "crossOverFrequency", crossOverFrequencySlider
        )
    );
}*/

//==============================================================================
BreaQAudioProcessorEditor::BreaQAudioProcessorEditor (
        BreaQAudioProcessor& p, 
        juce::AudioProcessorValueTreeState& vts
    ) : 
        AudioProcessorEditor (&p), 
        audioProcessor (p) {
    // Cross over Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    addAndMakeVisible(crossOverFrequencySlider);
    crossOverFrequencySlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    crossOverFrequencySlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    crossOverFrequencySlider.hideTextBox(false);
    crossOverFrequencyAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "crossOverFrequency", crossOverFrequencySlider
        )
    );

    addAndMakeVisible(crossOverFrequencyLabel);
    crossOverFrequencyLabel.setText(
        "Cross Over Frequency", juce::dontSendNotification
    );
    
    addAndMakeVisible(crossOverWidthSlider);
    crossOverWidthSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    crossOverWidthSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    crossOverWidthSlider.hideTextBox(false);
    crossOverWidthAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "crossOverWidth", crossOverWidthSlider
        )
    );

    addAndMakeVisible(crossOverWidthLabel);
    crossOverWidthLabel.setText(
        "Cross Over Width", juce::dontSendNotification
    );

    addAndMakeVisible(lowPassOrderSlider);
    lowPassOrderSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassOrderSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassOrderSlider.hideTextBox(false);
    lowPassOrderAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassRollOff", lowPassOrderSlider
        )
    );

    addAndMakeVisible(highPassOrderSlider);
    highPassOrderSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassOrderSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassOrderSlider.hideTextBox(false);
    highPassOrderAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassRollOff", highPassOrderSlider
        )
    );

    // Low Pass Envelope Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    addAndMakeVisible(lowPassInitialSlider);
    lowPassInitialSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassInitialSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassInitialSlider.hideTextBox(false);
    lowPassInitialAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassInitial", lowPassInitialSlider
        )
    );

    addAndMakeVisible(lowPassPeakSlider);
    lowPassPeakSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassPeakSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassPeakSlider.hideTextBox(false);
    lowPassPeakAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassPeak", lowPassPeakSlider
        )
    );

    addAndMakeVisible(lowPassAttackSlider);
    lowPassAttackSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassAttackSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassAttackSlider.hideTextBox(false);
    lowPassAttackAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassAttack", lowPassAttackSlider
        )
    );

    addAndMakeVisible(lowPassAttackCurveSlider);
    lowPassAttackCurveSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassAttackCurveSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassAttackCurveSlider.hideTextBox(false);
    lowPassAttackCurveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassAttackCurve", lowPassAttackCurveSlider
        )
    );

    addAndMakeVisible(lowPassDecaySlider);
    lowPassDecaySlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassDecaySlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassDecaySlider.hideTextBox(false);
    lowPassDecayAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassDecay", lowPassDecaySlider
        )
    );

    addAndMakeVisible(lowPassDecayCurveSlider);
    lowPassDecayCurveSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassDecayCurveSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassDecayCurveSlider.hideTextBox(false);
    lowPassDecayCurveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassDecayCurve", lowPassDecayCurveSlider
        )
    );

    addAndMakeVisible(lowPassSustainSlider);
    lowPassSustainSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassSustainSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassSustainSlider.hideTextBox(false);
    lowPassSustainAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassSustain", lowPassSustainSlider
        )
    );

    addAndMakeVisible(lowPassReleaseSlider);
    lowPassReleaseSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassReleaseSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassReleaseSlider.hideTextBox(false);
    lowPassReleaseAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassRelease", lowPassReleaseSlider
        )
    );

    addAndMakeVisible(lowPassReleaseCurveSlider);
    lowPassReleaseCurveSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    lowPassReleaseCurveSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    lowPassReleaseCurveSlider.hideTextBox(false);
    lowPassReleaseCurveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "lowPassReleaseCurve", lowPassReleaseCurveSlider
        )
    );

    // High Pass Envelope Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    addAndMakeVisible(highPassInitialSlider);
    highPassInitialSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassInitialSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassInitialSlider.hideTextBox(false);
    highPassInitialAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassInitial", highPassInitialSlider
        )
    );

    addAndMakeVisible(highPassPeakSlider);
    highPassPeakSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassPeakSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassPeakSlider.hideTextBox(false);
    highPassPeakAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassPeak", highPassPeakSlider
        )
    );

    addAndMakeVisible(highPassAttackSlider);
    highPassAttackSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassAttackSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassAttackSlider.hideTextBox(false);
    highPassAttackAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassAttack", highPassAttackSlider
        )
    );

    addAndMakeVisible(highPassAttackCurveSlider);
    highPassAttackCurveSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassAttackCurveSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassAttackCurveSlider.hideTextBox(false);
    highPassAttackCurveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassAttackCurve", highPassAttackCurveSlider
        )
    );

    addAndMakeVisible(highPassDecaySlider);
    highPassDecaySlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassDecaySlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassDecaySlider.hideTextBox(false);
    highPassDecayAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassDecay", highPassDecaySlider
        )
    );

    addAndMakeVisible(highPassDecayCurveSlider);
    highPassDecayCurveSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassDecayCurveSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassDecayCurveSlider.hideTextBox(false);
    highPassDecayCurveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassDecayCurve", highPassDecayCurveSlider
        )
    );

    addAndMakeVisible(highPassSustainSlider);
    highPassSustainSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassSustainSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassSustainSlider.hideTextBox(false);
    highPassSustainAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassSustain", highPassSustainSlider
        )
    );

    addAndMakeVisible(highPassReleaseSlider);
    highPassReleaseSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassReleaseSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassReleaseSlider.hideTextBox(false);
    highPassReleaseAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassRelease", highPassReleaseSlider
        )
    );

    addAndMakeVisible(highPassReleaseCurveSlider);
    highPassReleaseCurveSlider.setSliderStyle(
        juce::Slider::SliderStyle::RotaryVerticalDrag
    );
    highPassReleaseCurveSlider.setTextBoxStyle(
        juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0
    );
    highPassReleaseCurveSlider.hideTextBox(false);
    highPassReleaseCurveAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            vts, "highPassReleaseCurve", highPassReleaseCurveSlider
        )
    );

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    setSize (1200, 600);
    juce::LookAndFeel::setDefaultLookAndFeel(&breaQLookAndFeel);
}

BreaQAudioProcessorEditor::~BreaQAudioProcessorEditor() {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void BreaQAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll(juce::Colours::darkturquoise);
    g.setColour (juce::Colours::bisque);
    g.setFont (15.0f);
}

void BreaQAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();

    auto mainMargin = bounds.getWidth() / 30;
    bounds.removeFromTop(mainMargin);
    bounds.removeFromRight(mainMargin);
    bounds.removeFromBottom(mainMargin);
    bounds.removeFromLeft(mainMargin);

    auto topPanel = bounds.removeFromTop(bounds.getHeight() * 0.45f);

    auto envelopePanelWidth = bounds.getWidth() * 0.42f;

    auto leftPanelBounds = bounds.removeFromLeft(envelopePanelWidth);
    auto rightPanelBounds = bounds.removeFromRight(envelopePanelWidth);
    
    // Centre Panel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    auto peripheralControlsWidth = bounds.getWidth() * 0.3f;
    auto leftPeripheralControlsPanel = bounds.removeFromLeft(peripheralControlsWidth).expanded(3);
    auto rightPeripheralControlsPanel = bounds.removeFromRight(peripheralControlsWidth).expanded(3);

    lowPassOrderSlider.setBounds(leftPeripheralControlsPanel.removeFromTop(peripheralControlsWidth));
    highPassOrderSlider.setBounds(rightPeripheralControlsPanel.removeFromTop(peripheralControlsWidth));

    auto crossOverFrequencyControlBounds = 
        bounds.removeFromTop(bounds.getHeight() * 0.5f);
    auto crossOverFrequencyLabelBounds = 
        crossOverFrequencyControlBounds.removeFromBottom(bounds.getHeight() * 0.2f);
    crossOverFrequencySlider.setBounds(crossOverFrequencyControlBounds);
    crossOverFrequencyLabel.setBounds(crossOverFrequencyLabelBounds);

    auto crossOverWidthControlBounds = bounds;
    auto crossOverWidthLabelBounds =
        crossOverWidthControlBounds.removeFromBottom(bounds.getHeight() * 0.2f);
    crossOverWidthSlider.setBounds(crossOverWidthControlBounds);
    crossOverWidthLabel.setBounds(crossOverWidthLabelBounds);

    // Left Panel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    auto leftEnvelopeVisualizerBounds = leftPanelBounds.removeFromTop(leftPanelBounds.getHeight() * 0.4f);

    auto controlHeight = leftPanelBounds.getHeight() / 3;
    int controlWidth = leftPanelBounds.getWidth() / 6;
    const auto controlExpansion = 3;

    auto leftEnvelopeControlsBounds = leftPanelBounds.removeFromTop(controlHeight);
    auto leftInitialBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftPeakBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftSustainBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);

    leftEnvelopeControlsBounds = leftPanelBounds.removeFromTop(controlHeight);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftAttackBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftDecayBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftReleaseBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);

    leftEnvelopeControlsBounds = leftPanelBounds.removeFromTop(controlHeight);
    auto leftAttackCurveBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftDecayCurveBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftReleaseCurveBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    
    lowPassInitialSlider.setBounds(leftInitialBounds);
    lowPassPeakSlider.setBounds(leftPeakBounds);
    lowPassSustainSlider.setBounds(leftSustainBounds);

    lowPassAttackSlider.setBounds(leftAttackBounds);
    lowPassDecaySlider.setBounds(leftDecayBounds);
    lowPassReleaseSlider.setBounds(leftReleaseBounds);

    lowPassAttackCurveSlider.setBounds(leftAttackCurveBounds);
    lowPassDecayCurveSlider.setBounds(leftDecayCurveBounds);
    lowPassReleaseCurveSlider.setBounds(leftReleaseCurveBounds);

    // Right Panel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    auto rightEnvelopeVisualizerBounds = rightPanelBounds.removeFromTop(rightPanelBounds.getHeight() * 0.4f);

    /*

    auto rightEnvelopeControlsBounds = rightPanelBounds.removeFromTop(rightPanelBounds.getHeight() * 0.5f);
    int rightEnvelopeControlsWidth = rightPanelBounds.getWidth() / 4;

    auto rightAttackBounds = rightEnvelopeControlsBounds.removeFromLeft(rightEnvelopeControlsWidth);
    auto rightDecayBounds = rightEnvelopeControlsBounds.removeFromLeft(rightEnvelopeControlsWidth);
    auto rightSustainBounds = rightEnvelopeControlsBounds.removeFromLeft(rightEnvelopeControlsWidth);
    auto rightReleaseBounds = rightEnvelopeControlsBounds.removeFromLeft(rightEnvelopeControlsWidth);

    highPassAttackSlider.setBounds(rightAttackBounds);
    highPassDecaySlider.setBounds(rightDecayBounds);
    highPassSustainSlider.setBounds(rightSustainBounds);
    highPassReleaseSlider.setBounds(rightReleaseBounds);

    auto rightAttackCurveBounds = rightPanelBounds.removeFromLeft(rightEnvelopeControlsWidth);
    auto rightDecayCurveBounds = rightPanelBounds.removeFromLeft(rightEnvelopeControlsWidth);
    rightPanelBounds.removeFromLeft(rightEnvelopeControlsWidth); // Empty Space
    auto rightReleaseCurveBounds = rightPanelBounds.removeFromLeft(rightEnvelopeControlsWidth);

    highPassAttackCurveSlider.setBounds(rightAttackCurveBounds);
    highPassDecayCurveSlider.setBounds(rightDecayCurveBounds);
    highPassReleaseCurveSlider.setBounds(rightReleaseCurveBounds);*/

    auto rightEnvelopeControlsBounds = rightPanelBounds.removeFromTop(controlHeight);
    auto rightInitialBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightPeakBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightSustainBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);

    rightEnvelopeControlsBounds = rightPanelBounds.removeFromTop(controlHeight);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightAttackBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightDecayBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightReleaseBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);

    rightEnvelopeControlsBounds = rightPanelBounds.removeFromTop(controlHeight);
    auto rightAttackCurveBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightDecayCurveBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightReleaseCurveBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);

    highPassInitialSlider.setBounds(rightInitialBounds);
    highPassPeakSlider.setBounds(rightPeakBounds);
    highPassSustainSlider.setBounds(rightSustainBounds);

    highPassAttackSlider.setBounds(rightAttackBounds);
    highPassDecaySlider.setBounds(rightDecayBounds);
    highPassReleaseSlider.setBounds(rightReleaseBounds);

    highPassAttackCurveSlider.setBounds(rightAttackCurveBounds);
    highPassDecayCurveSlider.setBounds(rightDecayCurveBounds);
    highPassReleaseCurveSlider.setBounds(rightReleaseCurveBounds);
}