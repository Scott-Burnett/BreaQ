#include "ParallelLowPassFilter.h"

/*void ParallelLowPassFilter::processBlock(juce::AudioBuffer<float>& audioBuffer, juce::MidiBuffer& midiBuffer) {
	constexpr auto PI = 3.14159265359f;
	const auto tan = std::tan(PI * cutOffFrequency / sampleRate);
	const auto a1 = (tan - 1.0f) / (tan + 1.0f);

	dnBuffer.resize(audioBuffer.getNumChannels(), 0.0f);

	for (auto channel = 0; channel < audioBuffer.getNumChannels(); channel++) {
		auto channelSamples = audioBuffer.getReadPointer(channel);
		for (auto i = 0; i < audioBuffer.getNumSamples(); i++) {
			const auto inputSample = channelSamples[i];

			const auto allPassFilteredSample = a1 * inputSample + dnBuffer[channel];
			dnBuffer[channel] = inputSample - a1 * allPassFilteredSample;

			const auto filterOutput = 0.5f * (inputSample + allPassFilteredSample);
			// outputSamples[i] = filterOutput;
			// Write to left and right output buffers
			leftOutputBuffer[i] = filterOutput;
			rightOutputBuffer[i] = filterOutput;
		}
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

	leftDnBuffer = 0.0f;
	rightDnBuffer = 0.0f;

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

/*void ParallelLowPassFilter::initializeOutputBuffers(int bufferSize) {
	leftOutputBuffer = new float[bufferSize];
	rightOutputBuffer = new float[bufferSize];
}

ParallelLowPassFilter::ParallelLowPassFilter() {
	leftOutputBuffer = nullptr;
	rightOutputBuffer = nullptr;
}*/