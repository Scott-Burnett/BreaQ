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

    lowPassInitialParameter = parameters.getRawParameterValue("lowPassInitial");
    lowPassAttackParameter = parameters.getRawParameterValue("lowPassAttack");
    lowPassAttackCurveParameter = parameters.getRawParameterValue("lowPassAttackCurve");
    lowPassPeakParameter = parameters.getRawParameterValue("lowPassPeak");
    lowPassDecayParameter = parameters.getRawParameterValue("lowPassDecay");
    lowPassDecayCurveParameter = parameters.getRawParameterValue("lowPassDecayCurve");
    lowPassSustainParameter = parameters.getRawParameterValue("lowPassSustain");
    lowPassReleaseParameter = parameters.getRawParameterValue("lowPassRelease");
    lowPassReleaseCurveParameter = parameters.getRawParameterValue("lowPassReleaseCurve");

    highPassInitialParameter = parameters.getRawParameterValue("highPassInitial");
    highPassAttackParameter = parameters.getRawParameterValue("highPassAttack");
    highPassAttackCurveParameter = parameters.getRawParameterValue("highPassAttackCurve");
    highPassPeakParameter = parameters.getRawParameterValue("highPassPeak");
    highPassDecayParameter = parameters.getRawParameterValue("highPassDecay");
    highPassDecayCurveParameter = parameters.getRawParameterValue("highPassDecayCurve");
    highPassSustainParameter = parameters.getRawParameterValue("highPassSustain");
    highPassReleaseParameter = parameters.getRawParameterValue("highPassRelease");
    highPassReleaseCurveParameter = parameters.getRawParameterValue("highPassReleaseCurve");

    parameters.getParameter("lowPassInitial")->addListener(this);
    parameters.getParameter("lowPassAttack")->addListener(this);
    parameters.getParameter("lowPassAttackCurve")->addListener(this);
    parameters.getParameter("lowPassPeak")->addListener(this);
    parameters.getParameter("lowPassDecay")->addListener(this);
    parameters.getParameter("lowPassDecayCurve")->addListener(this);
    parameters.getParameter("lowPassSustain")->addListener(this);
    parameters.getParameter("lowPassRelease")->addListener(this);
    parameters.getParameter("lowPassReleaseCurve")->addListener(this);

    parameters.getParameter("highPassInitial")->addListener(this);
    parameters.getParameter("highPassAttack")->addListener(this);
    parameters.getParameter("highPassAttackCurve")->addListener(this);
    parameters.getParameter("highPassPeak")->addListener(this);
    parameters.getParameter("highPassDecay")->addListener(this);
    parameters.getParameter("highPassDecayCurve")->addListener(this);
    parameters.getParameter("highPassSustain")->addListener(this);
    parameters.getParameter("highPassRelease")->addListener(this);
    parameters.getParameter("highPassReleaseCurve")->addListener(this);

    // editor = new BreaQAudioProcessorEditor(*this, parameters, spectrumAnalyzer);
    parametersChanged = false;

    startTimer(500);

    //auto& params = parameters.parameters;
    //    for (auto param : params)
    //    {
            //switch (param->getName(20)) {
            //case "lowPassInitial":
            //case "lowPassAttack":
            //case "lowPassDecay":
            //case "lowPassSustain":
            //case "lowPassRelease":
            //case "lowPassAttackCurve":
            //case "lowPassDecayCurve":
            //case "lowPassReleaseCurve":
      //      param->addListener(this);
      //      param->addListener(this);

     //       return layout;

    /*
    parameters.getParameter("lowPassInitial")->addListener(this);
    parameters.getParameter("lowPassAttack")->addListener(this);
    parameters.getParameter("lowPassAttackCurve")->addListener(this);
    parameters.getParameter("lowPassPeak")->addListener(this);
    parameters.getParameter("lowPassDecay")->addListener(this);
    parameters.getParameter("lowPassDecayCurve")->addListener(this);
    parameters.getParameter("lowPassSustain")->addListener(this);
    parameters.getParameter("lowPassRelease")->addListener(this);
    parameters.getParameter("lowPassReleaseCurve")->addListener(this);

    parameters.getParameter("highPassInitial")->addListener(this);
    parameters.getParameter("highPassAttack")->addListener(this);
    parameters.getParameter("highPassAttackCurve")->addListener(this);
    parameters.getParameter("highPassPeak")->addListener(this);
    parameters.getParameter("highPassDecay")->addListener(this);
    parameters.getParameter("highPassDecayCurve")->addListener(this);
    parameters.getParameter("highPassSustain")->addListener(this);
    parameters.getParameter("highPassRelease")->addListener(this);
    parameters.getParameter("highPassReleaseCurve")->addListener(this);
    */
}

