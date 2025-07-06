#pragma once
#include "BreaQLookAndFeel.h"
#include "ParameterNames.h"

//==============================================================================
void BreaQLookAndFeel::drawRotarySlider(
		juce::Graphics& g,
		int x,
		int y,
		int width,
		int height,
		float sliderPos,
		const float rotaryStartAngle,
		const float rotaryEndAngle,
		juce::Slider& slider
) {
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(8);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight());// / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 1.2f;
    auto arcRadius = radius - lineW * 0.5f;
    auto fillDiameter = arcRadius;

    juce::Rectangle fillRect = 
        juce::Rectangle<float>(fillDiameter, fillDiameter)
        .withCentre(bounds.getCentre())
    ;

    g.setGradientFill({
        Colours::transparentPink, fillRect.getTopLeft(),
        Colours::transparentBackground2, fillRect.getCentre(),
        false
        });

    g.fillEllipse(fillRect);

    auto centreToThumb = fillDiameter * 0.25f;
    auto thumbDiameter = centreToThumb * 1.0f;

    juce::Point<float> thumbStart = fillRect.getCentre().getPointOnCircumference(radius * 0.5f, toAngle);
    juce::Point<float> thumbEnd = fillRect.getCentre().getPointOnCircumference(radius * 0.3f, toAngle);

    g.setColour(Colours::orange3);

    g.drawEllipse(fillRect, lineW);

    g.drawLine(thumbStart.getX(), thumbStart.getY(), thumbEnd.getX(), thumbEnd.getY(), lineW * 2.0f);
}

//==============================================================================
void BreaQLookAndFeel::drawToggleButton(
    juce::Graphics& g,
    juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown
) {
    juce::Rectangle bounds = button.getLocalBounds().toFloat();
    float edge = juce::jmin(bounds.getWidth(), bounds.getHeight());

    auto lineW = 1.2f;

    juce::Rectangle fillRect = 
        juce::Rectangle(edge, edge)
        .withCentre(bounds.getCentre())
        .toFloat().reduced(2);

    if (button.getToggleState()) {
        g.setGradientFill({
            Colours::orange1, fillRect.getTopLeft(),
            Colours::orange3, fillRect.getBottomRight(),
            false
            });
    }
    else {
        g.setGradientFill({
            Colours::transparentOrange3, fillRect.getTopLeft(),
            Colours::transparentOrange1, fillRect.getBottomRight(),
            false
        });
    }

    g.fillRect(fillRect);

    g.setGradientFill({
        Colours::orange3, fillRect.getTopLeft(),
        Colours::orange1, fillRect.getBottomRight(),
        false
        });

    g.drawRect(fillRect.reduced(lineW), lineW);
}