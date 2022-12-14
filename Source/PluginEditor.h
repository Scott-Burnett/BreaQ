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
#include "ADSRVisualizer.h"

//==============================================================================
/*
*/
class BreaQAudioProcessorEditor  : public juce::AudioProcessorEditor //, juce::AudioSource, juce::Timer
{
public:
    // Colours:
    /*
    * background": "#0d1016",
    * background dark : #00010a
    * 
    * lilac : "#daade0"
    * bright pink : "#ee68af"
    * 
    * grid 1: #292d36
    * grid 2: #3d424d
    * grid 3: #b3b1ad
    * 
    * pale orange: #fdc68a
    * pale Orange again": "#f5bb6c"
    * pale orange paler : #f7cb8b
    * 
    */

    const juce::Colour background1 = juce::Colour(13, 16, 22); // (0x0d1016);
    const juce::Colour background2 = juce::Colour(0, 1, 10); // (0x00010a);

    const juce::Colour lilac = juce::Colour(218, 173, 224); // (0xdaade0);

    const juce::Colour pink = juce::Colour(238, 104, 175); // (0xee68af);
    
    const juce::Colour grid1 = juce::Colour(41, 45, 54); // (0x292d36);
    const juce::Colour grid2 = juce::Colour(61, 66, 77); // (0x3d424d);
    const juce::Colour grid3 = juce::Colour(179, 177, 173); // (0xb3b1ad);
    
    const juce::Colour orange2 = juce::Colour(245, 187, 108); // (0xf5bb6c);
    const juce::Colour orange1 = juce::Colour(253, 198, 138); // (0xfdc68a);
    const juce::Colour orange3 = juce::Colour(247, 203, 139); // (0xf7cb8b);

    SpectrumAnalyzer* spectrumAnalyzer;
    ADSRVisualizer* lowPassADSRVisualizer;
    ADSRVisualizer* highPassADSRVisualizer;

    BreaQAudioProcessorEditor (
        BreaQAudioProcessor&, 
        juce::AudioProcessorValueTreeState&, 
        SpectrumAnalyzer&
    );
    ~BreaQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void loadParameters(
        float crossOverFrequency,
        float crossOverWidth,
        float highPassOrder,
        float lowPassOrder,

        float lowPassInitial,
        float lowPassAttack,
        float lowPassAttackCurve,
        float lowPassPeak,
        float lowPassDecay,
        float lowPassDecayCurve,
        float lowPassSustain,
        float lowPassRelease,
        float lowPassReleaseCurve,

        float highPassInitial,
        float highPassAttack,
        float highPassAttackCurve,
        float highPassPeak,
        float highPassDecay,
        float highPassDecayCurve,
        float highPassSustain,
        float highPassRelease,
        float highPassReleaseCurve
    );

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
    
    juce::Rectangle<int> leftPanelDecorationBounds;
    juce::Rectangle<int> rightPanelDecorationBounds;
    juce::Rectangle<int> centrePanelDecorationBounds;
    juce::Rectangle<int> spectrumAnalyzerBounds;
    juce::Rectangle<int> spectrumAnalyzerDecorationBounds;
    juce::Rectangle<int> lowPassADSRVisualizerBounds;
    juce::Rectangle<int> lowPassADSRVisualizerDecorationBounds;
    juce::Rectangle<int> highPassADSRVisualizerBounds;
    juce::Rectangle<int> highPassADSRVisualizerDecorationBounds;

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

    // Decorations %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    void decorateBounds(juce::Graphics&, juce::Rectangle<int> bounds, juce::Colour borderColour, juce::Colour fillColour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessorEditor)
};
