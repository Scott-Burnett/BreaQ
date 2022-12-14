#include "ADSRVisualizer.h"

ADSRVisualizer::ADSRVisualizer() {
    startTimer(700);
}

ADSRVisualizer::~ADSRVisualizer() {
}

void ADSRVisualizer::timerCallback() {
    if (needsRepaint) {
        repaint();
        needsRepaint = false;
    }
}

void ADSRVisualizer::parameterValueChanged(int parameterIndex, float newValue) {
    needsRepaint = true;
}


void ADSRVisualizer::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {

}

void ADSRVisualizer::paint(juce::Graphics& g) {
    auto left = (double)getLocalBounds().getTopLeft().getX();
    auto right = (double)getLocalBounds().getTopRight().getX();
    auto top = (double)getLocalBounds().getTopLeft().getY();
    auto bottom = (double)getLocalBounds().getBottomLeft().getY();

    const float minimumTime = 0.05f;
    const auto attackTime = (double)juce::jmax(attack, minimumTime);
    const auto decayTime = (double)juce::jmax(decay, minimumTime);
    const auto sustainTime = 0.25f;
    const auto releaseTime = (double)juce::jmax(release, minimumTime);

    const auto totalLength = attackTime + decayTime + sustainTime + releaseTime;

    int initialY = (int)juce::jmap(
        (double)initial,
        bottom,
        top
    );

    int attackX = (int)juce::jmap(
        attackTime,
        0.0,
        totalLength,
        left,
        right
    );

    int attackY = (int)juce::jmap(
        (double)peak,
        bottom,
        top
    );

    int decayX = (int)juce::jmap(
        attackTime + decayTime,
        0.0,
        totalLength,
        left,
        right
    );

    int decayY = (int)juce::jmap(
        (double)sustain,
        bottom,
        top
    );

    int sustainX = (int)juce::jmap(
        attackTime + decayTime + sustainTime,
        0.0,
        totalLength,
        left,
        right
    );

    int releaseX = right;
    int releaseY = bottom;

    auto attackCurveX = juce::jmap(
        (double)attackCurve,
        0.0,
        2.0,
        left,
        (double)attackX
    );

    auto attackCurveY = juce::jmap(
        (double)attackCurve,
        0.0,
        2.0,
        (double)attackY,
        (double)initialY
    );

    auto decayCurveX = juce::jmap(
        (double)decayCurve,
        0.0,
        2.0,
        (double)attackX,
        (double)decayX
    );

    auto decayCurveY = juce::jmap(
        (double)decayCurve,
        0.0,
        2.0,
        (double)decayY,
        (double)attackY
    );

    auto releaseCurveX = juce::jmap(
        (double)releaseCurve,
        0.0,
        2.0,
        (double)sustainX,
        (double)releaseX
    );

    auto releaseCurveY = juce::jmap(
        (double)releaseCurve,
        0.0,
        2.0,
        (double)releaseY,
        (double)decayY
    );

    juce::Path path;
    path.startNewSubPath(left, initialY);
    path.quadraticTo(attackCurveX, attackCurveY, attackX, attackY);
    path.quadraticTo(decayCurveX, decayCurveY, decayX, decayY);
    path.lineTo(sustainX, decayY);
    path.quadraticTo(releaseCurveX, releaseCurveY, releaseX, releaseY);

    g.setColour(lilac);
    g.strokePath(
        path,
        juce::PathStrokeType(
            2,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    g.setColour(orange1);
    g.fillPath(path);
}