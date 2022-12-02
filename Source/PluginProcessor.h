/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <ParallelLowPassFilter.h>
#include <ParallelHighPassFilter.h>

//==============================================================================
/**
*/

enum ChainPositions {
    LowCut,
    Notch,
    HighCut
};

enum Slope {
    DbOct12,
    DbOct24,
    DbOct36,
    DbOct48
};

struct ChainSettings
{
    float notchFrequency{ 0.0f };
    float notchGainInDecibels{ 0.0f };
    float notchQuality{ 0.0f };

    float lowCutFrequency{ 0.0f };
    Slope lowCutSlope{ Slope::DbOct12 };

    float highCutFrequency{ 0.0f };
    Slope highCutSlope{ Slope::DbOct12 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class BreaQAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
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

    /*juce::AudioProcessorValueTreeState apvts{
        *this, nullptr, "Parameters", createParameterLayout()
    };*/

private:
    /*using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    MonoChain leftChain, rightChain;*/

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState parameters{
        *this, nullptr, "Parameters", createParameterLayout()
    };

    std::atomic<float>* crossOverFrequencyParameter = nullptr;
    std::atomic<float>* crossOverWidthParameter = nullptr;

    float lowPassFrequency;
    float highPassFrequency;

    ParallelLowPassFilter lowPassFilter;
    ParallelHighPassFilter highPassFilter;

    /*void updateNotchFilter(ChainSettings chainSettings);
    void updateLowCutFilter(ChainSettings chainSettings);*/
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessor)
};
