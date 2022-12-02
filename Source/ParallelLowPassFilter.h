#pragma once
#include <vector>
#include "JuceHeader.h"

class ParallelLowPassFilter
{
public:
	float cutOffFrequency;
	float sampleRate;
	// std::vector<float> dnBuffer;

	float leftDnBuffer;
	float rightDnBuffer;

	// float* leftOutputBuffer = nullptr;
	// float* rightOutputBuffer = nullptr;

	// void initializeOutputBuffers(int bufferSize);

	// void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);
	void processBlock(
		float* leftBuffer,
		float* rightBuffer,
		const float* leftPointer,
		const float* rightPointer,
		int bufferSize);

	// ParallelLowPassFilter();
};