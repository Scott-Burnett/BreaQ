#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterNames.h"

//==============================================================================
Slice::Slice() {
    probability = 0.0f;
    length = 0;
    progress = -1;
    sliceId = 0;
    enabled = false;
    isOn = false;
}

//==============================================================================
Slice::~Slice() {

}

//==============================================================================
void Slice::setSliceId(int id) {
    sliceId = id;
}

//==============================================================================
void Slice::init (
    juce::AudioProcessorValueTreeState& vts, 
    juce::AudioProcessorParameter::Listener& listener
) {
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
        ParameterOptions::lengthOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::sliceEnabled[sliceId], 
        "Enabled", 
        ParameterOptions::toggleButtonOptions,
        0
    ));
}

//==============================================================================
void Slice::loadParameters() {
    probability = probabilityParameter->load();
    length = lengthParameter->load();
    enabled = (bool) enabledParameter->load();
}

//==============================================================================
Strip::Strip() {
    probability = 0.0f;
    group = 0;
    noteNumber = 0;
    stripId = 0;
    enabled = false;
    bypassed = false;
    isOn = false;

    currentSlice = nullptr;

    slices = new Slice[NUM_SLICES];
}

//==============================================================================
Strip::~Strip() {

}

//==============================================================================
void Strip::setStrpId(int id) {
    stripId = id;
    noteNumber = 60 + id;

    // slices = new Slice[NUM_SLICES];

     for (int i = 0, offset = stripId * NUM_SLICES; i < NUM_SLICES; i++) {
         slices[i].setSliceId(i + offset);
     }
}

//==============================================================================
void Strip::init (
    juce::AudioProcessorValueTreeState& vts, 
    juce::AudioProcessorParameter::Listener& listener
) {
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

    for (int i = 0, offset = stripId * NUM_SLICES; i < NUM_SLICES; i++) {
        slices[i].init(vts, listener);
    }
}

//==============================================================================
void Strip::createParameterLayout (
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
        ParameterOptions::groupOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripEnabled[stripId], 
        "Enabled", 
        ParameterOptions::toggleButtonOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripBypassed[stripId], 
        "Bypass", 
        ParameterOptions::toggleButtonOptions,
        0
    ));

    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].createParameterLayout(layout);
    }
}

//==============================================================================
void Strip::loadParameters() {
    probability = probabilityParameter->load();
    group = (int) groupParameter->load();
    enabled = (bool) enabledParameter->load();
    bypassed = (bool) bypassedParameter->load();

    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].loadParameters();
    }
}

//==============================================================================
void Group::setGroupId(int id) {
    id = id;
}

//==============================================================================
void Group::init(
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorParameter::Listener& listener
) {
    enabled = true;
    currentStrip = nullptr;
}

//==============================================================================
void Group::createParameterLayout(
    juce::AudioProcessorValueTreeState::ParameterLayout& layout
) {
}

//==============================================================================
void Group::loadParameters() {
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
    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].init(parameters, *this);
    }

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].init(parameters, *this);
    }

    needsRepaint = false;

    random = juce::Random::getSystemRandom();

    startTimer(30);
}

//==============================================================================
BreaQAudioProcessor::~BreaQAudioProcessor() {

}

//==============================================================================
void BreaQAudioProcessor::parameterValueChanged(int parameterIndex, float newValue) {
    needsRepaint = true;
}

//==============================================================================
void BreaQAudioProcessor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {

}

