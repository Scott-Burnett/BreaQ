/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "ParallelLowPassFilter.h"
#include "ParallelHighPassFilter.h"
#include "ADSR.h"
#include "SpectrumAnalyzer.h"

//==============================================================================
/**
*/

enum Slope {
    DbOct12,
    DbOct24,
    DbOct36,
    DbOct48
};

//==============================================================================
/**
*/
class BreaQAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    float crossOverFequency;
    float lowPassFrequency;
    float highPassFrequency;

    ParallelLowPassFilter lowPassFilter;
    ParallelHighPassFilter highPassFilter;

    ADSR lowPassADSR;
    ADSR highPassADSR;

    SpectrumAnalyzer spectrumAnalyzer;

    //==============================================================================
    BreaQAudioProcessor();
    ~BreaQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState parameters {
        *this, nullptr, "Parameters", createParameterLayout()
    };

    std::atomic<float>* crossOverFrequencyParameter = nullptr;
    std::atomic<float>* crossOverWidthParameter = nullptr;
    std::atomic<float>* highPassOrderParameter = nullptr;
    std::atomic<float>* lowPassOrderParameter = nullptr;

    std::atomic<float>* lowPassInitialParameter = nullptr;
    std::atomic<float>* lowPassAttackParameter = nullptr;
    std::atomic<float>* lowPassAttackCurveParameter = nullptr;
    std::atomic<float>* lowPassPeakParameter = nullptr;
    std::atomic<float>* lowPassDecayParameter = nullptr;
    std::atomic<float>* lowPassDecayCurveParameter = nullptr;
    std::atomic<float>* lowPassSustainParameter = nullptr;
    std::atomic<float>* lowPassReleaseParameter = nullptr;
    std::atomic<float>* lowPassReleaseCurveParameter = nullptr;

    std::atomic<float>* highPassInitialParameter = nullptr;
    std::atomic<float>* highPassAttackParameter = nullptr;
    std::atomic<float>* highPassAttackCurveParameter = nullptr;
    std::atomic<float>* highPassPeakParameter = nullptr;
    std::atomic<float>* highPassDecayParameter = nullptr;
    std::atomic<float>* highPassDecayCurveParameter = nullptr;
    std::atomic<float>* highPassSustainParameter = nullptr;
    std::atomic<float>* highPassReleaseParameter = nullptr;
    std::atomic<float>* highPassReleaseCurveParameter = nullptr;

    
    // Slope lowPassOrder;
    // Slope highPassOrder;

    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessor)
};
