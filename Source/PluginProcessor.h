#pragma once

#include <../JuceLibraryCode/JuceHeader.h>

//==============================================================================
/**
*/
struct Lock {
public:
    Lock();
    Lock(int* key);
    ~Lock();

    bool isLocked();
    bool hasLock(int key);
    bool canLock(int key);
    bool tryLock(int key);
    void unLock(int key);

private:
    int* shape;

    bool locked;
    int holder;
};

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

    bool isPreparedToPlay();

    void setSliceId(int);
    void init(juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

private:
    std::atomic<float>* probabilityParameter = nullptr;
    std::atomic<float>* lengthParameter = nullptr;
    std::atomic<float>* plusSixteenParameter = nullptr;
    std::atomic<float>* enabledParameter = nullptr;
};

//==============================================================================
/**
*/
class Strip {
public:
    Slice* slices;
    Slice* currentSlice;

    Lock lock;

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

    bool isPreparedToPlay();

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
struct Step {
public:
    bool hasValue;

    int noteNumber;
    int channel;
    juce::uint8 velocity;

    Strip* strip;
    Slice* slice;

    Step();
    ~Step();

    static Step loadFrom(Strip*, Slice*);
    static bool differs(Step*, Step*);

    void clear();

    void addNoteOnEvent(juce::MidiBuffer&, int);
    void addNoteOffEvent(juce::MidiBuffer&, int);
};


//==============================================================================
/**
*/
class Group {
public:
    int id;
    int length;
    int plusSixteen;
    int progress;
    long plusSixteenProgress;
    bool enabled;
    bool isOn;

    Strip* currentStrip;

    int step;
    int numSteps;
    Step* sequence;

    Group();
    ~Group();

    void setGroupId(int);
    void init(juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

    void createSequence(Strip* strips, juce::Random random);
    void takeStep(juce::MidiBuffer&, int, Strip*, juce::Random);
    void newNote(Step*, Step*, juce::MidiBuffer&, int);

private:
    std::atomic<float>* lengthParameter = nullptr;
    std::atomic<float>* plusSixteenParameter = nullptr;
    std::atomic<float>* enabledParameter = nullptr;
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