//==============================================================================
void BreaQAudioProcessor::timerCallback() {
    if (needsRepaint) {
        for (int i = 0; i < NUM_GROUPS; i++) {
            groups[i].loadParameters();
        }
        for (int i = 0; i < NUM_STRIPS; i++) {
            strips[i].loadParameters();
        }

        // TODO: Load Editor State
        // GroupDto groupDtos[NUM_GROUPS];

        // StripDto stripDtos[NUM_STRIPS];
        StripDto* stripDto = nullptr;
        Strip* strip = nullptr;
        SliceDto* sliceDto = nullptr;
        Slice* slice = nullptr;

        for (int i = 0; i < NUM_STRIPS; i++) {
            stripDto = &stripDtos[i];
            strip = &strips[i];

            stripDto->isOn = strip->isOn;
            stripDto->isEnabled = strip->enabled;
            stripDto->isBypassed = strip->bypassed;
            stripDto->group = strip->group;

            for (int j = 0; j < NUM_SLICES; j++) {
                sliceDto = &stripDto->sliceDtos[j];
                slice = &strip->slices[j];

                sliceDto->isEnabled = slice->enabled;
                sliceDto->isOn = slice->isOn;
                sliceDto->length = slice->length;
                sliceDto->progress = slice->progress;
            }
        }

        editor->LoadState(groupDtos, stripDtos);

        needsRepaint = false;
    }
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
    // ???
    juce::ScopedNoDenormals noDenormals;

    // Clear channels
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        audioBuffer.clear(i, 0, audioBuffer.getNumSamples());
    }

    processedBuffer.clear();

    // Ready to go
    juce::MidiBuffer::Iterator iterator(midiBuffer);
    juce::MidiMessage current;
    int samplePos;

    Group *group = nullptr;
    Strip *strip = nullptr;
    Strip *currentStrip = nullptr;
    Slice *slice = nullptr;
    Slice *currentSlice = nullptr;
    
    int numWorkingStrips = 0;
    Strip *workingStrips[NUM_STRIPS];

    int numWorkingSlices = 0;
    Slice *workingSlices[NUM_SLICES];

    float maxProbability = 0;
    /*juce::Random random = juce::Random::getSystemRandom();*/

    while (iterator.getNextEvent(current, samplePos)) {
        if (!current.isNoteOn()) {
            continue;
        }

        for (int g = 0; g < NUM_GROUPS; g++) {
            group = &groups[g]; 

            strip = currentStrip = 
                group->currentStrip;

            slice = currentSlice = 
                currentStrip != nullptr
                ? currentStrip->currentSlice
                : nullptr;

            if (!group->enabled) {
                // If note is currently on, End it
                if (currentStrip != nullptr &&
                    currentSlice != nullptr &&
                    currentSlice->isOn) {
                        // TODO: Move to method
                        currentStrip->isOn = false;
                        currentSlice->isOn = false;
                        currentSlice->progress = -1;
                        processedBuffer.addEvent(juce::MidiMessage::noteOff(1, currentStrip->noteNumber), samplePos);

                        group->currentStrip = nullptr;
                }

                continue;
            }

            // Group Is Enabled

            if (currentSlice != nullptr &&
                !currentSlice->enabled) {
                    currentStrip->isOn = false;
                    currentSlice->isOn = false;
                    processedBuffer.addEvent(juce::MidiMessage::noteOff(1, currentStrip->noteNumber), samplePos);

                    group->currentStrip = nullptr;

                    continue;
            }

            if (currentSlice != nullptr && 
                currentSlice->progress++ < currentSlice->length) {
                // Current Slice Not Complete, progress incremented, now break
                continue;
            }

            // Slice is complete: Choose new Strip For Group
            maxProbability = 0.0f;
            numWorkingStrips = 0;
            for (int st = 0; st < NUM_STRIPS; st++) {
                strip = &strips[st];

                if (!strip->enabled ||
                    strip->probability == 0.0f ||
                    strip->group != g) {
                    // Strip not enabled, in group or probability is zero, not a candidate
                    continue;
                }

                bool noSlicesEnabled = true;
                for (int sl = 0; sl < NUM_SLICES; sl++) {
                    slice = &strip->slices[sl];
                    if (slice->enabled &&
                        slice->probability > 0.0f) {
                        // At least one slice is a candidate
                        noSlicesEnabled = false;
                        break;
                    }
                }

                if (noSlicesEnabled) {
                    // No slices enabled for this strip, not a candidate
                    continue;
                }

                workingStrips[numWorkingStrips++] = strip;
                maxProbability += strip->probability; 
            }

            if (numWorkingStrips == 0) {
                group->currentStrip = nullptr;
                continue;
            }
            
            float r = random.nextFloat() * maxProbability;
            float p = 0.0f;

            for (int wst = 0; wst < numWorkingStrips; wst++) {
                strip = workingStrips[wst];
                p += strip->probability;

                if (r <= p) {
                    break;
                }
            }

            // Choose new Slice for Strip
            maxProbability = 0.0f;
            numWorkingSlices = 0;
            for (int sl = 0; sl < NUM_SLICES; sl++) {
                slice = &strip->slices[sl];

                if (!slice->enabled ||
                    slice->probability == 0.0f) {
                    continue;
                }

                workingSlices[numWorkingSlices++] = slice;
                maxProbability += slice->probability;
            }

            // ToDo: Find a better way to handle No new slice found, currently just set to nullptr and check later
            slice = nullptr;

            r = random.nextFloat() * maxProbability;
            p = 0.0f;
            for (int wsl = 0; wsl < numWorkingSlices; wsl++) {
                slice = workingSlices[wsl];
                p += slice->probability;

                if (r <= p) {
                    break;
                }
            }

            // TODO: Legato option here?

            // -> Set current Note OFF
            if (currentStrip != nullptr) {
                currentStrip->isOn = false;
            }

            if (currentSlice != nullptr) {
                currentSlice->isOn = false;
                currentSlice->progress = -1;
                
                // TODO: UGLY
                // Add Note Off Event for current note
                processedBuffer.addEvent(juce::MidiMessage::noteOff(1, currentStrip->noteNumber), samplePos);
            }

            // -> POINT Current to new
            if (slice == nullptr) {
                // no new slice
                continue;
            }
            currentSlice = slice;

            currentStrip = strip;
            currentStrip->currentSlice = currentSlice;

            group->currentStrip = currentStrip;
            
            // -> SET Current Slice ON
            currentSlice->isOn = true;
            currentStrip->isOn = true;

            currentSlice->progress = 0;

            // Add Note On Event for current Note
            processedBuffer.addEvent(juce::MidiMessage::noteOn(1, currentStrip->noteNumber, (juce::uint8) 127), samplePos);

        } // ForEach Group

        needsRepaint = true;

    } // MidiMessage Iterator 

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

    groups = new Group[NUM_GROUPS];
    groupDtos = new GroupDto[NUM_GROUPS];
    strips = new Strip[NUM_STRIPS];
    stripDtos = new StripDto[NUM_STRIPS];

    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].setGroupId(i);
        groups[i].createParameterLayout(layout);
    }

    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].setStrpId(i);
        strips[i].createParameterLayout(layout);

        //stripDtos[i]->sliceDtos = new SliceDto[NUM_SLICES];
    }

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BreaQAudioProcessor();
}
