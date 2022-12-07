#pragma once
#include <vector>
#include "JuceHeader.h"

class ParallelLowPassFilter
{
public:
	float cutOffFrequency;
	float sampleRate;
	int f_order;

	void processBlock(
		float* leftBuffer,
		float* rightBuffer,
		const float* leftPointer,
		const float* rightPointer,
		int bufferSize
	);

	ParallelLowPassFilter();

private:
	float leftDnBuffer;
	float rightDnBuffer;
};