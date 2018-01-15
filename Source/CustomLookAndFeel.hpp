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
    Colour black;
    Colour white;
    Colour red;

    CustomLookAndFeel() {
        black = *new Colour(0xff181818);
        white = *new Colour(0xffffffff);
        red = *new Colour(0xffCC3333);
    };

    void
    drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle,
                     const float rotaryEndAngle, Slider &slider) override {
        const float radius = jmin(width / 2, height / 2) - 15.0f;
        const float centreX = x + width * 0.5f;
        const float centreY = y + height * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(black);
        g.fillEllipse(rx, ry, rw, rw);
        // outline
        g.setColour(white);
        g.drawEllipse(rx, ry, rw, rw, 5.0f);

        Path p;
        const float pointerLength = radius * 0.5f;
        const float pointerThickness = 5.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(red);
        g.fillPath(p);

        g.setColour(white);
        const int numLines = 10;
        const float lineThickness = pointerThickness * 0.5f;
        const float lineAngleStep = (rotaryEndAngle - rotaryStartAngle) / numLines;
        const float lineLength = pointerLength * 0.5f;
        for (int i = 1; i < numLines; i++) {
            Path line;
            const float lineAngle = rotaryStartAngle + (i * lineAngleStep);
            line.addRectangle(-lineThickness * 0.5f, -radius - lineLength - 5.0f, lineThickness, lineLength);
            line.applyTransform(AffineTransform::rotation(lineAngle).translated(centreX, centreY));

            // lines
            g.fillPath(line);
        }

        g.setFont(11.0f);
        std::string labelStart = std::to_string((int) slider.getMinimum());
        std::string labelEnd = std::to_string((int) slider.getMaximum());
        g.drawFittedText(labelStart, 0, height - 15, 20, 11, Justification::right, 1);
        g.drawFittedText(labelEnd, width - 20, height - 15, 20, 11, Justification::left, 1);
    }

    void drawComboBox(Graphics &g, int width, int height, bool, int, int, int, int, ComboBox &box) override {
        const Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(white);
        g.fillRect(boxBounds.toFloat());

        g.setColour(black);
        g.drawRect(boxBounds.toFloat().reduced(0.5f, 0.5f));

        Rectangle<int> arrowZone(width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
        path.lineTo(static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
        path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

        g.setColour(black);
        g.strokePath(path, PathStrokeType(2.0f));

        box.setColour(ComboBox::textColourId, black);
    }

    void drawPopupMenuItem(Graphics &g, const Rectangle<int> &area, const bool isSeparator, const bool isActive,
                           const bool isHighlighted, const bool isTicked, const bool hasSubMenu, const String &text,
                           const String &shortcutKeyText, const Drawable *icon,
                           const Colour *const textColourToUse) override {
        auto textColour = (textColourToUse == nullptr ? findColour(PopupMenu::textColourId) : *textColourToUse);
        auto r = area.reduced(1);

        if (isHighlighted) {
            g.setColour(white);
        } else {
            g.setColour(black);
        }
        g.fillRect(r);
        g.setColour(black);

        r.reduce(jmin(5, area.getWidth() / 20), 0);
        auto font = getPopupMenuFont();
        const auto maxFontHeight = r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight) {
            font.setHeight(maxFontHeight);
        }

        g.setFont(font);

        auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight / 2)).toFloat();
        if (isTicked) {
            Path p;
            p.addEllipse(0, 0, 1, 1);
            if (isHighlighted) {
                g.setColour(black);
            } else {
                g.setColour(white);
            }
            g.fillPath(p, p.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        r.removeFromRight(3);

        if (isHighlighted) {
            g.setColour(black);
        } else {
            g.setColour(white);
        }
        g.drawFittedText(text, r, Justification::centredLeft, 1);
    }


    void drawButtonBackground(Graphics &g, Button &button, const Colour &backgroundColour, bool isMouseOverButton,
                              bool isButtonDown) override {

        const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

        auto baseColour = black.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.05f);

        g.setColour(baseColour);

        if (button.isConnectedOnLeft() || button.isConnectedOnRight()) {
            Path path;
            path.addRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());

            g.fillPath(path);

            g.setColour(white);
            g.strokePath(path, PathStrokeType(1.0f));
        } else {
            g.fillRect(bounds);

            g.setColour(white);
            g.drawRect(bounds, 1.0f);
        }
    }
};

#endif /* CustomLookAndFeel_hpp */
