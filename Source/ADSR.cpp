#include "ADSR.h"
#include <cmath>

void ADSR::trigger() {
	state = ADSR::State::Attack;
	timeSinceTrigger = 0.0f;
	attenuation = peakLevel;
}

void ADSR::release() {
	state = ADSR::State::Release;
	timeSinceRelease = 0.0f;
	attenuation = 0.0f;
}

void ADSR::processBlock(
		juce::MidiBuffer midiBuffer,
		float* leftPointer,
		float* rightPointer,
		int bufferSize) {
	int i = 0;
	for (const juce::MidiMessageMetadata metadata : midiBuffer) {
		auto message = metadata.getMessage();
		int nextTimeStamp = message.getTimeStamp();
		while (i < nextTimeStamp) {
			incrementTime();
			leftPointer[i] *= attenuation;
			rightPointer[i] *= attenuation;
			i++;

			// std::cout << attenuation << '\n';
		}

		if (message.isNoteOn()) {
			trigger();
			currentNoteNumber = message.getNoteNumber();
			currentNoteChannel = message.getChannel();

		} else
		if (message.isNoteOff() &&
			message.getNoteNumber() == currentNoteNumber &&
			message.getChannel() == currentNoteChannel) {
			release();
		}
	}

	while (i < bufferSize) {
		incrementTime();
		leftPointer[i] *= attenuation;
		rightPointer[i] *= attenuation;
		i++;

		//std::cout << attenuation << '\n';
	}
}


void ADSR::incrementTime() {
	switch(state) {
	case ADSR::State::Idle:
		// Do Nothing
		break;

	case ADSR::State::Attack:
		timeSinceTrigger += sampleDuration;
		if (timeSinceTrigger >= attackTime) {
			state = ADSR::State::Decay;
			timeSinceDecay = timeSinceTrigger - attackTime;
			attenuation = peakLevel;
			return;
		}

		attenuation = // No need to check for zero. zero attack time would trigger above
			pow(
				initialLevel + timeSinceTrigger / attackTime * (peakLevel - initialLevel),
				attackCurve
			);
		break;

	case ADSR::State::Decay:
		timeSinceDecay += sampleDuration;
		if (timeSinceDecay >= decayTime) {
			state = ADSR::State::Sustain;
			attenuation = sustainLevel;
			return;
		}

		attenuation = // No need to check for zero. zero decay time would trigger above
			pow(
				peakLevel + timeSinceDecay / decayTime * (sustainLevel - peakLevel),
				decayCurve
			);
		break;

	case ADSR::State::Sustain:
		// Do Nothing
		break;

	case ADSR::State::Release:
		timeSinceRelease += sampleDuration;
		if (timeSinceRelease >= releaseTime) {
			state = ADSR::State::Idle;
			attenuation = 0.0f;
			return;
		}

		attenuation = // No need to check for zero. zero decay time would trigger above
			pow(
				sustainLevel + timeSinceRelease / releaseTime * (0.0f - sustainLevel),
				releaseCurve
			);
		break;
	}
}


ADSR::ADSR() {
	sampleDuration = 0.0f;
	attackTime = 0.0f;
	attackCurve = 1.0f;
	decayTime = 0.05f;
	decayCurve = 1.0f;
	initialLevel = 0.0f;
	peakLevel = 1.0f;
	sustainLevel = 0.75f;
	releaseTime = 0.0f;
	releaseCurve = 1.0f;

	state = ADSR::State::Idle;
	currentNoteNumber = -1;
	currentNoteChannel = -1;
	timeSinceTrigger = 0.0f;
	timeSinceDecay = 0.0f;
	timeSinceRelease = 0.0f;
	attenuation = 0.0f;
}

ADSR::~ADSR() {
}