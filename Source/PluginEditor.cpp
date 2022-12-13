/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

//==============================================================================
BreaQAudioProcessorEditor::BreaQAudioProcessorEditor (
        BreaQAudioProcessor& p, 
        juce::AudioProcessorValueTreeState& vts,
        SpectrumAnalyzer& analyzer
    ) : 
        AudioProcessorEditor (&p), 
        audioProcessor (p) {
    spectrumAnalyzer = &analyzer;
    addAndMakeVisible(spectrumAnalyzer);
    
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

    /*addAndMakeVisible(crossOverFrequencyLabel);
    crossOverFrequencyLabel.setText(
        "Cross Over Frequency", juce::dontSendNotification
    );*/
    
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

    /*addAndMakeVisible(crossOverWidthLabel);
    crossOverWidthLabel.setText(
        "Cross Over Width", juce::dontSendNotification
    );*/

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
    
    g.setColour(juce::Colours::black);
    g.fillRect(spectrumAnalyzerBounds);
    g.fillRect(lowPassADSRVisualizerBounds);
    g.fillRect(highPassADSRVisualizerBounds);

    DrawFrequencyResponseCurve(
        g,
        spectrumAnalyzerBounds,
        audioProcessor.crossOverFequency,
        audioProcessor.lowPassFrequency,
        audioProcessor.highPassFrequency
    );
    
    g.setColour(juce::Colours::bisque);
    DrawADSRCurve(g, lowPassADSRVisualizerBounds, audioProcessor.lowPassADSR);
    DrawADSRCurve(g, highPassADSRVisualizerBounds, audioProcessor.highPassADSR);
    
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

    spectrumAnalyzerBounds = bounds.removeFromTop(bounds.getHeight() * 0.45f);
    spectrumAnalyzerBounds.reduce(10, 10);

    spectrumAnalyzer->setBounds(spectrumAnalyzerBounds);

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
    crossOverFrequencySlider.setBounds(crossOverFrequencyControlBounds);
    /*auto crossOverFrequencyLabelBounds =
        crossOverFrequencyControlBounds.removeFromBottom(bounds.getHeight() * 0.2f);*/
    /*crossOverFrequencyLabel.setBounds(crossOverFrequencyLabelBounds);*/

    auto crossOverWidthControlBounds = bounds;
    crossOverWidthSlider.setBounds(crossOverWidthControlBounds);
    /*auto crossOverWidthLabelBounds =
        crossOverWidthControlBounds.removeFromBottom(bounds.getHeight() * 0.2f);*/
    /*crossOverWidthLabel.setBounds(crossOverWidthLabelBounds);*/

    // Left Panel %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    lowPassADSRVisualizerBounds = leftPanelBounds.removeFromTop(leftPanelBounds.getHeight() * 0.4f);
    lowPassADSRVisualizerBounds.reduce(10, 10);

    auto controlHeight = leftPanelBounds.getHeight() / 3;
    int controlWidth = leftPanelBounds.getWidth() / 6;
    int indent = controlWidth / 5;
    const auto controlExpansion = 3;

    auto leftEnvelopeControlsBounds = leftPanelBounds.removeFromTop(controlHeight);
    auto leftInitialBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftPeakBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftSustainBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);

    leftEnvelopeControlsBounds = leftPanelBounds.removeFromTop(controlHeight);
    leftEnvelopeControlsBounds.removeFromLeft(indent);
    auto leftAttackBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftDecayBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto leftReleaseBounds = leftEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    leftEnvelopeControlsBounds.removeFromLeft(controlWidth);

    leftEnvelopeControlsBounds = leftPanelBounds.removeFromTop(controlHeight);
    leftEnvelopeControlsBounds.removeFromLeft(indent);
    leftEnvelopeControlsBounds.removeFromLeft(indent);
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

    highPassADSRVisualizerBounds = rightPanelBounds.removeFromTop(rightPanelBounds.getHeight() * 0.4f);
    highPassADSRVisualizerBounds.reduce(10, 10);

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
    rightEnvelopeControlsBounds.removeFromLeft(indent);
    auto rightAttackBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightDecayBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);
    auto rightReleaseBounds = rightEnvelopeControlsBounds.removeFromLeft(controlWidth).expanded(controlExpansion);
    rightEnvelopeControlsBounds.removeFromLeft(controlWidth);

    rightEnvelopeControlsBounds = rightPanelBounds.removeFromTop(controlHeight);
    rightEnvelopeControlsBounds.removeFromLeft(indent);
    rightEnvelopeControlsBounds.removeFromLeft(indent);
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

