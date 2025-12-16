#pragma once

#include <../JuceLibraryCode/JuceHeader.h>

//==============================================================================
/**
*/
struct Lock {
public:
    Lock();
    Lock(int* key);
    Lock(int* key, std::function<void()> onLockAquired);
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
    
    std::function<void()> onLockAquired;
};

//==============================================================================
/**
*/
class Strip {
public:
    Lock lock;

    float probability;
    int group;
    int choice;
    // int noteNumber;
    int stripId;
    bool enabled;
    bool isOn;

    // New Stuff
    float choke;
    int variants;

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

    // New Stuff
    std::atomic<float>* chokeParameter = nullptr;
    std::atomic<float>* variantsParameter = nullptr;
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

    Step();
    ~Step();

    static Step loadFrom(Strip*);
    static Step empty();
    static bool differs(Step, Step);

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
    bool enabled;
    bool loop;
    bool isOn;

    Strip* currentStrip;

    int step;
    int numSteps;
    Step* sequence;
    bool needsReset;

    // new stuff
    int tjopLength;
    int tjopLengthMultiplier;
    int intervalLength;
    int intervalLengthMultiplier;
    int sequenceLength;
    int sequenceLengthMultiplier;
    float density;

    Group();
    ~Group();

    void setGroupId(int);
    void init(juce::AudioProcessorValueTreeState&, juce::AudioProcessorParameter::Listener&);
    void createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout&);
    void loadParameters();

    void createSequence(Strip* strips);
    void takeStep(juce::MidiBuffer&, int, Strip*);
    void newNote(Step, Step, juce::MidiBuffer&, int);
    void scheduleReset();

private:
    std::atomic<float>* lengthParameter = nullptr;
    std::atomic<float>* plusSixteenParameter = nullptr;
    std::atomic<float>* enabledParameter = nullptr;
    std::atomic<float>* loopParameter = nullptr;
    // New Stuff
    std::atomic<float>* tjopLengthParameter = nullptr;
    std::atomic<float>* tjopLengthMultiplierParameter = nullptr;
    std::atomic<float>* intervalLengthParameter = nullptr;
    std::atomic<float>* intervalLengthMultiplierParameter = nullptr;
    std::atomic<float>* sequenceLengthParameter = nullptr;
    std::atomic<float>* sequenceLengthMultiplierParameter = nullptr;
    std::atomic<float>* densityParameter = nullptr;
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

    juce::MidiBuffer processedBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BreaQAudioProcessor)
};
