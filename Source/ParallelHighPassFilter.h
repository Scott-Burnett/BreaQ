#pragma once
#include <vector>
#include "JuceLibraryCode/JuceHeader.h"

class ParallelHighPassFilter
{
public:
	float cutOffFrequency;
	float sampleRate;
	int f_order;

	void processBlock(
		float* leftBuffer,
		float* rightBuffer,
		int bufferSize
	);

	ParallelHighPassFilter();

private:
	float leftDnBuffer;
	float rightDnBuffer;
};

