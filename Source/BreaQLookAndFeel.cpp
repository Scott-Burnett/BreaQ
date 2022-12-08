#pragma once
#include "BreaQLookAndFeel.h"

void BreaQLookAndFeel::drawRotarySlider(
		juce::Graphics& g,
		int x,
		int y,
		int width,
		int height,
		float sliderPos,
		const float rotaryStartAngle,
		const float rotaryEndAngle,
		juce::Slider& slider) {

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(unusedArcColour);
    g.strokePath(
        backgroundArc, 
        juce::PathStrokeType(
            lineW, 
            juce::PathStrokeType::curved, 
            juce::PathStrokeType::rounded
        )
    );

    if (slider.isEnabled()) {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(usedArcColour);
        g.strokePath(
            valueArc, 
            juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    juce::Point<float> thumbPoint(
        bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi)
    );

    g.setColour(usedArcColour);
    g.drawLine(
        bounds.getCentreX(),
        bounds.getCentreY(),
        thumbPoint.getX(),
        thumbPoint.getY(),
        lineW
    );

    g.fillEllipse(
        juce::Rectangle<float>(lineW, lineW)
        .withCentre(bounds.getCentre())
    );

    g.fillEllipse(
        juce::Rectangle<float>(lineW, lineW)
        .withCentre(thumbPoint)
    );
}