#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterNames.h"

//==============================================================================
static float nextFloat() {
    return juce::Random::getSystemRandom().nextFloat();
}

//==============================================================================
static int nextInt(int lower, int upper) {
    return juce::Random::getSystemRandom().nextInt({lower, upper});
}

//==============================================================================
static Strip* choose (
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

    float r = nextFloat() * maxProbability;
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
Lock::Lock() {
    shape = nullptr;

    locked = false;
    holder = -1;
}

//==============================================================================
Lock::~Lock() {
}

//==============================================================================
bool Lock::isLocked() {
    return locked;
}

//==============================================================================
bool Lock::hasLock(int key) {
    return (
        locked &&
        holder == key
    );
}

//==============================================================================
bool Lock::canLock(int key) {
    return (
        locked &&
        holder == key
    ) || (
        !locked &&
        *shape == key
    );
}

//==============================================================================
bool Lock::tryLock(int key) {
    if (locked &&
        holder == key) {
        return true;
    }

    if (!locked &&
        *shape == key) {
        locked = true;
        holder = key;

        if (onLockAquired) {
            onLockAquired();
        }

        return true;
    }

    return false;
}

//==============================================================================
void Lock::unLock(int key) {
    locked = false;
    holder = -1;
}

//==============================================================================
Lock::Lock(int* key) {
    shape = key;

    locked = false;
    holder = -1;
}

//==============================================================================
Lock::Lock(int* key, std::function<void()> onLockAquired) {
    shape = key;

    locked = false;
    holder = -1;

    this->onLockAquired = onLockAquired;
}

//==============================================================================
Strip::Strip() {
    probability = 0.0f;
    group = 0;
    choice = 0;
    // noteNumber = 0;
    stripId = 0;
    enabled = false;
    isOn = false;

    lock = Lock(&group);
}

//==============================================================================
Strip::~Strip() {

}

//==============================================================================
bool Strip::isPreparedToPlay() {
    return 
        enabled &&
        probability > 0.0f;
}

//==============================================================================
void Strip::setStrpId(int id) {
    stripId = id;
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
    chokeParameter = vts.getRawParameterValue(
        ParameterNames::stripChoke[stripId]
    );
    variantsParameter = vts.getRawParameterValue(
        ParameterNames::stripVariants[stripId]
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
    vts.getParameter(ParameterNames::stripChoke[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripVariants[stripId])->
        addListener(&listener);
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
    
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::stripChoke[stripId], 
        "Choke", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripVariants[stripId], 
        "Variants", 
        ParameterOptions::variantsOptions,
        0
    ));
}

//==============================================================================
void Strip::loadParameters() {
    probability = probabilityParameter->load();
    group = (int) groupParameter->load();
    choice = choiceParameter->load();
    enabled = (bool) enabledParameter->load();
    choke = chokeParameter->load();
    variants = (int) variantsParameter->load();
}

//==============================================================================
Step::Step() {
    hasValue = false;

    noteNumber = 0;
    channel = 1;
    velocity = (juce::uint8) 127;

    strip = nullptr;
}

//==============================================================================
Step::~Step() {

}

//==============================================================================
void Step::clear() {
    hasValue = false;
    strip = nullptr;
}

//==============================================================================
void Step::addNoteOnEvent(juce::MidiBuffer& midiBuffer, int samplePos) {
    midiBuffer.addEvent (
        juce::MidiMessage::noteOn (
            channel, noteNumber, velocity
        ),
        samplePos
    );
    strip->isOn = true;
}

//==============================================================================
void Step::addNoteOffEvent(juce::MidiBuffer& midiBuffer, int samplePos) {
    midiBuffer.addEvent (
        juce::MidiMessage::noteOff (
            channel, noteNumber
        ),
        samplePos
    );
    strip->isOn = false;
}

//==============================================================================
Step Step::loadFrom(Strip *strip) {
    int noteNumber = 60 + strip->choice + (strip->stripId * NUM_CHOICES);
    int channel = 1;
    juce::uint8 velocity = (juce::uint8) nextInt(1, 127);

    Step step;
    step.noteNumber = noteNumber;
    step.channel = channel;
    step.velocity = velocity;
    step.strip = strip;

    step.hasValue = true;

    return step;
}

//==============================================================================
Step Step::empty() {
    Step step;
    step.hasValue = false;

    return step;
}

//==============================================================================
bool Step::differs(Step first, Step second) {
    if (!first.hasValue) {
        return second.hasValue;
    }

    if (!second.hasValue) {
        return true;
    }

    return // probably overboard
        first.channel != second.channel ||
        first.noteNumber != second.noteNumber ||
        first.velocity != second.velocity ||
        first.strip != second.strip
    ;
}

//==============================================================================
Group::Group() {
    id = -1;
    length = 0;
    plusSixteen = 0;
    enabled = true;
    loop = false;
    isOn = false;

    currentStrip = nullptr;

    numSteps = 0;
    step = 0;
    sequence = new Step[MAX_STEPS];

    sequenceLength = 0;
    sequenceLengthMultiplier = 0;

    intervalLength = 0;
    intervalLengthMultiplier = 0;

    tjopLength = 0;
    tjopLengthMultiplier = 0;

    density = 0.0f;

}

//==============================================================================
void Group::createSequence(
    Strip* strips
) {
    // release all locks
    for (int i = 0; i < this->numSteps; i++) {
        if (sequence[i].hasValue &&
            sequence[i].strip->lock.hasLock(this->id)) {
            sequence[i].strip->lock.unLock(this->id);
        }
    }

    this->numSteps = sequenceLength * sequenceLengthMultiplier;

    int s = 0;
    int i = 0;
    int t = 0;
    int sequenceSteps = sequenceLength * sequenceLengthMultiplier;
    int intervalSteps = intervalLength * intervalLengthMultiplier;
    int tjopSteps = tjopLength * tjopLengthMultiplier;

    // TODO: Variants Kak
    // Get all possible variants for each strip upfront
    // then each time we get a strip, we then get the variant before finally getting the step

    s = 0;
    while (
        s < sequenceSteps ) {

        i = 0;
        while (
            s < sequenceSteps &&
            i < intervalSteps) {
            // float d = juce::Random::getSystemRandom().nextFloat();

            Strip* strip = nextFloat() < density
                ? choose (
                    strips,
                    [this] (Strip* strip) -> bool {
                        return
                            strip->isPreparedToPlay() &&
                            strip->lock.canLock(this->id);
                    }
                )
                : nullptr
            ;

            Step next = 
                strip != nullptr &&
                strip->lock.tryLock(this->id)
                ? Step::loadFrom(strip)
                : Step::empty()
            ;

            int c = strip != nullptr
                ? (int) ((float) tjopSteps * strip->choke)
                : tjopSteps
            ;

            t = 0;
            while (
                s < sequenceSteps &&
                i < intervalSteps &&
                t < tjopSteps) {

                if (t > c) { // Choke
                    next = Step::empty();
                }

                sequence[s] = next;

                s++, i++, t++;
            } // Tjop Loop
        } // Interval Loop
    } // Sequence Loop
}

//==============================================================================
void Group::takeStep(juce::MidiBuffer& midiBuffer, int samplePos, Strip* strips) {
    Step currentStep = isOn
        ? sequence[step]
        : Step::empty()
    ;

    if (++step >= numSteps) {
        if (!loop) {
            createSequence(strips);
        }
        step = 0;
    }
    
    Step newStep = 
        enabled &&
        sequence[step].hasValue &&
        sequence[step].strip->enabled
        ? sequence[step]
        : Step::empty()
    ;

    newNote(currentStep, newStep, midiBuffer, samplePos);
}

//==============================================================================
void Group::newNote(Step currentStep, Step newStep, juce::MidiBuffer& midiBuffer, int samplePos) {
    if (Step::differs(currentStep, newStep)) {
        if (currentStep.hasValue) {
            currentStep.addNoteOffEvent(midiBuffer, samplePos);
            isOn = false;
        }

        if (newStep.hasValue) {
            newStep.addNoteOnEvent(midiBuffer, samplePos);
            isOn = true;
        }
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
    loopParameter = vts.getRawParameterValue(
        ParameterNames::groupLoop[id]
    );
    tjopLengthParameter = vts.getRawParameterValue(
        ParameterNames::groupTjopLength[id]
    );
    tjopLengthMultiplierParameter = vts.getRawParameterValue(
        ParameterNames::groupTjopLengthMultiplier[id]
    );
    intervalLengthParameter = vts.getRawParameterValue(
        ParameterNames::groupIntervalLength[id]
    );
    intervalLengthMultiplierParameter = vts.getRawParameterValue(
        ParameterNames::groupIntervalLengthMultiplier[id]
    );
    sequenceLengthParameter = vts.getRawParameterValue(
        ParameterNames::groupSequenceLength[id]
    );
    sequenceLengthMultiplierParameter = vts.getRawParameterValue(
        ParameterNames::groupSequenceLengthMultiplier[id]
    );
    densityParameter = vts.getRawParameterValue(
        ParameterNames::groupDensity[id]
    );

    vts.getParameter(ParameterNames::groupLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupPlusSixteen[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupEnabled[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupLoop[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupTjopLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupTjopLengthMultiplier[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupIntervalLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupIntervalLengthMultiplier[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupSequenceLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupSequenceLengthMultiplier[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupDensity[id])->
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

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupLoop[id],
        "Loop",
        ParameterOptions::toggleButtonOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupTjopLength[id],
        "TjopLength",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupTjopLengthMultiplier[id],
        "TjopLengthMultiplier",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupIntervalLength[id],
        "IntervalLength",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupIntervalLengthMultiplier[id],
        "IntervalLengthMultiplier",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupSequenceLength[id],
        "SequenceLength",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupSequenceLengthMultiplier[id],
        "SequenceLengthMultiplier",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::groupDensity[id], 
        "Density", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));
}

//==============================================================================
void Group::loadParameters() {
    length = lengthParameter->load() + 1;
    plusSixteen = plusSixteenParameter->load();
    enabled = (bool) enabledParameter->load();
    loop = (bool) loopParameter->load();

    density = densityParameter->load();

    tjopLength = tjopLengthParameter->load() + 1;
    tjopLengthMultiplier = tjopLengthMultiplierParameter->load() + 1;

    intervalLength = intervalLengthParameter->load() + 1;
    intervalLengthMultiplier = intervalLengthMultiplierParameter->load() + 1;

    sequenceLength = sequenceLengthParameter->load() + 1;
    sequenceLengthMultiplier = sequenceLengthMultiplierParameter->load() + 1;
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
    startTimer(15);
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

        if (editor != nullptr) {
            editor->LoadState(groups, strips);
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

    while (iterator.getNextEvent(current, samplePos)) {
        if (!current.isNoteOn()) {
            continue;
        }

        for (int g = 0; g < NUM_GROUPS; g++) {
            groups[g].takeStep(processedBuffer, samplePos, strips);
        }

        needsRepaint = true;
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

    groups = new Group[NUM_GROUPS];
    strips = new Strip[NUM_STRIPS];

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