//==============================================================================
/*void BreaQAudioProcessorEditor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    if (bufferToFill.buffer->getNumChannels() > 0) {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

        for (auto i = 0; i < bufferToFill.numSamples; ++i)
            pushNextSampleIntoFifo(channelData[i]);
    }
}

void BreaQAudioProcessorEditor::pushNextSampleIntoFifo(float sample) noexcept {
    if (fifoIndex == fftSize) {
        if (!nextFFTBlockReady) {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[fifoIndex++] = sample;
}

void BreaQAudioProcessorEditor::drawNextFrameOfSpectrum() {
    window.multiplyWithWindowingTable(fftData, fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i) {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
            - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f
        );

        scopeData[i] = level;
    }
}

void BreaQAudioProcessorEditor::timerCallback()
{
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}

void BreaQAudioProcessorEditor::drawFrame(juce::Graphics& g)
{
    for (int i = 1; i < scopeSize; ++i)
    {
        auto width = getLocalBounds().getWidth();
        auto height = getLocalBounds().getHeight();

        g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                      (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
                              juce::jmap(scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
    }
}*/



void BreaQAudioProcessorEditor::DrawFrequencyResponseCurve(
        juce::Graphics& g, 
        juce::Rectangle<int> bounds,
        float crossOverFrequency,
        float lowPassCutOffFrequency,
        float highPassCutOffFrequency) {
    auto left = (double)bounds.getTopLeft().getX();
    auto right = (double)bounds.getTopRight().getX();
    auto top = (double)bounds.getTopLeft().getY();
    auto bottom = (double)bounds.getBottomLeft().getY();

    int crossOverFrequencyX = (int)juce::jmap(
        juce::mapFromLog10((double)crossOverFrequency, 20.0, 20000.0),
        left,
        right
    );

    int lowPassCutOffFrequencyX = (int)juce::jmap(
        juce::mapFromLog10((double)lowPassCutOffFrequency, 20.0, 20000.0),
        left,
        right
    );

    int highPassCutOffFrequencyX = (int)juce::jmap(
        juce::mapFromLog10((double)highPassCutOffFrequency, 20.0, 20000.0),
        left,
        right
    );

    g.setColour(juce::Colours::green);
    g.drawVerticalLine(crossOverFrequencyX, top, bottom);
    g.drawVerticalLine(lowPassCutOffFrequencyX, top, bottom);
    g.drawVerticalLine(highPassCutOffFrequencyX, top, bottom);

    int zeroY = (int)juce::jmap(0.5, top, bottom);

    float highPassEndFrequency = highPassCutOffFrequency;
    for (int i = 5; i > audioProcessor.highPassFilter.f_order; i--) {
        highPassEndFrequency /= 2;
    }

    float lowPassEndFrequency = lowPassCutOffFrequency;
    for (int i = 5; i > audioProcessor.lowPassFilter.f_order; i--) {
        lowPassEndFrequency *= 2;
    }

    int highPassEndFrequencyX = (int)juce::jmap(
        juce::mapFromLog10((double)highPassEndFrequency, 20.0, 20000.0),
        left,
        right
    );

    int highPassControlX = (int)juce::jmap(
        0.5,
        (double)highPassCutOffFrequencyX,
        (double)highPassEndFrequencyX
    );

    int lowPassEndFrequencyX = (int)juce::jmap(
        juce::mapFromLog10((double)lowPassEndFrequency, 20.0, 20000.0),
        left,
        right
    );

    int lowPassControlX = (int)juce::jmap(
        0.5,
        (double)lowPassCutOffFrequencyX,
        (double)lowPassEndFrequencyX
    );

    juce::Path highPassPath;
    highPassPath.startNewSubPath(right, zeroY);
    highPassPath.lineTo(highPassCutOffFrequencyX, zeroY);
    highPassPath.quadraticTo(highPassControlX, zeroY, highPassEndFrequencyX, bottom);

    juce::Path lowPassPath;
    lowPassPath.startNewSubPath(left, zeroY);
    lowPassPath.lineTo(lowPassCutOffFrequencyX, zeroY);
    lowPassPath.quadraticTo(lowPassControlX, zeroY, lowPassEndFrequencyX, bottom);

    g.setColour(juce::Colours::bisque);
    g.strokePath(
        highPassPath,
        juce::PathStrokeType(
            2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    g.strokePath(
        lowPassPath,
        juce::PathStrokeType(
            2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );
}

void BreaQAudioProcessorEditor::DrawADSRCurve(
        juce::Graphics& g,
        juce::Rectangle<int> bounds,
        ADSR adsr) {
    auto left = (double)bounds.getTopLeft().getX();
    auto right = (double)bounds.getTopRight().getX();
    auto top = (double)bounds.getTopLeft().getY();
    auto bottom = (double)bounds.getBottomLeft().getY();

    auto totalLength = (double)adsr.attackTime + (double)adsr.decayTime + (double)adsr.releaseTime;

    int initialY = (int)juce::jmap(
        (double)adsr.initialLevel,
        bottom,
        top
    );

    int attackX = (int)juce::jmap(
        (double)adsr.attackTime, 
        (double)0.0f, 
        (double)totalLength, 
        left, 
        right
    );

    int attackY = (int)juce::jmap(
        (double)adsr.peakLevel,
        bottom,
        top
    );

    int decayX = (int)juce::jmap(
        (double)(adsr.attackTime + adsr.decayTime),
        (double)0.0f,
        (double)totalLength,
        left,
        right
    );

    int decayY = (int)juce::jmap(
        (double)adsr.sustainLevel,
        bottom,
        top
    );

    int releaseX = right;
    int releaseY = bottom;

    auto attackCurveX = juce::jmap(
        (double)adsr.attackCurve, 
        0.0, 
        2.0, 
        left, 
        (double)attackX
    );

    auto attackCurveY = juce::jmap(
        (double)adsr.attackCurve,
        0.0,
        2.0,
        (double)attackY,
        (double)initialY
    );

    auto decayCurveX = juce::jmap(
        (double)adsr.decayCurve,
        0.0,
        2.0,
        (double)attackX,
        (double)decayX
    );

    auto decayCurveY = juce::jmap(
        (double)adsr.decayCurve,
        0.0,
        2.0,
        (double)decayY,
        (double)attackY
    );

    auto releaseCurveX = juce::jmap(
        (double)adsr.releaseCurve,
        0.0,
        2.0,
        (double)decayX,
        (double)releaseX
    );

    auto releaseCurveY = juce::jmap(
        (double)adsr.releaseCurve,
        0.0,
        2.0,
        (double)releaseY,
        (double)decayY
    );

    g.setColour(juce::Colours::green);
    g.drawVerticalLine(attackX, top, bottom);
    g.drawVerticalLine(decayX, top, bottom);

    juce::Path path;
    path.startNewSubPath(left, initialY);
    path.quadraticTo(attackCurveX, attackCurveY, attackX, attackY);
    path.quadraticTo(decayCurveX, decayCurveY, decayX, decayY);
    path.quadraticTo(releaseCurveX, releaseCurveY, releaseX, releaseY);

    g.setColour(juce::Colours::bisque);
    g.strokePath(
        path,
        juce::PathStrokeType(
            2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    
}