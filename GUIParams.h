#pragma once

#define TIME_OFFSET 0
#define TIME_OFFSET_ID "timeOffset"
#define TIME_OFFSET_NAME "Time offset"

#define RISE_REVERSE 1
#define RISE_REVERSE_ID "riseReverse"
#define RISE_REVERSE_NAME "Rise reverse"

#define FALL_REVERSE 2
#define FALL_REVERSE_ID "fallReverse"
#define FALL_REVERSE_NAME "Fall reverse"

#define RISE_REVERB 3
#define RISE_REVERB_ID "riseReverb"
#define RISE_REVERB_NAME "Rise reverb"

#define RISE_DELAY 4
#define RISE_DELAY_ID "riseDelay"
#define RISE_DELAY_NAME "Rise delay"

#define FALL_REVERB 5
#define FALL_REVERB_ID "fallReverb"
#define FALL_REVERB_NAME "Fall reverb"

#define FALL_DELAY 6
#define FALL_DELAY_ID "fallDelay"
#define FALL_DELAY_NAME "Fall delay"

#define RISE_TIME_WARP 7
#define RISE_TIME_WARP_ID "riseTimeWarp"
#define RISE_TIME_WARP_NAME "Rise time warp"

#define FALL_TIME_WARP 8
#define FALL_TIME_WARP_ID "fallTimeWarp"
#define FALL_TIME_WARP_NAME "Fall time warp"

#define DELAY_TIME 9
#define DELAY_TIME_ID "delayTime"
#define DELAY_TIME_NAME "Delay time"

#define DELAY_FEEDBACK 10
#define DELAY_FEEDBACK_ID "delayFeedback"
#define DELAY_FEEDBACK_NAME "Delay feedback"

#define IMPULSE_RESPONSE 11
#define IMPULSE_RESPONSE_ID "impulseResponse"
#define IMPULSE_RESPONSE_NAME "Impulse response"

#define REVERB_MIX 12
#define REVERB_MIX_ID "reverbMix"
#define REVERB_MIX_NAME "Reverb mix"

#define DELAY_MIX 13
#define DELAY_MIX_ID "delayMix"
#define DELAY_MIX_NAME "Delay mix"

#define FILTER_TYPE 14
#define FILTER_TYPE_ID "filterType"
#define FILTER_TYPE_NAME "Filter type"

#define FILTER_CUTOFF 15
#define FILTER_CUTOFF_ID "filterCutoff"
#define FILTER_CUTOFF_NAME "Filter cutoff"

#define FILTER_RESONANCE 16
#define FILTER_RESONANCE_ID "filterResonance"
#define FILTER_RESONANCE_NAME "Filter resonance"

#include <JuceHeader.h>

