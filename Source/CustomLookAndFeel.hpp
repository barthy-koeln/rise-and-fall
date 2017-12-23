//
//  CustomLookAndFeel.hpp
//  RISE&FALL - VST
//
//  Created by Barthélémy Bonhomme on 22.12.17.
//

#ifndef CustomLookAndFeel_hpp
#define CustomLookAndFeel_hpp

#include "../JuceLibraryCode/JuceHeader.h"

class CustomLookAndFeel : public LookAndFeel_V4 {
public:
    Colour darkBlue;
    Colour blue;
    Colour lightBlue;
    Colour darkRed;
    Colour red;
    
    CustomLookAndFeel(){
        darkBlue = *new Colour(0xff2b2d42);
        blue = *new Colour(0xff8d99ae);
        lightBlue = *new Colour(0xffedf2f4);
        red = *new Colour(0xffef233c);
        darkRed = *new Colour(0xffd90429);
    };
    
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        const float radius = jmin (width / 2, height / 2) -15.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // fill
        g.setColour (darkBlue);
        g.fillEllipse (rx, ry, rw, rw);
        // outline
        g.setColour (lightBlue);
        g.drawEllipse (rx, ry, rw, rw, 5.0f);
        
        Path p;
        const float pointerLength = radius * 0.5f;
        const float pointerThickness = 5.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
        
        // pointer
        g.setColour(red);
        g.fillPath(p);
        
        g.setColour(lightBlue);
        const int numLines = 10;
        const float lineThickness = pointerThickness * 0.5f;
        const float lineAngleStep = (rotaryEndAngle - rotaryStartAngle) / numLines;
        const float lineLength = pointerLength * 0.5f;
        for(int i = 1; i < numLines; i++){
            if(i != ((numLines/2))){
                Path line;
                const float lineAngle = rotaryStartAngle + (i * lineAngleStep);
                line.addRectangle(-lineThickness * 0.5f, -radius - lineLength - 5.0f, lineThickness, lineLength);
                line.applyTransform(AffineTransform::rotation(lineAngle).translated(centreX, centreY));
                
                // lines
                g.fillPath(line);
            }
        }
        
        g.setFont(11.0f);
        std::string labelStart = std::to_string((int)slider.getMinimum());
        std::string labelEnd = std::to_string((int)slider.getMaximum());
        std::string labelMid = std::to_string((int)((slider.getMinimum() + slider.getMaximum()) * 0.5));
        g.drawFittedText(labelStart, 0, height - 15, 20, 11, Justification::right, 1);
        g.drawFittedText(labelEnd, width - 20, height - 15, 20, 11, Justification::left, 1);
        g.drawFittedText(labelMid, centreX - 10, 0, 20, 11, Justification::centred, 1);
    }
};

#endif /* CustomLookAndFeel_hpp */