BreaQAudioProcessor::~BreaQAudioProcessor() {
}

//==============================================================================

void BreaQAudioProcessor::parameterValueChanged(int parameterIndex, float newValue) {
    /*
    crossOverFequency = crossOverFrequencyParameter->load();
    const auto crossOverWidth = crossOverWidthParameter->load();
    const auto highPassOrder = (int)highPassOrderParameter->load();
    const auto lowPassOrder = (int)lowPassOrderParameter->load();

    highPassFilter.f_order = highPassOrder + 1;
    lowPassFilter.f_order = lowPassOrder + 1;

    lowPassFrequency = crossOverFequency + crossOverWidth;
    highPassFrequency = crossOverFequency - crossOverWidth;

    lowPassFrequency = std::clamp(lowPassFrequency, 20.0f, 20000.0f);
    highPassFrequency = std::clamp(highPassFrequency, 20.0f, 20000.0f);

    lowPassFilter.cutOffFrequency = lowPassFrequency;
    highPassFilter.cutOffFrequency = highPassFrequency;

    // Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    const auto lowPassInitial = lowPassInitialParameter->load();
    const auto lowPassAttack = lowPassAttackParameter->load();
    const auto lowPassAttackCurve = lowPassAttackCurveParameter->load();
    const auto lowPassPeak = lowPassPeakParameter->load();
    const auto lowPassDecay = lowPassDecayParameter->load();
    const auto lowPassDecayCurve = lowPassDecayCurveParameter->load();
    const auto lowPassSustain = lowPassSustainParameter->load();
    const auto lowPassRelease = lowPassReleaseParameter->load();
    const auto lowPassReleaseCurve = lowPassReleaseCurveParameter->load();

    lowPassADSR.initialLevel = lowPassInitial;
    lowPassADSR.attackTime = lowPassAttack;
    lowPassADSR.attackCurve = lowPassAttackCurve;
    lowPassADSR.peakLevel = lowPassPeak;
    lowPassADSR.decayTime = lowPassDecay;
    lowPassADSR.decayCurve = lowPassDecayCurve;
    lowPassADSR.sustainLevel = lowPassSustain;
    lowPassADSR.releaseTime = lowPassRelease;
    lowPassADSR.releaseCurve = lowPassReleaseCurve;

    const auto highPassInitial = highPassInitialParameter->load();
    const auto highPassAttack = highPassAttackParameter->load();
    const auto highPassAttackCurve = highPassAttackCurveParameter->load();
    const auto highPassPeak = highPassPeakParameter->load();
    const auto highPassDecay = highPassDecayParameter->load();
    const auto highPassDecayCurve = highPassDecayCurveParameter->load();
    const auto highPassSustain = highPassSustainParameter->load();
    const auto highPassRelease = highPassReleaseParameter->load();
    const auto highPassReleaseCurve = highPassReleaseCurveParameter->load();

    highPassADSR.initialLevel = highPassInitial;
    highPassADSR.attackTime = highPassAttack;
    highPassADSR.attackCurve = highPassAttackCurve;
    highPassADSR.peakLevel = highPassPeak;
    highPassADSR.decayTime = highPassDecay;
    highPassADSR.decayCurve = highPassDecayCurve;
    highPassADSR.sustainLevel = highPassSustain;
    highPassADSR.releaseTime = highPassRelease;
    highPassADSR.releaseCurve = highPassReleaseCurve;


    //repaint();
    //editor->crossOverFrequency = crossOverFequency;
    //editor->crossOverWidth = crossOverWidth;
    //editor->highPassOrder = highPassOrder;
    //editor->lowPassOrder = lowPassOrder;
    editor->lowPassADSRVisualizer->initial = lowPassInitial;
    editor->lowPassADSRVisualizer->attack = lowPassAttack;
    editor->lowPassADSRVisualizer->attackCurve = lowPassAttackCurve;
    editor->lowPassADSRVisualizer->peak = lowPassPeak;
    editor->lowPassADSRVisualizer->decay = lowPassDecay;
    editor->lowPassADSRVisualizer->decayCurve = lowPassDecayCurve;
    editor->lowPassADSRVisualizer->sustain = lowPassSustain;
    editor->lowPassADSRVisualizer->release = lowPassRelease;
    editor->lowPassADSRVisualizer->releaseCurve = lowPassReleaseCurve;

    editor->highPassADSRVisualizer->initial = highPassInitial;
    editor->highPassADSRVisualizer->attack = highPassAttack;
    editor->highPassADSRVisualizer->attackCurve = highPassAttackCurve;
    editor->highPassADSRVisualizer->peak = highPassPeak;
    editor->highPassADSRVisualizer->decay = highPassDecay;
    editor->highPassADSRVisualizer->decayCurve = highPassDecayCurve;
    editor->highPassADSRVisualizer->sustain = highPassSustain;
    editor->highPassADSRVisualizer->release = highPassRelease;
    editor->highPassADSRVisualizer->releaseCurve = highPassReleaseCurve;

    //editor->repaint();
    */

    parametersChanged = true;
}


void BreaQAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {

}

void BreaQAudioProcessor::timerCallback() {
    if (parametersChanged) {

        // ToDo: Move These ####################################################
        // Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        crossOverFequency = crossOverFrequencyParameter->load();
        const auto crossOverWidth = crossOverWidthParameter->load();
        const auto highPassOrder = (int)highPassOrderParameter->load();
        const auto lowPassOrder = (int)lowPassOrderParameter->load();

        highPassFilter.f_order = highPassOrder + 1;
        lowPassFilter.f_order = lowPassOrder + 1;

        lowPassFrequency = crossOverFequency + crossOverWidth;
        highPassFrequency = crossOverFequency - crossOverWidth;

        lowPassFrequency = std::clamp(lowPassFrequency, 20.0f, 20000.0f);
        highPassFrequency = std::clamp(highPassFrequency, 20.0f, 20000.0f);

        lowPassFilter.cutOffFrequency = lowPassFrequency;
        highPassFilter.cutOffFrequency = highPassFrequency;

        // Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        const auto lowPassInitial = lowPassInitialParameter->load();
        const auto lowPassAttack = lowPassAttackParameter->load();
        const auto lowPassAttackCurve = lowPassAttackCurveParameter->load();
        const auto lowPassPeak = lowPassPeakParameter->load();
        const auto lowPassDecay = lowPassDecayParameter->load();
        const auto lowPassDecayCurve = lowPassDecayCurveParameter->load();
        const auto lowPassSustain = lowPassSustainParameter->load();
        const auto lowPassRelease = lowPassReleaseParameter->load();
        const auto lowPassReleaseCurve = lowPassReleaseCurveParameter->load();

        lowPassADSR.initialLevel = lowPassInitial;
        lowPassADSR.attackTime = lowPassAttack;
        lowPassADSR.attackCurve = lowPassAttackCurve;
        lowPassADSR.peakLevel = lowPassPeak;
        lowPassADSR.decayTime = lowPassDecay;
        lowPassADSR.decayCurve = lowPassDecayCurve;
        lowPassADSR.sustainLevel = lowPassSustain;
        lowPassADSR.releaseTime = lowPassRelease;
        lowPassADSR.releaseCurve = lowPassReleaseCurve;

        const auto highPassInitial = highPassInitialParameter->load();
        const auto highPassAttack = highPassAttackParameter->load();
        const auto highPassAttackCurve = highPassAttackCurveParameter->load();
        const auto highPassPeak = highPassPeakParameter->load();
        const auto highPassDecay = highPassDecayParameter->load();
        const auto highPassDecayCurve = highPassDecayCurveParameter->load();
        const auto highPassSustain = highPassSustainParameter->load();
        const auto highPassRelease = highPassReleaseParameter->load();
        const auto highPassReleaseCurve = highPassReleaseCurveParameter->load();

        highPassADSR.initialLevel = highPassInitial;
        highPassADSR.attackTime = highPassAttack;
        highPassADSR.attackCurve = highPassAttackCurve;
        highPassADSR.peakLevel = highPassPeak;
        highPassADSR.decayTime = highPassDecay;
        highPassADSR.decayCurve = highPassDecayCurve;
        highPassADSR.sustainLevel = highPassSustain;
        highPassADSR.releaseTime = highPassRelease;
        highPassADSR.releaseCurve = highPassReleaseCurve;

        //if (editor != nullptr) {
        /*
            editor->lowPassADSRVisualizer->initial = lowPassInitial;
            editor->lowPassADSRVisualizer->attack = lowPassAttack;
            editor->lowPassADSRVisualizer->attackCurve = lowPassAttackCurve;
            editor->lowPassADSRVisualizer->peak = lowPassPeak;
            editor->lowPassADSRVisualizer->decay = lowPassDecay;
            editor->lowPassADSRVisualizer->decayCurve = lowPassDecayCurve;
            editor->lowPassADSRVisualizer->sustain = lowPassSustain;
            editor->lowPassADSRVisualizer->release = lowPassRelease;
            editor->lowPassADSRVisualizer->releaseCurve = lowPassReleaseCurve;

            editor->highPassADSRVisualizer->initial = highPassInitial;
            editor->highPassADSRVisualizer->attack = highPassAttack;
            editor->highPassADSRVisualizer->attackCurve = highPassAttackCurve;
            editor->highPassADSRVisualizer->peak = highPassPeak;
            editor->highPassADSRVisualizer->decay = highPassDecay;
            editor->highPassADSRVisualizer->decayCurve = highPassDecayCurve;
            editor->highPassADSRVisualizer->sustain = highPassSustain;
            editor->highPassADSRVisualizer->release = highPassRelease;
            editor->highPassADSRVisualizer->releaseCurve = highPassReleaseCurve;
            */
            //}
        editor->loadParameters(
            crossOverFequency,
            crossOverWidth,
            highPassOrder,
            lowPassOrder,
            lowPassInitial,
            lowPassAttack,
            lowPassAttackCurve,
            lowPassPeak,
            lowPassDecay,
            lowPassDecayCurve,
            lowPassSustain,
            lowPassRelease,
            lowPassReleaseCurve,
            highPassInitial,
            highPassAttack,
            highPassAttackCurve,
            highPassPeak,
            highPassDecay,
            highPassDecayCurve,
            highPassSustain,
            highPassRelease,
            highPassReleaseCurve
        );

        parametersChanged = false;
    }
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

    float sampleDuration = 1.0f / (float)sampleRate;
    //lowPassADSR.sampleDuration = sampleDuration;
    //highPassADSR.sampleDuration = sampleDuration;

    lowPassADSR.sampleDuration = sampleDuration;
    highPassADSR.sampleDuration = sampleDuration;

    std::cout << sampleDuration << "  ::  " << sampleRate << '\n';
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

void BreaQAudioProcessor::processBlock(
    juce::AudioBuffer<float>& audioBuffer,
    juce::MidiBuffer& midiBuffer) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        audioBuffer.clear(i, 0, audioBuffer.getNumSamples());

    /*
    // ToDo: Move These ####################################################
    // Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    crossOverFequency = crossOverFrequencyParameter->load();
    const auto crossOverWidth = crossOverWidthParameter->load();
    const auto highPassOrder = (int)highPassOrderParameter->load();
    const auto lowPassOrder = (int)lowPassOrderParameter->load();

    highPassFilter.f_order = highPassOrder + 1;
    lowPassFilter.f_order = lowPassOrder + 1;

    lowPassFrequency = crossOverFequency + crossOverWidth;
    highPassFrequency = crossOverFequency - crossOverWidth;

    lowPassFrequency = std::clamp(lowPassFrequency, 20.0f, 20000.0f);
    highPassFrequency = std::clamp(highPassFrequency, 20.0f, 20000.0f);

    lowPassFilter.cutOffFrequency = lowPassFrequency;
    highPassFilter.cutOffFrequency = highPassFrequency;

    // Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    const auto lowPassInitial = lowPassInitialParameter->load();
    const auto lowPassAttack = lowPassAttackParameter->load();
    const auto lowPassAttackCurve = lowPassAttackCurveParameter->load();
    const auto lowPassPeak = lowPassPeakParameter->load();
    const auto lowPassDecay = lowPassDecayParameter->load();
    const auto lowPassDecayCurve = lowPassDecayCurveParameter->load();
    const auto lowPassSustain = lowPassSustainParameter->load();
    const auto lowPassRelease = lowPassReleaseParameter->load();
    const auto lowPassReleaseCurve = lowPassReleaseCurveParameter->load();

    lowPassADSR.initialLevel = lowPassInitial;
    lowPassADSR.attackTime = lowPassAttack;
    lowPassADSR.attackCurve = lowPassAttackCurve;
    lowPassADSR.peakLevel = lowPassPeak;
    lowPassADSR.decayTime = lowPassDecay;
    lowPassADSR.decayCurve = lowPassDecayCurve;
    lowPassADSR.sustainLevel = lowPassSustain;
    lowPassADSR.releaseTime = lowPassRelease;
    lowPassADSR.releaseCurve = lowPassReleaseCurve;

    const auto highPassInitial = highPassInitialParameter->load();
    const auto highPassAttack = highPassAttackParameter->load();
    const auto highPassAttackCurve = highPassAttackCurveParameter->load();
    const auto highPassPeak = highPassPeakParameter->load();
    const auto highPassDecay = highPassDecayParameter->load();
    const auto highPassDecayCurve = highPassDecayCurveParameter->load();
    const auto highPassSustain = highPassSustainParameter->load();
    const auto highPassRelease = highPassReleaseParameter->load();
    const auto highPassReleaseCurve = highPassReleaseCurveParameter->load();

    highPassADSR.initialLevel = highPassInitial;
    highPassADSR.attackTime = highPassAttack;
    highPassADSR.attackCurve = highPassAttackCurve;
    highPassADSR.peakLevel = highPassPeak;
    highPassADSR.decayTime = highPassDecay;
    highPassADSR.decayCurve = highPassDecayCurve;
    highPassADSR.sustainLevel = highPassSustain;
    highPassADSR.releaseTime = highPassRelease;
    highPassADSR.releaseCurve = highPassReleaseCurve;

    //if (editor != nullptr) {
    /*
        editor->lowPassADSRVisualizer->initial = lowPassInitial;
        editor->lowPassADSRVisualizer->attack = lowPassAttack;
        editor->lowPassADSRVisualizer->attackCurve = lowPassAttackCurve;
        editor->lowPassADSRVisualizer->peak = lowPassPeak;
        editor->lowPassADSRVisualizer->decay = lowPassDecay;
        editor->lowPassADSRVisualizer->decayCurve = lowPassDecayCurve;
        editor->lowPassADSRVisualizer->sustain = lowPassSustain;
        editor->lowPassADSRVisualizer->release = lowPassRelease;
        editor->lowPassADSRVisualizer->releaseCurve = lowPassReleaseCurve;

        editor->highPassADSRVisualizer->initial = highPassInitial;
        editor->highPassADSRVisualizer->attack = highPassAttack;
        editor->highPassADSRVisualizer->attackCurve = highPassAttackCurve;
        editor->highPassADSRVisualizer->peak = highPassPeak;
        editor->highPassADSRVisualizer->decay = highPassDecay;
        editor->highPassADSRVisualizer->decayCurve = highPassDecayCurve;
        editor->highPassADSRVisualizer->sustain = highPassSustain;
        editor->highPassADSRVisualizer->release = highPassRelease;
        editor->highPassADSRVisualizer->releaseCurve = highPassReleaseCurve;
        */
    //}

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    auto leftInputChannel = audioBuffer.getReadPointer(0);
    auto rightInputChannel = audioBuffer.getReadPointer(1);

    int bufferSize = audioBuffer.getNumSamples();
    float* leftHighPassOutputChannel = new float[bufferSize];
    float* rightHighPassOutputChannel = new float[bufferSize];
    float* leftLowPassOutputChannel = new float[bufferSize];
    float* rightLowPassOutputChannel = new float[bufferSize];

    for (int i = 0; i < bufferSize; i++) {
        leftLowPassOutputChannel[i] = leftInputChannel[i];
        rightLowPassOutputChannel[i] = rightInputChannel[i];
        leftHighPassOutputChannel[i] = leftInputChannel[i];
        rightHighPassOutputChannel[i] = rightInputChannel[i];
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    lowPassFilter.processBlock(
        leftLowPassOutputChannel,
        rightLowPassOutputChannel,
        bufferSize
    );

    lowPassADSR.processBlock(
        midiBuffer,
        leftLowPassOutputChannel,
        rightLowPassOutputChannel,
        bufferSize
    );

    highPassFilter.processBlock(
        leftHighPassOutputChannel,
        rightHighPassOutputChannel,
        bufferSize
    );

    highPassADSR.processBlock(
        midiBuffer,
        leftHighPassOutputChannel,
        rightHighPassOutputChannel,
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

    spectrumAnalyzer.processBuffer(leftOutputChannel, bufferSize);

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
    editor = new BreaQAudioProcessorEditor(*this, parameters, spectrumAnalyzer);

    /*parameters.getParameter("lowPassInitial")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassAttack")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassAttackCurve")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassPeak")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassDecay")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassDecayCurve")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassSustain")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassRelease")->addListener(editor->lowPassADSRVisualizer);
    parameters.getParameter("lowPassReleaseCurve")->addListener(editor->lowPassADSRVisualizer);

    parameters.getParameter("highPassInitial")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassAttack")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassAttackCurve")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassPeak")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassDecay")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassDecayCurve")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassSustain")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassRelease")->addListener(editor->highPassADSRVisualizer);
    parameters.getParameter("highPassReleaseCurve")->addListener(editor->highPassADSRVisualizer);*/

    return editor;
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

    // Cross Over Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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

    // Low Pass Envelope Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassInitial", "Initial", juce::NormalisableRange{
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassAttack", "Attack", juce::NormalisableRange{
            0.0f, 0.5f, 0.000001f, 0.2f, false
        },
        0.5f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassPeak", "Peak", juce::NormalisableRange{
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassDecay", "Decay", juce::NormalisableRange{
            0.0f, 1.0f, 0.000001f, 0.2f, false
        },
        0.5f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassSustain", "Sustain", juce::NormalisableRange{
            0.0f, 1.0f, 0.1f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassRelease", "Release", juce::NormalisableRange{
            0.0f, 1.0f, 0.000001f, 0.2f, false
        },
        0.5f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassAttackCurve", "Attack Curve", juce::NormalisableRange{
            0.0f, 2.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassDecayCurve", "Decay Curve", juce::NormalisableRange{
            0.0f, 2.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "lowPassReleaseCurve", "Release Curve", juce::NormalisableRange{
            0.0f, 2.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    // High Pass Envelope Controls %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassInitial", "Initial", juce::NormalisableRange{
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassAttack", "Attack", juce::NormalisableRange{
            0.0f, 0.5f, 0.000001f, 0.2f, false
        },
        0.5f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassPeak", "Peak", juce::NormalisableRange{
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassDecay", "Decay", juce::NormalisableRange{
            0.0f, 1.0f, 0.000001f, 0.2f, false
        },
        0.5f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassSustain", "Sustain", juce::NormalisableRange{
            0.0f, 1.0f, 0.1f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassRelease", "Release", juce::NormalisableRange{
            0.0f, 1.0f, 0.000001f, 0.2f, false
        },
        0.5f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassAttackCurve", "Attack Curve", juce::NormalisableRange{
            0.0f, 2.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassDecayCurve", "Decay Curve", juce::NormalisableRange{
            0.0f, 2.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "highPassReleaseCurve", "Release Curve", juce::NormalisableRange{
            0.0f, 2.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BreaQAudioProcessor();
}
