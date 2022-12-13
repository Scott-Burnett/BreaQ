/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BreaQLookAndFeel.h"
#include "SpectrumAnalyzer.h"

//==============================================================================
/*
*/
class BreaQAudioProcessorEditor  : public juce::AudioProcessorEditor //, juce::AudioSource, juce::Timer
{
public:
    SpectrumAnalyzer* spectrumAnalyzer;
    // static const int fftOrder = 11;
    // static const int fftSize = 1 << fftOrder;
    // static const int scopeSize = 512;

    BreaQAudioProcessorEditor (BreaQAudioProcessor&, juce::AudioProcessorValueTreeState&, SpectrumAnalyzer&);
    ~BreaQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    // void timerCallback() override;

private:
    BreaQAudioProcessor& audioProcessor;
    BreaQLookAndFeel breaQLookAndFeel;


    // Cross Over Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    juce::Slider crossOverFrequencySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> 
        crossOverFrequencyAttachment;
    juce::Label crossOverFrequencyLabel;

    juce::Slider crossOverWidthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        crossOverWidthAttachment;
    juce::Label crossOverWidthLabel;

    juce::Slider lowPassOrderSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassOrderAttachment;

    juce::Slider highPassOrderSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassOrderAttachment;

    // High Pass Envelope Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    juce::Slider highPassInitialSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassInitialAttachment;

    juce::Slider highPassAttackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassAttackAttachment;

    juce::Slider highPassAttackCurveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassAttackCurveAttachment;

    juce::Slider highPassPeakSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassPeakAttachment;

    juce::Slider highPassDecaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassDecayAttachment;

    juce::Slider highPassDecayCurveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassDecayCurveAttachment;

    juce::Slider highPassSustainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassSustainAttachment;

    juce::Slider highPassReleaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassReleaseAttachment;

    juce::Slider highPassReleaseCurveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        highPassReleaseCurveAttachment;

    // Low Pass Envelope Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    juce::Slider lowPassInitialSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassInitialAttachment;

    juce::Slider lowPassAttackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassAttackAttachment;

    juce::Slider lowPassAttackCurveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassAttackCurveAttachment;

    juce::Slider lowPassPeakSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassPeakAttachment;

    juce::Slider lowPassDecaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassDecayAttachment;

    juce::Slider lowPassDecayCurveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassDecayCurveAttachment;

    juce::Slider lowPassSustainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassSustainAttachment;

    juce::Slider lowPassReleaseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassReleaseAttachment;

    juce::Slider lowPassReleaseCurveSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        lowPassReleaseCurveAttachment;
    
    juce::Rectangle<int> spectrumAnalyzerBounds;
    juce::Rectangle<int> lowPassADSRVisualizerBounds;
    juce::Rectangle<int> highPassADSRVisualizerBounds;

    // Analyzer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /*juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    float scopeData[scopeSize];

    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextFrameOfSpectrum();
    void drawFrame(juce::Graphics& g);*/

    // Curves %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    void DrawFrequencyResponseCurve(
        juce::Graphics& g,
        juce::Rectangle<int> bounds,
        float crossOverFrequency,
        float lowPassCutOffFrequency,
        float highPassCutOffFrequency
    );

    void DrawADSRCurve(
        juce::Graphics& g, 
        juce::Rectangle<int> bounds, 
        ADSR adsr
    );

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessorEditor)
};
