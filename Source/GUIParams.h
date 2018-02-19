/*
  ==============================================================================

    GUIParams.h
    Created: 18 Jan 2018 3:20:51pm
    Author:  Barthélémy Bonhomme

  ==============================================================================
*/

#pragma once

class GUIParams {

public:
    double timeOffset = 0;
    bool riseReverse = true;
    bool fallReverse = false;
    bool riseEffects = true;
    bool fallEffects = true;
    int riseTimeWarp = 0;
    int fallTimeWarp = 0;
    int delayTime = 500;
    int delayFeedback = 50;
    int impulseResponse = 0;
    int filterType = 0;
};
