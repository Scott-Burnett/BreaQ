/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BreaQAudioProcessor::BreaQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (
         BusesProperties()
#if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
    #endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
        )
#endif
{
    crossOverFrequencyParameter = parameters.getRawParameterValue("crossOverFrequency");
    crossOverWidthParameter = parameters.getRawParameterValue("crossOverWidth");  
    highPassOrderParameter = parameters.getRawParameterValue("highPassRollOff");
    lowPassOrderParameter = parameters.getRawParameterValue("lowPassRollOff");
}

BreaQAudioProcessor::~BreaQAudioProcessor() {
}

//==============================================================================
const juce::String BreaQAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool BreaQAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BreaQAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BreaQAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BreaQAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int BreaQAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BreaQAudioProcessor::getCurrentProgram() {
    return 0;
}

void BreaQAudioProcessor::setCurrentProgram (int index) {
}

const juce::String BreaQAudioProcessor::getProgramName (int index) {
    return {};
}

void BreaQAudioProcessor::changeProgramName (int index, const juce::String& newName) {
}

//==============================================================================
void BreaQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    lowPassFilter.sampleRate = static_cast<float>(sampleRate);
    highPassFilter.sampleRate = static_cast<float>(sampleRate);
}

void BreaQAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

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

void BreaQAudioProcessor::processBlock (
        juce::AudioBuffer<float>& audioBuffer, 
        juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        audioBuffer.clear (i, 0, audioBuffer.getNumSamples());

    // ToDo: Move These
    const auto crossOverFrequency = crossOverFrequencyParameter->load();
    const auto crossOverWidth = crossOverWidthParameter->load();
    const auto highPassOrder = (int)highPassOrderParameter->load();
    const auto lowPassOrder = (int)lowPassOrderParameter->load();

    highPassFilter.f_order = highPassOrder + 1;
    lowPassFilter.f_order = lowPassOrder + 1;

    lowPassFrequency = crossOverFrequency + crossOverWidth;
    highPassFrequency = crossOverFrequency - crossOverWidth;

    lowPassFrequency = std::clamp(lowPassFrequency, 20.0f, 20000.0f);
    highPassFrequency = std::clamp(highPassFrequency, 20.0f, 20000.0f);

    lowPassFilter.cutOffFrequency = lowPassFrequency;
    highPassFilter.cutOffFrequency = highPassFrequency;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    auto leftInputChannel = audioBuffer.getReadPointer(0);
    auto rightInputChannel = audioBuffer.getReadPointer(1);

    int bufferSize = audioBuffer.getNumSamples();
    float* leftHighPassOutputChannel = new float[bufferSize];
    float* rightHighPassOutputChannel = new float[bufferSize];
    float* leftLowPassOutputChannel = new float[bufferSize];
    float* rightLowPassOutputChannel = new float[bufferSize];

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    lowPassFilter.processBlock(
        leftLowPassOutputChannel,
        rightLowPassOutputChannel,
        leftInputChannel,
        rightInputChannel,
        bufferSize
    );

    highPassFilter.processBlock(
        leftHighPassOutputChannel,
        rightHighPassOutputChannel,
        leftInputChannel,
        rightInputChannel,
        bufferSize
    );

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    auto leftOutputChannel = audioBuffer.getWritePointer(0);
    auto rightOutputChannel = audioBuffer.getWritePointer(1);

    for (auto i = 0; i < bufferSize; i++) {
        float leftSample = 0.5f * (leftLowPassOutputChannel[i] + leftHighPassOutputChannel[i]);
        leftOutputChannel[i] = leftSample;

        float rightSample = 0.5f * (rightLowPassOutputChannel[i] + rightHighPassOutputChannel[i]);
        rightOutputChannel[i] = rightSample;
    }

    // ??
    delete(leftHighPassOutputChannel);
    delete(rightHighPassOutputChannel);
    delete(leftLowPassOutputChannel);
    delete(rightLowPassOutputChannel);
}

//==============================================================================
bool BreaQAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BreaQAudioProcessor::createEditor() {
    return new BreaQAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void BreaQAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    juce::MemoryOutputStream mos (destData, true);
    parameters.state.writeToStream(mos);
}

void BreaQAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        parameters.replaceState(tree);
        // ToDo: Update everything with new parameters
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout BreaQAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "crossOverFrequency", "Cross Over Frequency", juce::NormalisableRange{
            20.0f, 20000.0f, 0.1f, 0.2f, false
        },
        1000.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "crossOverWidth", "Cross Over Width", juce::NormalisableRange{
            -500.0f, 500.0f, 0.1f, 1.0f, false
        },
        40.0f
    ));

    juce::StringArray rollOffOrderParameterOptions;
    for (int i = 12; i < 60; i += 12) {
        juce::String str;
        str << i << " db/Oct";
        rollOffOrderParameterOptions.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "lowPassRollOff", 
        "Low Pass Roll Off", 
        rollOffOrderParameterOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "highPassRollOff",
        "High Pass Roll Off",
        rollOffOrderParameterOptions,
        0
        ));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BreaQAudioProcessor();
}
