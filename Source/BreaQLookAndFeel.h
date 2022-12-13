#pragma once
#include <JuceHeader.h>

class BreaQLookAndFeel : public juce::LookAndFeel_V4
{
public:
	const juce::Colour background1 = juce::Colour(13, 16, 22); // (0x0d1016);
	const juce::Colour background2 = juce::Colour(0, 1, 10); // (0x00010a);

	const juce::Colour lilac = juce::Colour(218, 173, 224); // (0xdaade0);

	const juce::Colour pink = juce::Colour(238, 104, 175); // (0xee68af);

	const juce::Colour grid1 = juce::Colour(41, 45, 54); // (0x292d36);
	const juce::Colour grid2 = juce::Colour(61, 66, 77); // (0x3d424d);
	const juce::Colour grid3 = juce::Colour(179, 177, 173); // (0xb3b1ad);

	const juce::Colour orange2 = juce::Colour(245, 187, 108); // (0xf5bb6c);
	const juce::Colour orange1 = juce::Colour(253, 198, 138); // (0xfdc68a);
	const juce::Colour orange3 = juce::Colour(247, 203, 139); // (0xf7cb8b);

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