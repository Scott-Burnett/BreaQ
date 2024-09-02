#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterNames.h"

//==============================================================================
void Slice::init(
    int id, 
    juce::AudioProcessorValueTreeState& vts, 
    juce::AudioProcessorParameter::Listener& listener
) {
    sliceId = id;

    probabilityParameter = vts.getRawParameterValue(
        ParameterNames::sliceProbability[sliceId]
    );
    lengthParameter = vts.getRawParameterValue(
        ParameterNames::sliceLength[sliceId]
    );
    enabledParameter = vts.getRawParameterValue(
        ParameterNames::sliceEnabled[sliceId]
    );

    vts.getParameter(ParameterNames::sliceProbability[sliceId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::sliceLength[sliceId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::sliceEnabled[sliceId])->
        addListener(&listener);
}

//==============================================================================
void Slice::createParameterLayout (
    juce::AudioProcessorValueTreeState::ParameterLayout& layout
) {
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::sliceProbability[sliceId], 
        "Probability", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::sliceLength[sliceId], 
        "Length", 
        juce::StringArray { 
            "1", "2", "3", "4", "5", "6", "7", "8", 
            "9", "10", "11", "12", "13", "14", "15", "16" 
        },
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::sliceEnabled[sliceId], 
        "Enabled", 
        juce::StringArray { "On", "Off" },
        0
    ));
}

//==============================================================================
void Slice::loadParameters() {
    // probability = probabilityParameter->load();
    // length = lengthParameter->load();
    // enabled = (bool) enabledParameter->load();
}

//==============================================================================
void Strip::init(
    int id, 
    juce::AudioProcessorValueTreeState& vts, 
    juce::AudioProcessorParameter::Listener& listener
) {
    stripId = id;

    probabilityParameter = vts.getRawParameterValue(
        ParameterNames::stripProbability[stripId]
    );
    groupParameter = vts.getRawParameterValue(
        ParameterNames::stripGroup[stripId]
    );
    enabledParameter = vts.getRawParameterValue(
        ParameterNames::stripEnabled[stripId]
    );
    bypassedParameter = vts.getRawParameterValue(
        ParameterNames::stripBypassed[stripId]
    );

    vts.getParameter(ParameterNames::stripProbability[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripGroup[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripEnabled[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripBypassed[stripId])->
        addListener(&listener);

    slices = new Slice[numSlices];

    // for (int i = 0, offset = stripId * numSlices; i < numSlices; i++) {
    //     slices[i].init(i + offset, vts, listener);
    // }
}

//==============================================================================
void Strip::createParameterLayout(
    juce::AudioProcessorValueTreeState::ParameterLayout& layout
) {
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::stripProbability[stripId], 
        "Probability", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripGroup[stripId], 
        "Group", 
        juce::StringArray { "1", "2", "3", "4" },
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripEnabled[stripId], 
        "Enabled", 
        juce::StringArray { "On", "Off" },
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripBypassed[stripId], 
        "Bypass", 
        juce::StringArray { "On", "Off" },
        0
    ));

    for (int i = 0; i < numSlices; i++) {
        slices[i].createParameterLayout(layout);
    }
}

//==============================================================================
void Strip::loadParameters() {
    // probability = probabilityParameter->load();
    // group = groupParameter->load();
    // enabled = (bool) enabledParameter->load();
    // bypassed = (bool) bypassedParameter->load();
}

//==============================================================================
BreaQAudioProcessor::BreaQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (
         BusesProperties()
#if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
    #endif
#endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
        )
#endif
{
    /*strips = new Strip[numStrips];

    for (int i = 0; i < numStrips; i++) {
        strips[i].init(i, parameters, *this);
    }*/

    parametersChanged = false;

    startTimer(60);
}

//==============================================================================
BreaQAudioProcessor::~BreaQAudioProcessor() {

}

//==============================================================================
void BreaQAudioProcessor::parameterValueChanged(int parameterIndex, float newValue) {
    parametersChanged = true;
}

//==============================================================================
void BreaQAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {

}

//==============================================================================
void BreaQAudioProcessor::timerCallback() {
    // if (parametersChanged) {
    //     for (int i = 0; i < numStrips; i++) {
    //         strips[i].loadParameters();
    //     }

    //     // TODO: Load Editor State

    //     parametersChanged = false;
    // }
}

//==============================================================================
const juce::String BreaQAudioProcessor::getName() const {
    return JucePlugin_Name;
}

//==============================================================================
bool BreaQAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

//==============================================================================
bool BreaQAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

//==============================================================================
bool BreaQAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

//==============================================================================
double BreaQAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

//==============================================================================
int BreaQAudioProcessor::getNumPrograms() {
    // NB: some hosts don't cope very well if you tell them there are 0 programs
    //     so this should be at least 1.
    return 1;   
}

//==============================================================================
int BreaQAudioProcessor::getCurrentProgram() {
    return 0;
}

//==============================================================================
void BreaQAudioProcessor::setCurrentProgram (int index) {
}

//==============================================================================
const juce::String BreaQAudioProcessor::getProgramName (int index) {
    return {};
}

//==============================================================================
void BreaQAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

//==============================================================================
void BreaQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {

}

//==============================================================================
void BreaQAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
#ifndef JucePlugin_PreferredChannelConfigurations
bool BreaQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//==============================================================================
void BreaQAudioProcessor::processBlock (
    juce::AudioBuffer<float>& audioBuffer,
    juce::MidiBuffer& midiBuffer
) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        audioBuffer.clear(i, 0, audioBuffer.getNumSamples());
    }

    processedBuffer.clear();

    juce::MidiBuffer::Iterator iterator(midiBuffer);
    juce::MidiMessage current;
    int samplePos;

    while (iterator.getNextEvent(current, samplePos)) {
        if (current.isNoteOnOrOff()) {
            current.setNoteNumber(50); 
        }
    }

    midiBuffer.swapWith(processedBuffer);
}

//==============================================================================
bool BreaQAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

//==============================================================================
juce::AudioProcessorEditor* BreaQAudioProcessor::createEditor() {
    editor = new BreaQAudioProcessorEditor(*this, parameters);
    return editor;
}

//==============================================================================
void BreaQAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    juce::MemoryOutputStream mos (destData, true);
    parameters.state.writeToStream(mos);
}

//==============================================================================
void BreaQAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        parameters.replaceState(tree);
        // ToDo: Update everything with new parameters
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout BreaQAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    strips = new Strip[numStrips];

    for (int i = 0; i < numStrips; i++) {
        strips[i].init(i, parameters, *this);
    }

    for (int i = 0; i < numStrips; i++) {
        strips[i].createParameterLayout(layout);
    }

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BreaQAudioProcessor();
}
