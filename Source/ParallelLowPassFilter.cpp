#include "ParallelLowPassFilter.h"

/*void ParallelLowPassFilter::processBlock(
		float* leftBuffer, 
		float* rightBuffer, 
		const float* leftPointer, 
		const float* rightPointer,
		int bufferSize) {
	constexpr auto PI = 3.14159265359f;
	const auto tan = std::tan(PI * cutOffFrequency / sampleRate);
	const auto a1 = (tan - 1.0f) / (tan + 1.0f);

	// leftDnBuffer = 0.0f;
	// rightDnBuffer = 0.0f;
	for (int i = 0; i < bufferSize; i++) {
		leftBuffer[i] = leftPointer[i];
		rightBuffer[i] = rightPointer[i];
	}

	for (int i = 0; i < bufferSize; i++) {
		const auto leftInputSample = leftPointer[i];
		const auto rightInputSample = rightPointer[i];

		const auto leftAllPassFilteredSample = a1 * leftInputSample + leftDnBuffer;
		leftDnBuffer = leftInputSample - a1 * leftAllPassFilteredSample;

		const auto rightAllPassFilteredSample = a1 * rightInputSample + rightDnBuffer;
		rightDnBuffer = rightInputSample - a1 * rightAllPassFilteredSample;

		const auto leftFilterOutput = 0.5f * (leftInputSample + leftAllPassFilteredSample);
		leftBuffer[i] = leftFilterOutput;

		const auto rightFilterOutput = 0.5f * (rightInputSample + rightAllPassFilteredSample);
		rightBuffer[i] = rightFilterOutput;
	}
}*/

void ParallelLowPassFilter::processBlock(
		float* leftBuffer,
		float* rightBuffer,
		const float* leftPointer,
		const float* rightPointer,
		int bufferSize) {
	constexpr auto PI = 3.14159265359f;
	const auto tan = std::tan(PI * cutOffFrequency / sampleRate);
	const auto a1 = (tan - 1.0f) / (tan + 1.0f);

	for (int i = 0; i < bufferSize; i++) {
		leftBuffer[i] = leftPointer[i];
		rightBuffer[i] = rightPointer[i];
	}

	for (int k = 0; k < (int)f_order; k++) {
		for (int i = 0; i < bufferSize; i++) {
			const auto leftInputSample = leftBuffer[i];
			const auto rightInputSample = rightBuffer[i];

			const auto leftAllPassFilteredSample = a1 * leftInputSample + leftDnBuffer;
			leftDnBuffer = leftInputSample - a1 * leftAllPassFilteredSample;

			const auto rightAllPassFilteredSample = a1 * rightInputSample + rightDnBuffer;
			rightDnBuffer = rightInputSample - a1 * rightAllPassFilteredSample;

			const auto leftFilterOutput = 0.5f * (leftInputSample + leftAllPassFilteredSample);
			leftBuffer[i] = leftFilterOutput;

			const auto rightFilterOutput = 0.5f * (rightInputSample + rightAllPassFilteredSample);
			rightBuffer[i] = rightFilterOutput;
		}
	}
}

ParallelLowPassFilter::ParallelLowPassFilter() {
	cutOffFrequency = 0.0f;
	sampleRate = 0.0f;
	leftDnBuffer = 0.0f;
	rightDnBuffer = 0.0f;
	f_order = 1;
}