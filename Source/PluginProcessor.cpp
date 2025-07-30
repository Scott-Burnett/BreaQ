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
bool Slice::isPreparedToPlay() {
    return
        enabled &&
        probability > 0.0f;
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

    lock = Lock(&group);

    currentSlice = nullptr;

    slices = new Slice[NUM_SLICES];
}

//==============================================================================
Strip::~Strip() {

}

//==============================================================================
bool Strip::isPreparedToPlay() {
    if (!enabled ||
        probability == 0.0f) {
        return false;
    }
    // enabled && probability > 0.0f
    for (int i = 0; i < NUM_SLICES; i++) {
        if (slices[i].isPreparedToPlay()) {
            // at least one slice is prepared to play
            return true;
        }
    }

    return false;
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

    for (int i = 0; i < NUM_SLICES; i++) {
        slices[i].loadParameters();
    }
}

//==============================================================================
Step::Step() {
    hasValue = false;

    noteNumber = 0;
    channel = 1;
    velocity = (juce::uint8) 127;

    strip = nullptr;
    slice = nullptr;
}

//==============================================================================
Step::~Step() {

}

//==============================================================================
void Step::clear() {
    hasValue = false;
    strip = nullptr;
    slice = nullptr;
}

//==============================================================================
void Step::addNoteOnEvent(juce::MidiBuffer& midiBuffer, int samplePos) {
    midiBuffer.addEvent (
        juce::MidiMessage::noteOn (
            channel, noteNumber, velocity
        ),
        samplePos
    );
    // Todo: set strip & slice on
}

//==============================================================================
void Step::addNoteOffEvent(juce::MidiBuffer& midiBuffer, int samplePos) {
    midiBuffer.addEvent (
        juce::MidiMessage::noteOff (
            channel, noteNumber
        ),
        samplePos
    );
    // Todo: set strip & slice off
}

//==============================================================================
Step Step::loadFrom(Strip *strip, Slice *slice) {
    int noteNumber = 60 + strip->choice + (strip->stripId * NUM_CHOICES);
    int channel = 1;
    juce::uint8 velocity = (juce::uint8) 127;

    Step step;
    step.noteNumber = noteNumber;
    step.channel = channel;
    step.velocity = velocity;
    step.strip = strip;
    step.slice = slice;

    step.hasValue = true;

    return step;
}

//==============================================================================
bool Step::differs(Step *first, Step *second) {
    if (first == nullptr) {
        return second == nullptr;
    }

    if (second == nullptr) {
        return false;
    }

    return // probably overboard
        first->hasValue == second->hasValue &&
        first->channel == second->channel &&
        first->noteNumber == second->noteNumber &&
        first->velocity == second->velocity &&
        first->strip == second->strip &&
        first->slice == second->slice
    ;
}

//==============================================================================
Group::Group() {
    id = -1;
    length = 0;
    plusSixteen = 0;
    progress = -1;
    plusSixteenProgress = -1l;
    enabled = true;
    isOn = false;

    currentStrip = nullptr;

    numSteps = 0;
    step = 0;
    sequence = new Step[MAX_STEPS];
}

//==============================================================================
void Group::createSequence(
    Strip* strips,
    juce::Random random
) {
    // release all locks
    for (int i = 0; i < this->numSteps; i++) {
        if (sequence[i].hasValue &&
            sequence[i].strip->lock.hasLock(this->id)) {
            sequence[i].strip->lock.unLock(this->id);
        }
    }

    this->numSteps = length + plusSixteen * 16;

    int s = 0;
    while (s < numSteps) {
        Strip* strip = choose (
            random,
            strips,
            [this] (Strip* strip) -> bool {
                return
                    strip->isPreparedToPlay() &&
                    strip->lock.canLock(this->id);
            }
        );

        if (strip == nullptr ||
            !strip->lock.tryLock(this->id)) {
            goto clearToEnd;
        }

        Slice* slice = choose (
            random,
            strip->slices,
            [] (Slice* slice) -> bool {
                return
                    slice->isPreparedToPlay();
            }
        );
        // Should always be at least 1 slice here
        int length = slice->length + slice->plusSixteen * 16;
        Step next = Step::loadFrom(strip, slice);

        for (
            int i = 0; 
            i < length && s < numSteps; 
            i++, s++ // Not sure if this skips a step?
        ) {
            sequence[s] = next;
        }

        // back to top ..
    }

clearToEnd:
    while (s < MAX_STEPS) {
        sequence[s++].clear();
    }
}

//==============================================================================
void Group::takeStep(juce::MidiBuffer& midiBuffer, int samplePos, Strip* strips, juce::Random random) {
    Step* currentStep = isOn
        ? &sequence[step]
        : nullptr
    ;

    if (++step >= numSteps
        /*ToDo: and not looping*/) {
        // todo: Create sequence here
        createSequence(strips, random);
        step = 0;
    }
    
    Step *newStep = 
        enabled &&
        sequence[step].hasValue &&
        sequence[step].strip->enabled
        ? &sequence[step]
        : nullptr
    ;

    newNote(currentStep, newStep, midiBuffer, samplePos);
}

//==============================================================================
void Group::newNote(Step *currentStep, Step *newStep, juce::MidiBuffer& midiBuffer, int samplePos) {
    if (Step::differs(currentStep, newStep)) {
        if (currentStep != nullptr &&
            currentStep->hasValue) {
            currentStep->addNoteOffEvent(midiBuffer, samplePos);
            // TODO: Slice & Strip isOn
            isOn = false;
        }

        if (newStep != nullptr &&
            newStep->hasValue) {
            newStep->addNoteOffEvent(midiBuffer, samplePos);
            // Here too
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
            groups[g].takeStep(processedBuffer, samplePos, strips, random);
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