class GUIParams :
  public AudioProcessorValueTreeState
{
  public:

    GUIParams(AudioProcessor &processor) :
      AudioProcessorValueTreeState(
        processor,
        nullptr,
        "PARAMETERS",
        {
          std::make_unique<AudioParameterInt>(
            TIME_OFFSET_ID,
            TIME_OFFSET_NAME,
            -1024,
            1024,
            1
          ),
          std::make_unique<AudioParameterBool>(
            RISE_REVERSE_ID,
            RISE_REVERSE_NAME,
            true
          ),
          std::make_unique<AudioParameterBool>(
            FALL_REVERSE_ID,
            FALL_REVERSE_NAME,
            false
          ),
          std::make_unique<AudioParameterBool>(
            RISE_REVERB_ID,
            RISE_REVERB_NAME,
            true
          ),
          std::make_unique<AudioParameterBool>(
            RISE_DELAY_ID,
            RISE_DELAY_NAME,
            true
          ),
          std::make_unique<AudioParameterBool>(
            FALL_REVERB_ID,
            FALL_REVERB_NAME,
            true
          ),
          std::make_unique<AudioParameterBool>(
            FALL_DELAY_ID,
            FALL_DELAY_NAME,
            true
          ),
          std::make_unique<AudioParameterFloat>(
            RISE_TIME_WARP_ID,
            RISE_TIME_WARP_NAME,
            NormalisableRange<float>(
              -4.0f,
              4.0f,
              2.0f
            ),
            0.0f
          ),
          std::make_unique<AudioParameterFloat>(
            FALL_TIME_WARP_ID,
            FALL_TIME_WARP_NAME,
            NormalisableRange<float>(
              -4.0f,
              4.0f,
              2.0f
            ),
            0.0f
          ),
          std::make_unique<AudioParameterFloat>(
            DELAY_TIME_ID,
            DELAY_TIME_NAME,
            NormalisableRange<float>(
              -6.0f,
              2.0f,
              1.0f
            ),
            0.0f
          ),
          std::make_unique<AudioParameterFloat>(
            DELAY_FEEDBACK_ID,
            DELAY_FEEDBACK_NAME,
            NormalisableRange<float>(
              0.0f,
              99.0f,
              0.1f
            ),
            50.0f
          ),
          std::make_unique<AudioParameterChoice>(
            IMPULSE_RESPONSE_ID,
            IMPULSE_RESPONSE_NAME,
            StringArray(
              CharPointer_UTF8("Terry's Factory Warehouse"),
              CharPointer_UTF8("York Minster"),
              CharPointer_UTF8("R1 Nuclear Reactor Hall"),
              CharPointer_UTF8("St. George's Episcopal Church"),
              CharPointer_UTF8("Empty Apartment Bedroom"),
              CharPointer_UTF8("Stairway, University of York")
            ),
            0
          ),
          std::make_unique<AudioParameterFloat>(
            REVERB_MIX_ID,
            REVERB_MIX_NAME,
            NormalisableRange<float>(
              0.0f,
              100.0f,
              0.1f
            ),
            50.0f
          ),
          std::make_unique<AudioParameterFloat>(
            DELAY_MIX_ID,
            DELAY_MIX_NAME,
            NormalisableRange<float>(
              0.0f,
              100.0f,
              0.1f
            ),
            50.0f
          ),
          std::make_unique<AudioParameterChoice>(
            FILTER_TYPE_ID,
            FILTER_TYPE_NAME,
            StringArray(
              CharPointer_UTF8("LP"),
              CharPointer_UTF8("HP")
            ),
            0
          ),
          std::make_unique<AudioParameterFloat>(
            FILTER_CUTOFF_ID,
            FILTER_CUTOFF_NAME,
            GUIParams::logarithmicRange(20.0f, 20000.0f),
            20000.0f
          ),
          std::make_unique<AudioParameterFloat>(
            FILTER_RESONANCE_ID,
            FILTER_RESONANCE_NAME,
            NormalisableRange<float>(
              0.1f,
              10.0f,
              0.1f
            ),
            1.0f
          )
        }
      )
    {
    }

    static NormalisableRange<float> logarithmicRange(float logStart, float logEnd, float exponent = 6.0f)
    {
      return {
        logStart, logEnd,
        [=](float start, float end, float normalised) {
          return start + (std::exp2(normalised * exponent) - 1) * (end - start) / (std::exp2(exponent) - 1);
        },
        [=](float start, float end, float wild) {
          return std::log2(((wild - start) / (end - start) * (std::exp2(exponent) - 1)) + 1) / exponent;
        },
        [=](float start, float end, float value) {
          return juce::jlimit(start, end, juce::roundToInt(value * float(10000)) / float(10000));
        }
      };
    }

    void attach(Slider &slider)
    {
      auto parameter = this->getParameter(slider.getName());

      this->sliderAttachments.add(std::make_unique<SliderParameterAttachment>(*parameter, slider));
    }

    void attach(ComboBox &comboBox)
    {
      auto parameter = (AudioParameterChoice *) this->getParameter(comboBox.getName());
      comboBox.addItemList(parameter->choices, 1);

      this->comboBoxAttachments.add(std::make_unique<ComboBoxParameterAttachment>(*parameter, comboBox));
    }

    void attach(ToggleButton &button)
    {
      auto parameter = (AudioParameterBool *) this->getParameter(button.getName());

      this->toggleButtonAttachments.add(std::make_unique<ButtonParameterAttachment>(*parameter, button));
    }

    void detachAll()
    {
      this->toggleButtonAttachments.clear();
      this->comboBoxAttachments.clear();
      this->sliderAttachments.clear();
    }

  private:

    OwnedArray<SliderParameterAttachment> sliderAttachments;
    OwnedArray<ComboBoxParameterAttachment> comboBoxAttachments;
    OwnedArray<ButtonParameterAttachment> toggleButtonAttachments;

};
