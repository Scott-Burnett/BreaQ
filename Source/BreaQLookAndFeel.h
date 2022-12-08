#pragma once
#include <JuceHeader.h>

class BreaQLookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawRotarySlider(
		juce::Graphics& g, 
		int x, 
		int y, 
		int width, 
		int height, 
		float sliderPos,
		const float rotaryStartAngle, 
		const float rotaryEndAngle, 
		juce::Slider& slider
	);

private:
	const juce::Colour usedArcColour = 
		juce::Colour(255, 255, 255);
	const juce::Colour unusedArcColour = 
		juce::Colour(0, 0, 0);
};