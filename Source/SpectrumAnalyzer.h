#pragma once

#include <JuceHeader.h>

class SpectrumAnalyzer : public juce::Component, public juce::AudioSource, public juce::Timer {
public:
    const juce::Colour aquamarine = juce::Colour(73, 212, 157); // (49D49D);
    const juce::Colour teaGreen = juce::Colour(191, 215, 181); // (BFD7B5);
    const juce::Colour transparentTeaGreen = juce::Colour::fromRGBA(191, 215, 181, 75); // (BFD7B5);
    const juce::Colour springBud = juce::Colour(231, 239, 197); // (E7EFC5);
    const juce::Colour champagne = juce::Colour(242, 221, 164); // (F2DDA4);
    const juce::Colour citron = juce::Colour(162, 159, 21); // (A29F15);
    const juce::Colour terraCotta = juce::Colour(226, 109, 92); // (E26D5C);

    const juce::Colour background1 = juce::Colour(13, 16, 22); // (0x0d1016);
    const juce::Colour transparentBackground1 = juce::Colour::fromRGBA(13, 16, 22, 100); // (0x0d1016);
    const juce::Colour background2 = juce::Colour(0, 1, 10); // (0x00010a);
    const juce::Colour transparentBackground2 = juce::Colour::fromRGBA(0, 1, 10, 100); // (0x00010a);

    const juce::Colour lilac = juce::Colour(218, 173, 224); // (0xdaade0);
    const juce::Colour transparentLilac = juce::Colour::fromRGBA(218, 173, 224, 100); // (0xdaade0);

    const juce::Colour pink = juce::Colour(238, 104, 175); // (0xee68af);

    const juce::Colour grid1 = juce::Colour(41, 45, 54); // (0x292d36);
    const juce::Colour grid2 = juce::Colour(61, 66, 77); // (0x3d424d);
    const juce::Colour grid3 = juce::Colour(179, 177, 173); // (0xb3b1ad);

    const juce::Colour orange2 = juce::Colour(245, 187, 108); // (0xf5bb6c);
    const juce::Colour orange1 = juce::Colour(253, 198, 138); // (0xfdc68a);
    const juce::Colour orange3 = juce::Colour(247, 203, 139); // (0xf7cb8b);
    const juce::Colour transparentOrange3 = juce::Colour::fromRGBA(247, 203, 139, 100); // (0xf7cb8b);

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