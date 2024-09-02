#pragma once

#include <../JuceLibraryCode/JuceHeader.h>

//==============================================================================
/**
*/
class Slice {
public:
    float probability;
    int length;
    int progress;
    int sliceId;
    bool enabled;
    bool isOn;

    void setSliceId(int);
    void init(int, juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

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
    int stripId;
    bool enabled;
    bool bypassed;
    bool isOn;

    void setStrpId(int);
    void init(int, juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

private:
    const int numSlices = 4;
    Slice* slices;
    Slice* currentSlice;

    std::atomic<float>* probabilityParameter = nullptr;
    std::atomic<float>* groupParameter = nullptr;
    //std::atomic<float>* noteNumberParameter = nullptr;     
    std::atomic<float>* enabledParameter = nullptr;
    std::atomic<float>* bypassedParameter = nullptr;
    //std::atomic<float>* isOnParameter = nullptr;
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

    const int numGroups = 4;
    Group* groups;

    const int numStrips = 32;
    Strip* strips;

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
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState parameters {
        *this, nullptr, "Parameters", createParameterLayout()
    };

    bool parametersChanged;

    juce::MidiBuffer processedBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessor)
};
