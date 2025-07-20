#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterNames.h"

//==============================================================================
static Slice* choose (
    juce::Random random,
    Slice* items,
    std::function<bool(Slice*)> predicate
) {
    int candidates = 0;
    int lookup[NUM_SLICES];
    float maxProbability = 0.0f;
    for (int i = 0; i < NUM_SLICES; i++) {
        if (predicate(&items[i])) {
            lookup[candidates++] = i;
            maxProbability += items[i].probability;
        }
    }

    float r = random.nextFloat() * maxProbability;
    float p = 0.0f;
    for (int i = 0; i < candidates; i++) {
        p += items[lookup[i]].probability;
        if (p >= r) {
            return &items[lookup[i]];
        }
    }

    return nullptr;
}

//==============================================================================
static Strip* choose (
    juce::Random random,
    Strip* items,
    std::function<bool(Strip*)> predicate
) {
    int candidates = 0;
    int lookup[NUM_STRIPS];
    float maxProbability = 0.0f;
    for (int i = 0; i < NUM_STRIPS; i++) {
        if (predicate(&items[i])) {
            lookup[candidates++] = i;
            maxProbability += items[i].probability;
        }
    }

    float r = random.nextFloat() * maxProbability;
    float p = 0.0f;
    for (int i = 0; i < candidates; i++) {
        p += items[lookup[i]].probability;
        if (p >= r) {
            return &items[lookup[i]];
        }
    }

    return nullptr;
}

//==============================================================================
Slice::Slice() {
    probability = 0.0f;
    length = 0;
    progress = -1;
    plusSixteenProgress = -1l;
    plusSixteen = 0;
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
    plusSixteenParameter = vts.getRawParameterValue(
        ParameterNames::slicePlusSixteen[sliceId]
    );
    enabledParameter = vts.getRawParameterValue(
        ParameterNames::sliceEnabled[sliceId]
    );

    vts.getParameter(ParameterNames::sliceProbability[sliceId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::sliceLength[sliceId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::slicePlusSixteen[sliceId])->
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
        ParameterNames::slicePlusSixteen[sliceId], 
        "plusSixteen", 
        ParameterOptions::plusSixteenOptions,
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
    plusSixteen = plusSixteenParameter->load();
    enabled = (bool) enabledParameter->load();
}

//==============================================================================
Strip::Strip() {
    probability = 0.0f;
    group = 0;
    choice = 0;
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
    choiceParameter = vts.getRawParameterValue(
        ParameterNames::stripChoice[stripId]
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
    vts.getParameter(ParameterNames::stripChoice[stripId])->
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

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::stripChoice[stripId],
        "Choice",
        ParameterOptions::choiceOptions,
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
    choice = choiceParameter->load();
    enabled = (bool) enabledParameter->load();
    bypassed = (bool) bypassedParameter->load();

    // TODO: Set Note based on choice, or schedule refresh

    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].loadParameters();
    }
}

//==============================================================================
Step::Step() {
    stripId = -1;
    sliceId = -1;
    noteNumber = 0;
    channel = 1;
    velocity = (juce::uint8) 127;

    hasValue = false;
}

//==============================================================================
Step::~Step() {

}

//==============================================================================
void Step::loadFrom(Strip *strip, Slice *slice) {
    // TODO: Dont do this every the time
    int noteNumber = 60 + strip->choice + (strip->stripId * NUM_CHOICES);
    int channel = 1;
    juce::uint8 velocity = (juce::uint8) 127;

    noteNumber = noteNumber;
    channel = channel;
    velocity = velocity;
    stripId = strip->stripId;
    sliceId = slice->sliceId;

    hasValue = true;
}

//==============================================================================
Group::Group() {
    id = -1;
    // bool shed;
    length = 0;
    plusSixteen = 0;
    progress = -1;
    plusSixteenProgress = -1l;
    enabled = true;
    isOn = false;

    currentStrip = nullptr;

    numSteps = 0;
    sequence = new Step[MAX_STEPS];
    currentStep = nullptr;
}

//==============================================================================
void Group::createSequence(
    Strip* strips, 
    juce::Random random
) {
    this->numSteps = length + plusSixteen * 16;

    int step = 0;
    while (step < numSteps) {
        Strip* strip = choose (
            random,
            strips,
            [this] (Strip* strip) -> bool {
                return
                    // TODO: Locking Mechanism
                    strip->group == this->id &&
                    strip->enabled &&
                    strip->probability > 0.0f;
            }
        );

        if (strip == nullptr) {
            goto FillToEndWithEmpty;
        }

        Slice* slice = choose (
            random,
            strip->slices,
            [] (Slice* slice) -> bool {
                return
                    slice->enabled &&
                    slice->probability > 0.0f;
            }
        );

        if (slice == nullptr) {
            goto FillToEndWithEmpty;
        }

        int length = slice->length + slice->plusSixteen * 16;

        for (
            int i = 0; 
            i < length && step < numSteps; 
            i++, step++ // Not sure if this skips a step?
        ) {
            sequence[step].loadFrom(strip, slice);
        }
    }

FillToEndWithEmpty:
    while (step < MAX_STEPS) {
        sequence[step++].hasValue = false;
    }
}

//==============================================================================
Group::~Group() {

}

//==============================================================================
void Group::setGroupId(int id) {
    this->id = id;
}

//==============================================================================
void Group::init(
    juce::AudioProcessorValueTreeState& vts,
    juce::AudioProcessorParameter::Listener& listener
) {
    enabled = true;
    currentStrip = nullptr; 
    
    lengthParameter = vts.getRawParameterValue(
        ParameterNames::groupLength[id]
    );
    plusSixteenParameter = vts.getRawParameterValue(
        ParameterNames::groupPlusSixteen[id]
    );
    enabledParameter = vts.getRawParameterValue(
        ParameterNames::groupEnabled[id]
    );

    vts.getParameter(ParameterNames::groupLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupPlusSixteen[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupEnabled[id])->
        addListener(&listener);
}

//==============================================================================
void Group::createParameterLayout(
    juce::AudioProcessorValueTreeState::ParameterLayout& layout
) {
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupLength[id],
        "Length",
        ParameterOptions::lengthOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupPlusSixteen[id],
        "plusSixteen",
        ParameterOptions::plusSixteenOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupEnabled[id],
        "Enabled",
        ParameterOptions::toggleButtonOptions,
        0
    ));
}

