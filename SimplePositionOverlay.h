#pragma once

#include <JuceHeader.h>

class SimplePositionOverlay :
  public Component,
  private Timer
{
  public:
    SimplePositionOverlay(RiseAndFallAudioProcessor &p, CustomLookAndFeel &laf) :
      processor(p),
      lookAndFeel(laf)
    {
      startTimer(40);
    }

    void paint(Graphics &g) override
    {
      auto duration = processor.getNumSamples();

      if (duration <= 0) {
        return;
      }

      int position = processor.getPosition();
      int numSamples = processor.getNumSamples();
      float percentage = (float) position / numSamples;
      float drawPosition = (percentage * 656) + 16;
      g.setColour(lookAndFeel.COLOUR_BLACK);
      g.drawLine(
        drawPosition,
        0.0f,
        drawPosition,
        static_cast<float>(getHeight()),
        1.0f
      );
    }

  private:
    void timerCallback() override
    { repaint(); }

    RiseAndFallAudioProcessor &processor;
    CustomLookAndFeel &lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimplePositionOverlay)
};
