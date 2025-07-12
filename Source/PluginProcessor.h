#pragma once

#include <../JuceLibraryCode/JuceHeader.h>
// #include "PluginEditor.h"

//==============================================================================
/**
*/
class Slice {
public:
    float probability;
    int length;
    int plusSixteen;
    int progress;
    long plusSixteenProgress;
    int sliceId;
    bool enabled;
    bool isOn;

    Slice();
    ~Slice();

    void setSliceId(int);
    void init(juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

private:
    std::atomic<float>* probabilityParameter = nullptr;
    std::atomic<float>* lengthParameter = nullptr;
    std::atomic<float>* plusSixteenParameter = nullptr;
    std::atomic<float>* progressParameter = nullptr;
    std::atomic<float>* enabledParameter = nullptr;
    std::atomic<float>* isOnParameter = nullptr;
};

//==============================================================================
/**
*/
class Strip {
public:
    Slice* slices;
    Slice* currentSlice;

    float probability;
    int group;
    int choice;
    int noteNumber;
    int stripId;
    bool enabled;
    bool bypassed;
    bool isOn;

    Strip();
    ~Strip();

    void setStrpId(int);
    void init(juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

private:
    std::atomic<float>* probabilityParameter = nullptr;
    std::atomic<float>* groupParameter = nullptr;  
    std::atomic<float>* choiceParameter = nullptr;
    std::atomic<float>* enabledParameter = nullptr;
    std::atomic<float>* bypassedParameter = nullptr;
};

//==============================================================================
/**
*/
class Group {
public:
    int id;
    bool shed;
    bool enabled;

    Strip* currentStrip;

    void setGroupId(int);
    void init(juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

private:
    std::atomic<float>* shedParameter = nullptr;
    // TODO: Enabled Parameter
};

//==============================================================================
/**
*/
class BreaQAudioProcessorEditor;

class GroupDto;
class StripDto;
class SliceDto;

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
    GroupDto* groupDtos;

    Strip* strips;
    StripDto* stripDtos;

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

    bool needsRepaint;

    juce::Random random;

    juce::MidiBuffer processedBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessor)
};
