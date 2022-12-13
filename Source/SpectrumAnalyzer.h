#pragma once

#include <JuceHeader.h>

class SpectrumAnalyzer : public juce::Component, public juce::AudioSource, public juce::Timer {
public:
    static const int fftOrder = 11;
    static const int fftSize = 1 << fftOrder;
    static const int scopeSize = 512;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;

    void processBuffer(const float* buffer, int bufferSize);

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void timerCallback() override;

    void drawFrame(juce::Graphics& g);
    void paint(juce::Graphics& g) override;

    SpectrumAnalyzer();
    ~SpectrumAnalyzer();

private:
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    float scopeData[scopeSize];

    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextFrameOfSpectrum();
};