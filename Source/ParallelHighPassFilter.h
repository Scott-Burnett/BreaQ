#pragma once
#include <vector>
#include "JuceHeader.h"

class ParallelHighPassFilter
{
public:
	float cutOffFrequency;
	float sampleRate;

	float leftDnBuffer;
	float rightDnBuffer;

	void processBlock(
		float* leftBuffer,
		float* rightBuffer,
		const float* leftPointer,
		const float* rightPointer,
		int bufferSize
	);

	ParallelHighPassFilter();
};

