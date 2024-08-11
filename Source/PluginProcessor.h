#pragma once

#include <JuceLibraryCode/JuceHeader.h>
#include "ParallelLowPassFilter.h"
#include "ParallelHighPassFilter.h"
#include "ADSR.h"
#include "SpectrumAnalyzer.h"

//==============================================================================
/**
*/
class Slice {
public:
  float probability;
  int length;
  int progress;
  bool enabled;
  bool isOn;

private:
  std::atomic<float>* probabilityParameter = nullptr;
  std::atomic<float>* lengthParameter = nullptr;
  std::atomic<float>* progressParameter = nullptr;
  std::atomic<float>* enabledParameter = nullptr;
  std::atomic<float>* isOnParameter = nullptr;
};

//==============================================================================
/**
*/
class Strip {
public:
  float probability;
  int group;
  int noteNumber;
  bool enabled;
  bool isOn;

private:
  Slice* slices;
  Slice* currentSlice;

  std::atomic<float>* probabilityParameter = nullptr;
  std::atomic<float>* groupParameter = nullptr;
  std::atomic<float>* noteNumberParameter = nullptr;     
  std::atomic<float>* enabledParameter = nullptr;
  std::atomic<float>* isOnParameter = nullptr;
};

//==============================================================================
/**
*/
class Group {
public:
  int id;
  bool shed;

private:
  std::atomic<float>* shedParameter = nullptr;
};

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
class BreaQAudioProcessorEditor;

//==============================================================================
/**
*/
class BreaQAudioProcessor  : 
    public juce::AudioProcessor, 
    public juce::AudioProcessorParameter::Listener, 
    public juce::Timer
    #if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
    #endif
{
public:
    BreaQAudioProcessorEditor* editor;

    Group* groups;
    Strip* strips;

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
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
    void timerCallback() override;

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

    bool parametersChanged;

    juce::MidiBuffer processedBuffer;

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

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessor)
};