//==============================================================================
void Group::loadParameters() {
    length = lengthParameter->load();
    plusSixteen = plusSixteenParameter->load();
    enabled = (bool) enabledParameter->load();
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
    
    random = juce::Random::getSystemRandom();
    
    needsRepaint = false;
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
            stripDto->choice = strip->choice;

            for (int j = 0; j < NUM_SLICES; j++) {
                sliceDto = &stripDto->sliceDtos[j];
                slice = &strip->slices[j];

                sliceDto->isEnabled = slice->enabled;
                sliceDto->isOn = slice->isOn;
                sliceDto->length = slice->length;
                sliceDto->plusSixteen = slice->plusSixteen;
                sliceDto->progress = slice->progress;
                sliceDto->plusSixteenProgress = slice->plusSixteenProgress;
            }
        }

        if (editor != nullptr) {
            editor->LoadState(groupDtos, stripDtos);
        }

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
                        currentSlice->plusSixteenProgress = -1;
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
                    currentSlice->progress = -1;
                    currentSlice->plusSixteenProgress = -1;
                    processedBuffer.addEvent(juce::MidiMessage::noteOff(1, currentStrip->noteNumber), samplePos);

                    group->currentStrip = nullptr;

                    continue;
            }

            if (currentSlice != nullptr &&
                currentSlice->plusSixteen > 0 &&
                currentSlice->plusSixteenProgress < (long) currentSlice->plusSixteen * 16l) {
                // +sixteen not accounted for, here increment inside block so as to not overshoot, then break
                currentSlice->plusSixteenProgress++;
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

                bool atLeastOneCandidate = false;
                for (int sl = 0; sl < NUM_SLICES; sl++) {
                    slice = &strip->slices[sl];
                    if (slice->enabled &&
                        slice->probability > 0.0f) {
                        // At least one slice is a candidate
                        atLeastOneCandidate = true;
                        break;
                    }
                }

                if (!atLeastOneCandidate) {
                    // No slices enabled for this strip, not a candidate
                    continue;
                }

                workingStrips[numWorkingStrips++] = strip;
                maxProbability += strip->probability; 
            }

            if (numWorkingStrips == 0) {
                // No candidates for Strip, end current note and break
                if (currentStrip != nullptr && 
                    currentSlice != nullptr) {
                        currentStrip->isOn = false;
                        currentSlice->isOn = false;
                        currentSlice->progress = -1;
                        currentSlice->plusSixteenProgress = -1;

                        processedBuffer.addEvent(juce::MidiMessage::noteOff(1, currentStrip->noteNumber), samplePos);

                        group->currentStrip = nullptr;
                    }

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
                currentSlice->plusSixteenProgress = -1l;
                
                // TODO: UGLY
                // Add Note Off Event for current note
                processedBuffer.addEvent(juce::MidiMessage::noteOff(1, currentStrip->noteNumber), samplePos);
            }

            if (slice == nullptr) {
                // no new slice
                continue;
            }

            // -> POINT Current to new
            currentSlice = slice;

            currentStrip = strip;
            currentStrip->currentSlice = currentSlice;

            group->currentStrip = currentStrip;
            
            // -> SET Current Slice ON
            currentSlice->isOn = true;
            currentStrip->isOn = true;

            currentSlice->progress = 0;
            currentSlice->plusSixteenProgress = 0l;

            // Update Note numbe based on choice
            currentStrip->noteNumber = 60 + currentStrip->choice + (currentStrip->stripId * NUM_CHOICES);

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
    }

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BreaQAudioProcessor();
}
