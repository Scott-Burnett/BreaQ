#include "SpectrumAnalyzer.h"

SpectrumAnalyzer::SpectrumAnalyzer()
	:
		forwardFFT(fftOrder),
		window(fftSize, juce::dsp::WindowingFunction<float>::hann) {
    startTimer(100);
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
}

void SpectrumAnalyzer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    if (bufferToFill.buffer->getNumChannels() > 0) {
        auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);

        for (auto i = 0; i < bufferToFill.numSamples; ++i)
            pushNextSampleIntoFifo(channelData[i]);
    }
}

void SpectrumAnalyzer::processBuffer(const float* bufferToFill, int bufferSize) {
        for (auto i = 0; i < bufferSize; ++i)
            pushNextSampleIntoFifo(bufferToFill[i]);
}

void SpectrumAnalyzer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
}

void SpectrumAnalyzer::releaseResources() {
}

void SpectrumAnalyzer::pushNextSampleIntoFifo(float sample) noexcept {
    if (fifoIndex == fftSize) {
        if (!nextFFTBlockReady) {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }
        fifoIndex = 0;
    }
    fifo[fifoIndex++] = sample;
}

void SpectrumAnalyzer::drawNextFrameOfSpectrum() {
    window.multiplyWithWindowingTable(fftData, fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i) {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
            - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f
        );

        scopeData[i] = level;
    }
}

void SpectrumAnalyzer::timerCallback()
{
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrumAnalyzer::drawFrame(juce::Graphics& g)
{
    
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    auto bottom = getLocalBounds().getBottom();
    auto left = getLocalBounds().getTopLeft().getX();
    auto right = getLocalBounds().getTopRight().getX();

    juce::Path path;
    path.startNewSubPath(left, bottom);
    for (int i = 1; i < scopeSize; ++i)
    {
        auto sampleX = juce::jmap(i, 0, scopeSize - 1, 0, width);
        auto sampleY = juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f);

        path.lineTo(sampleX, sampleY);
    }
    path.lineTo(right, bottom);

    g.setColour(orange1);
    g.fillPath(path);

    //g.setColour(pink);
    //g.strokePath(
    //    path,
    //    juce::PathStrokeType(
    //        1.0f,
    //        juce::PathStrokeType::curved,
    //        juce::PathStrokeType::rounded
    //    )
    //);
}

void SpectrumAnalyzer::paint(juce::Graphics& g) {
    drawFrame(g);
}