#pragma once
#include <JuceHeader.h>

class ADSR {
public:
	enum class State {
		Idle = 0,
		Attack = 1,
		Decay = 2,
		Sustain = 3,
		Release = 4
	};

	State state;

	float sampleDuration;
	float attackTime;
	float attackCurve;
	float decayTime;
	float decayCurve;
	float initialLevel;
	float peakLevel;
	float sustainLevel;
	float releaseTime;
	float releaseCurve;

	float attenuation;

	void processBlock(
		juce::MidiBuffer midiBuffer,
		float* leftPointer,
		float* rightPointer,
		int bufferSize
	);

	ADSR();
	~ADSR();

private:
	int currentNoteNumber;
	int currentNoteChannel;
	float timeSinceTrigger;
	float timeSinceDecay;
	float timeSinceRelease;

	void trigger();
	void release();
	void incrementTime();
};