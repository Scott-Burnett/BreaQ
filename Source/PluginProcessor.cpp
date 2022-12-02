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
        ),
#else
    : // ToDo: Old way is better
#endif
    parameters(
        *this, 
        nullptr, 
        juce::Identifier("BreaQ"),
        {
            std::make_unique<juce::AudioParameterFloat>(
                "crossOverFrequency", "Cross Over Frequency", juce::NormalisableRange{
                    20.0f, 20000.0f, 0.1f, 0.2f, false
                },
                500.0f
            ),
            std::make_unique<juce::AudioParameterFloat>(
                "crossOverWidth", "Cross Over Width", juce::NormalisableRange{
                    20.0f, 20000.0f, 0.1f, 0.2f, false
                },
                500.0f
            )
        }
    )
{
    crossOverFrequencyParameter = parameters.getRawParameterValue("crossOverFrequency");
    crossOverWidthParameter = parameters.getRawParameterValue("crossOverWidth");  
}

BreaQAudioProcessor::~BreaQAudioProcessor()
{
}

//==============================================================================
const juce::String BreaQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BreaQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BreaQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BreaQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BreaQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BreaQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BreaQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BreaQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BreaQAudioProcessor::getProgramName (int index)
{
    return {};
}

void BreaQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BreaQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    /*juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(apvts);

    updateNotchFilter(chainSettings);
    updateLowCutFilter(chainSettings);*/

    lowPassFilter.sampleRate = static_cast<float>(sampleRate);
    lowPassFilter.initializeOutputBuffers(samplesPerBlock); // ToDo: Fix this
    
}

void BreaQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BreaQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BreaQAudioProcessor::processBlock (juce::AudioBuffer<float>& audioBuffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        audioBuffer.clear (i, 0, audioBuffer.getNumSamples());

    // ToDo: Move These
    const auto crossOverFrequency = crossOverFrequencyParameter->load();
    const auto crossOverWidth = crossOverWidthParameter->load();

    lowPassFilter.cutOffFrequency = crossOverFrequency;

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

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    auto leftOutputChannel = audioBuffer.getWritePointer(0);
    auto rightOutputChannel = audioBuffer.getWritePointer(1);

    for (auto i = 0; i < bufferSize; i++) {
        // ToDo: Sum Input with High Pass Filter
        leftOutputChannel[i] = leftLowPassOutputChannel[i];
        rightOutputChannel[i] = rightLowPassOutputChannel[i];
    }

    free(leftHighPassOutputChannel);
    free(rightHighPassOutputChannel);
    free(leftLowPassOutputChannel);
    free(rightLowPassOutputChannel);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /*
    auto chainSettings = getChainSettings(apvts);

    updateNotchFilter(chainSettings);
    updateLowCutFilter(chainSettings);

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
    */

    // const float* leftPointer = audioBuffer.getReadPointer(0);
}

//==============================================================================
bool BreaQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BreaQAudioProcessor::createEditor()
{
    // return new BreaQAudioProcessorEditor (*this);
    return new BreaQAudioProcessorEditor (*this, parameters);

    // return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void BreaQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BreaQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

/*ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts) {
    ChainSettings chainSettings;

    // TODO: This looks bad
    chainSettings.notchFrequency = apvts.getRawParameterValue("Notch Freq")-> load();
    chainSettings.notchGainInDecibels = apvts.getRawParameterValue("Notch Gain")-> load();
    chainSettings.notchQuality = apvts.getRawParameterValue("Notch Quality")-> load();

    chainSettings.lowCutFrequency = apvts.getRawParameterValue("LowCut Freq")-> load();
    chainSettings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")-> load());

    chainSettings.highCutFrequency = apvts.getRawParameterValue("HighCut Freq")-> load();
    chainSettings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());

    return chainSettings;
}*/

juce::AudioProcessorValueTreeState::ParameterLayout BreaQAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "crossOverFrequency", "Cross Over Frequency", juce::NormalisableRange{
            20.0f, 20000.0f, 0.1f, 0.2f, false
        },
        500.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "crossOverWidth", "Cross Over Width", juce::NormalisableRange{
            20.0f, 20000.0f, 0.1f, 0.2f, false
        },
        500.0f
    ));

    /*layout.add(std::make_unique<juce::AudioParameterFloat>(
        "LowCut Freq", 
        "LowCut Freq", 
        juce::NormalisableRange<float>(
                20.0f, 20000.0f, 1.0f, 0.25f),
        20.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "HighCut Freq",
        "HighCut Freq",
        juce::NormalisableRange<float>(
            20.0f, 20000.0f, 1.0f, 0.25f),
        20000.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Notch Freq",
        "Notch Freq",
        juce::NormalisableRange<float>(
            20.0f, 20000.0f, 1.0f, 0.25f),
        750.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Notch Gain",
        "Notch Gain",
        juce::NormalisableRange<float>(
            -24.0f, 24.0f, 0.5f, 1.0f),
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Notch Quality",
        "Notch Quality",
        juce::NormalisableRange<float>(
            0.1f, 10.0f, 0.05f, 1.0f),
        0.0f
    ));

    juce::StringArray stringArray;
    for (int i = 12; i < 60; i += 12) {
        juce::String str;
        str << i << " db/Oct";
        stringArray.add(str);
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "LowCut Slope",
        "LowCut Slope",
        stringArray,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "HighCut Slope",
        "HighCut Slope",
        stringArray,
        0
    ));*/



    return layout;
}

/*void BreaQAudioProcessor::updateNotchFilter(ChainSettings chainSettings) {
    auto notchCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        getSampleRate(),
        chainSettings.notchFrequency,
        chainSettings.notchQuality,
        juce::Decibels::decibelsToGain(chainSettings.notchGainInDecibels)
    );

    *leftChain.get<ChainPositions::Notch>().coefficients = *notchCoefficients;
    *rightChain.get<ChainPositions::Notch>().coefficients = *notchCoefficients;
}
void BreaQAudioProcessor::updateLowCutFilter(ChainSettings chainSettings) {
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        chainSettings.lowCutFrequency,
        getSampleRate(),
        2 * (chainSettings.lowCutSlope + 1)
    );

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();

    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);

    switch (chainSettings.lowCutSlope) {
    case DbOct48:
        *leftLowCut.get<3>().coefficients = *lowCutCoefficients[3];
        leftLowCut.setBypassed<3>(false);
        // Fall Through
    case DbOct36:
        *leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
        leftLowCut.setBypassed<2>(false);
        // Fall Through
    case DbOct24:
        *leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
        leftLowCut.setBypassed<1>(false);
        // Fall Through
    case DbOct12:
        *leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
        leftLowCut.setBypassed<0>(false);
        break;
    }

    auto& rightLowCut = leftChain.get<ChainPositions::LowCut>();

    rightLowCut.setBypassed<0>(true);
    rightLowCut.setBypassed<1>(true);
    rightLowCut.setBypassed<2>(true);
    rightLowCut.setBypassed<3>(true);

    switch (chainSettings.lowCutSlope) {
    case DbOct48:
        *rightLowCut.get<3>().coefficients = *lowCutCoefficients[3];
        rightLowCut.setBypassed<3>(false);
        // Fall Through
    case DbOct36:
        *rightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
        rightLowCut.setBypassed<2>(false);
        // Fall Through
    case DbOct24:
        *rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
        rightLowCut.setBypassed<1>(false);
        // Fall Through
    case DbOct12:
        *rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
        rightLowCut.setBypassed<0>(false);
        break;
    }
}*/

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BreaQAudioProcessor();
}
