#pragma once

#include <JuceHeader.h>
#include <stdlib.h>
#include <math.h>

class NoteLengthSlider :
  public Slider
{
  public:
    NoteLengthSlider (const String& name): Slider(name) {

    }

    String getTextFromValue(double value) override
    {
      String textValue = String(pow(2, (int) abs(value)));

      if (value < 0) {
        return "1/" + textValue;
      }

      return textValue;
    }
};
