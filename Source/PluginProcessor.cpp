#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterNames.h"

//==============================================================================
std::uint64_t identifiers[NUM_GROUPS] = {
    0ULL,
    0ULL,
    0ULL,
    0ULL
};

//==============================================================================
static std::uint64_t nextIdentifier(int group) {
    // Yes its eventually gonna wrap around
    // Long is big enough, Maximum of 16 ^ 2 notes per sequence
    // The last one that got 0ULL will be long gone
    return identifiers[group]++;
}

//==============================================================================
static float nextFloat() {
    return juce::Random::getSystemRandom().nextFloat();
}

//==============================================================================
static int nextInt(int lower, int upper) {
    return juce::Random::getSystemRandom().nextInt({lower, upper});
}

//==============================================================================
static int clamp(int value, int lower, int upper) {
    if (value <= lower) {
        return lower;
    }

    if (value >= upper) {
        return upper;
    }

    return value;
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
static Gate* choose (
    Step * step,
    Gate* gates
) {
    // this sucks, Rather pass in the strip?
    if (!step->hasValue) {
        return nullptr;
    }

    // Get availableGates from Strip here
    int thing = step->strip->group;

    int choice = nextInt(0, NUM_GATES - 1);

    return &gates[choice];
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
    stripId = 0;
    enabled = false;
    isOn = false;

    // new
    gateIsOpen = false;

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
    repeatProbabilityParameter = vts.getRawParameterValue(
        ParameterNames::stripRepeatProbability[stripId]
    );
    repeatLengthParameter = vts.getRawParameterValue(
        ParameterNames::stripRepeatLength[stripId]
    );
    offsetParameter = vts.getRawParameterValue(
        ParameterNames::stripOffset[stripId]
    );
    rangeParameter = vts.getRawParameterValue(
        ParameterNames::stripRange[stripId]
    );
    gateDensityParameter = vts.getRawParameterValue(
        ParameterNames::stripGateDensity[stripId]
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
    vts.getParameter(ParameterNames::stripRepeatProbability[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripRepeatLength[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripOffset[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripRange[stripId])->
        addListener(&listener);
    vts.getParameter(ParameterNames::stripGateDensity[stripId])->
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

    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::stripRepeatProbability[stripId], 
        "Repeat Probability", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterNames::stripRepeatLength[stripId], 
        "Repeat Length", 
        ParameterOptions::lengthOptions,
        0
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::stripOffset[stripId], 
        "Offset", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::stripRange[stripId], 
        "Range", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::stripGateDensity[stripId], 
        "Gate Density", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        1.0f
    ));
}

//==============================================================================
void Strip::loadParameters() {
    probability = probabilityParameter->load();
    group = (int) groupParameter->load();
    choice = choiceParameter->load();
    enabled = (bool) enabledParameter->load();
    choke = chokeParameter->load();
    repeatProbability = repeatProbabilityParameter->load();
    repeatLength = (int) repeatLengthParameter->load() + 1;
    offset = offsetParameter->load();
    range = rangeParameter->load();
}

//==============================================================================
Step::Step() {
    hasValue = false;
    identifier = 0ULL;

    noteNumber = 0;
    channel = 1;
    velocity = (juce::uint8) 127;
    
    // New
    isGateOpen = true;
    isGateClose = false;

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
    int channel = 1;

    std::uint64_t identifier = nextIdentifier(strip->group);

    // Note Number
    int noteNumber = 60 + strip->choice + (strip->stripId * NUM_CHOICES);
    
    // Velocity
    int vmin = clamp(
        (int) (strip->offset * 127.0f),
        1,
        127
    );

    int vmax = clamp(
        (int) ((strip->offset + strip->range) * 127.0f),
        1,
        127
    );

    juce::uint8 velocity = (juce::uint8) nextInt(vmin, vmax);

    Step step;
    step.noteNumber = noteNumber;
    step.channel = channel;
    step.velocity = velocity;
    step.strip = strip;

    step.hasValue = true;
    step.identifier = identifier;

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

    return first.identifier != second.identifier;
}

//==============================================================================
Gate::Gate() {
    id = -1;

    gateLength = 0;
    gateLengthMultiplier = 0;
    gateInterval = 0;
    gatePortion = 0.0f;
}

//==============================================================================
Gate::~Gate() {

}

//==============================================================================
void Gate::setGateId(int id) {
    this->id = id;
}

//==============================================================================
void Gate::init(
    juce::AudioProcessorValueTreeState& vts, 
    juce::AudioProcessorParameter::Listener& listener) {

    gateLengthParameter = vts.getRawParameterValue(
        ParameterNames::gateLength[id]
    );
    gateLengthMultiplierParameter = vts.getRawParameterValue(
        ParameterNames::gateLengthMultiplier[id]
    );
    gateIntervalParameter = vts.getRawParameterValue(
        ParameterNames::gateInterval[id]
    );
    gatePortionParameter = vts.getRawParameterValue(
        ParameterNames::gatePortion[id]
    );

    vts.getParameter(ParameterNames::gateLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::gateLengthMultiplier[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::gateInterval[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::gatePortion[id])->
        addListener(&listener);
}

//==============================================================================
void Gate::createParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout) {
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::gateLength[id],
        "GateLength",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::gateLengthMultiplier[id],
        "GateLengthMultiplier",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::gateInterval[id],
        "GateInterval",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterNames::gatePortion[id], 
        "Portion", 
        juce::NormalisableRange {
            0.0f, 1.0f, 0.01f, 1.0f, false
        },
        0.0f
    ));
}

//==============================================================================
void Gate::loadParameters() {
    gateLength = gateLengthParameter->load() + 1;
    gateLengthMultiplier = gateLengthMultiplierParameter->load() + 1;
    gateInterval = gateIntervalParameter->load() + 1;
    gatePortion = gatePortionParameter->load();
}

//==============================================================================
Group::Group() {
    id = -1;
    length = 0;
    plusSixteen = 0;
    enabled = true;
    loop = false;
    isOn = false;
    // gateIsOpen = false;

    currentStrip = nullptr;

    numSteps = 0;
    step = 0;
    sequence = new Step[MAX_STEPS];
    needsReset = false;

    sequenceLength = 0;
    sequenceLengthMultiplier = 0;

    intervalLength = 0;
    intervalLengthMultiplier = 0;

    divisionLength = 0;
    divisionLengthMultiplier = 0;

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
    int d = 0;
    int sequenceSteps = sequenceLength * sequenceLengthMultiplier;
    int intervalSteps = intervalLength * intervalLengthMultiplier;
    int divisionSteps = divisionLength * divisionLengthMultiplier;

    s = 0;
    while (
        s < sequenceSteps ) {

        i = 0;
        while (
            s < sequenceSteps &&
            i < intervalSteps) {

            Strip* strip = 
                nextFloat() < density
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

            int chokeSteps = 
                strip != nullptr
                ? (int) ((float) divisionSteps * strip->choke)
                : divisionSteps
            ;

            int repeatSteps = 
                strip != nullptr &&
                strip->repeatProbability > nextFloat()
                ? strip->repeatLength
                : divisionSteps
            ;

            d = 0;
            while (
                s < sequenceSteps &&
                i < intervalSteps &&
                d < divisionSteps) {

                int r = 0;
                while (
                    s < sequenceSteps &&
                    i < intervalSteps &&
                    d < divisionSteps &&
                    r < repeatSteps) {

                    if (d > chokeSteps) {
                        next = Step::empty();
                    }

                    sequence[s] = next;

                    s++, i++, d++, r++;
                } // Repeat Loop
                
                if (next.hasValue) {
                    next.identifier = nextIdentifier(strip->group);
                }
            } // Division Loop
        } // Interval Loop
    } // Sequence Loop

    // Now do the gating
    this->gateSequence();

    // Finally do the repeating here (Later)
}

//==============================================================================
void Group::gateSequence() {

    int sequenceSteps = this->numSteps;
    if (sequenceSteps == 0 || this->sequence == nullptr) return;

    // Initial parameters (Will be immediately overwritten on step 0 due to blockStartStep initialization)
    int gateSteps = 1;
    int gateLength = 1;
    float targetGateLimit = 0.0f;

    int gateIndex = 0; 

    Step* blockStartStep = nullptr;

    for (int s = 0; s < sequenceSteps; s++) {
        Step* currentStep = &this->sequence[s];

        
        if (blockStartStep == nullptr || 
            Step::differs(*blockStartStep, *currentStep)) {
            // 1. If block changes (or it's the very first step), force gateIndex to the end
            blockStartStep = currentStep;
            gateIndex = gateSteps; // Force trigger the wrap/reroll logic below
        }

        // 2. Look-ahead wrap & reroll check
        if (gateIndex >= gateSteps) {
            gateSteps = 4; // gate:gateLength * gate:gateLenthMultiplier
            gateLength = 1; // gate:gateInterval
            float gatePortion = 0.5f; // gate:gatePortion

            // Gate* theGate = choose(blockStartStep, this->gates);

            gateIndex = 0; 
            targetGateLimit = static_cast<float>(gateLength) * gatePortion;
        }

        int l = gateIndex % gateLength;
        float progress = static_cast<float>(l);

        currentStep->isGateOpen = (progress < targetGateLimit);
        currentStep->isGateClose = (progress >= targetGateLimit) || ((progress + 1.0f) != targetGateLimit);

        gateIndex++;
    }
}

//==============================================================================
void Group::takeStep(juce::MidiBuffer& midiBuffer, int samplePos, Strip* strips) {
    Step currentStep = isOn
        ? sequence[step]
        : Step::empty()
    ;

    if (++step >= numSteps ||
        needsReset) {
        if (!loop) {
            createSequence(strips);
        }
        step = 0;
        needsReset = false;
    }
    
    Step newStep = 
        enabled &&
        sequence[step].hasValue &&
        sequence[step].strip->enabled
        ? sequence[step]
        : Step::empty()
    ;

    newNote(currentStep, newStep, midiBuffer, samplePos);

    // Do We Just Open the Gate Here??
    // Fuckin yolo
    openGate(midiBuffer, samplePos, strips);
}

//==============================================================================
void Group::openGate(juce::MidiBuffer& midiBuffer, int samplePos, Strip* strips) {
    // This is bad because it assumes the step has already been incremented
    if (!sequence[step].hasValue ||
        sequence[step].strip->gateIsOpen ||
        !sequence[step].isGateOpen) {
        // gate is already open, or not trying to open the gate: IDGAF
        return;
    }

    // TODO: We should close the gate here if gateIsOpen == false

    // Were not open yet, and we are trying to open: Start a new gate note
    midiBuffer.addEvent (
        // This is really shit, But Im drunk so whatever
        juce::MidiMessage::noteOn (
            1,                                  // Channel
            8 + sequence[step].strip->stripId,  // NoteNumber: Fuckin watever, its 8 + Strip ID
            (juce::uint8) 127                   // Velocity
        ),
        samplePos
    );
    
    sequence[step].strip->gateIsOpen = true;
};

//==============================================================================
void Group::closeGate(juce::MidiBuffer& midiBuffer, int samplePos, Strip* strips) {
    // This is bad because is assumes the step is still pointing to the current step
    // Above is NOT NECESSARILY TRUE (Although almost always is)
    if (!sequence[step].hasValue ||
        !sequence[step].strip->gateIsOpen ||
        !sequence[step].isGateClose) {
        // Gate isnt open, or not trying to close the gate: IDGAF
        return;
    }

    // were open, and we are trying to close: end the gate note
    midiBuffer.addEvent (
        juce::MidiMessage::noteOff (
            // Remember, EXTREMELY SHIT
            // We cant use the existing note on/off methods because they turn the whole strip on/off
            // I told myself Id get this done tonights
            1,                                  // Channel
            8 + sequence[step].strip->stripId   // NoteNumber: its fucking 8 + Strip ID IDGAF, Fix it later
        ),
        samplePos
    );

    sequence[step].strip->gateIsOpen = false;
};

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
void Group::scheduleReset() {
    needsReset = true;
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
    divisionLengthParameter = vts.getRawParameterValue(
        ParameterNames::groupDivisionLength[id]
    );
    divisionLengthMultiplierParameter = vts.getRawParameterValue(
        ParameterNames::groupDivisionLengthMultiplier[id]
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
    vts.getParameter(ParameterNames::groupDivisionLength[id])->
        addListener(&listener);
    vts.getParameter(ParameterNames::groupDivisionLengthMultiplier[id])->
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
        ParameterNames::groupDivisionLength[id],
        "DivisionLength",
        ParameterOptions::lengthOptions,
        0
    ));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterNames::groupDivisionLengthMultiplier[id],
        "DivisionLengthMultiplier",
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

    divisionLength = divisionLengthParameter->load() + 1;
    divisionLengthMultiplier = divisionLengthMultiplierParameter->load() + 1;

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
    for (int i = 0; i < NUM_GATES; i++) {
        gates[i].init(parameters, *this);
    }
    
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
        for (int i = 0; i < NUM_GATES; i++) {
            gates[i].loadParameters();
        }
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
        // Now we just have two blocks, off or on ^_____^
        if (current.isNoteOff()) {
            // Yes, This is disgusting. Suggest something better or STFU
            for (int g = 0; g < NUM_GROUPS; g++) {
                switch (current.getNoteNumber()) {
                    // We do the same thing in every case, but rather be consistent
                    case NOTE_TAKE_STEP:
                        groups[g].closeGate(processedBuffer, samplePos, strips);
                        break;
                    case NOTE_SCHEDULE_RESET:
                        groups[g].closeGate(processedBuffer, samplePos, strips);
                        break;
                }
                
            }
        }

        if (current.isNoteOn()) {
            for (int g = 0; g < NUM_GROUPS; g++) {
                switch (current.getNoteNumber()) {
                    case NOTE_TAKE_STEP:
                        groups[g].takeStep(processedBuffer, samplePos, strips);
                        break;
                    case NOTE_SCHEDULE_RESET:
                        // Bad but good enough for now, but come back and fix this
                        // More like "STEP FROM FIRST" than "RESET"
                        // Fine as long as you dont send two note ons at once
                        groups[g].scheduleReset();
                        groups[g].takeStep(processedBuffer, samplePos, strips);
                        break;
                }
                
            }
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

    gates = new Gate[NUM_GATES];

    groups = new Group[NUM_GROUPS];
    strips = new Strip[NUM_STRIPS];

    for (int i = 0; i < NUM_GATES; i++) {
        gates[i].setGateId(i); // Dont think I need this
        gates[i].createParameterLayout(layout);
    }

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
