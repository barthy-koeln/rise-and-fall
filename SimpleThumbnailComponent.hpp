#pragma once

#include <JuceHeader.h>

class SimpleThumbnailComponent :
  public Component,
  public ChangeListener
{
  public:
    SimpleThumbnailComponent(
      AudioThumbnail &t,
      [[maybe_unused]] AudioThumbnailCache &cache,
      CustomLookAndFeel &laf
    ) :
      thumbnail(t), lookAndFeel(laf)
    {
      thumbnail.addChangeListener(this);
    }

    void paint(Graphics &g) override
    {
      if (thumbnail.getNumChannels() == 0) {
        paintIfNoFileLoaded(g);
        return;
      }

      paintIfFileLoaded(g);
    }

    void paintIfNoFileLoaded(Graphics &g)
    {
      g.fillAll(lookAndFeel.COLOUR_WHITE);
      g.setColour(lookAndFeel.COLOUR_BLACK);
      g.drawFittedText(
        "No File Loaded",
        getLocalBounds(),
        Justification::centred,
        1
      );
    }

    void paintIfFileLoaded(Graphics &g)
    {
      g.fillAll(lookAndFeel.COLOUR_WHITE);
      g.setColour(lookAndFeel.COLOUR_RED);
      thumbnail.drawChannels(
        g,
        getLocalBounds(),
        0.0,
        thumbnail.getTotalLength(),
        1.0f
      );
    }

    void changeListenerCallback(ChangeBroadcaster *source) override
    {
      if (source == &thumbnail) { thumbnailChanged(); }
    }

  private:
    void thumbnailChanged()
    {
      repaint();
    }

    AudioThumbnail &thumbnail;
    CustomLookAndFeel &lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleThumbnailComponent)
};
