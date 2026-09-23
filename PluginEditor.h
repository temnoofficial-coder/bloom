#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

namespace ui
{
namespace col
{
    const juce::Colour bg      { 0xff0e1014 };
    const juce::Colour panel   { 0xff171a21 };
    const juce::Colour panelHi { 0xff1f232c };
    const juce::Colour text    { 0xffdfe4ee };
    const juce::Colour dim     { 0xff7a8396 };
    const juce::Colour track   { 0xff2a2f3a };
    const juce::Colour band[4] { juce::Colour (0xffff6b5e), juce::Colour (0xffffb84d), juce::Colour (0xff3fe0c5), juce::Colour (0xff6fa8ff) };
    const juce::Colour verb[2] { juce::Colour (0xffb892ff), juce::Colour (0xffff7fd6) };
}

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
    juce::Label* createSliderTextBox (juce::Slider&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;
};

// Rotary knob + caption + parameter attachment
class Knob : public juce::Component
{
public:
    Knob (juce::AudioProcessorValueTreeState&, const juce::String& paramId, const juce::String& caption,
          juce::Colour accent, bool bipolar = false, const juce::String& tooltip = {});
    void resized() override;
    juce::Slider slider;
private:
    juce::Label caption;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
};

class Toggle : public juce::ToggleButton
{
public:
    Toggle (juce::AudioProcessorValueTreeState&, const juce::String& paramId, const juce::String& text,
            juce::Colour accent, const juce::String& tooltip = {});
private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> att;
};

// Frequency display with draggable crossover handles + live per-band gain meters
class CrossoverDisplay : public juce::Component, public juce::SettableTooltipClient
{
public:
    CrossoverDisplay (TransientBloomProcessor&);
    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    void tick();
private:
    float freqToX (float f) const;
    float xToFreq (float x) const;
    int   numBands() const;
    int   handleAt (float x) const;

    TransientBloomProcessor& proc;
    juce::RangedAudioParameter* xp[3] {};
    std::unique_ptr<juce::ParameterAttachment> att[3];
    float xv[3] { 150, 2500, 8000 };
    float meter[4] {};
    int dragging = -1, hover = -1;
};

class BandColumn : public juce::Component
{
public:
    BandColumn (juce::AudioProcessorValueTreeState&, int index);
    void paint (juce::Graphics&) override;
    void resized() override;
    void setTitle (const juce::String& t) { title = t; repaint(); }
    void setActive (bool a);
private:
    int index; juce::String title; bool active = true;
    Knob attack, sustain, atime, stime, send, gain;
    Toggle solo, bypass;
};

class VerbPanel : public juce::Component
{
public:
    VerbPanel (juce::AudioProcessorValueTreeState&, int which);
    void paint (juce::Graphics&) override;
    void resized() override;
    void setGate (float g) { if (std::abs (g - gate) > 0.01f) { gate = g; repaint (ledArea); } }
private:
    int which; float gate = 0.0f; juce::Rectangle<int> ledArea;
    juce::OwnedArray<Knob> knobs;
};
} // namespace ui

class TransientBloomEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit TransientBloomEditor (TransientBloomProcessor&);
    ~TransientBloomEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

    static constexpr int baseW = 1120, baseH = 730;

private:
    void timerCallback() override;
    void refreshBandTitles();

    struct Content : public juce::Component
    {
        explicit Content (TransientBloomProcessor&);
        void paint (juce::Graphics&) override;
        void resized() override;

        TransientBloomProcessor& proc;
        juce::ComboBox presets, bandsBox;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> bandsAtt;
        ui::Knob mix, out;
        ui::Toggle clip;
        ui::CrossoverDisplay xover;
        juce::OwnedArray<ui::BandColumn> columns;
        ui::VerbPanel shortVerb, longVerb;
        int lastBands = -1;
    };

    TransientBloomProcessor& proc;
    ui::LookAndFeel lnf;
    Content content;
    juce::TooltipWindow tooltips { this, 600 };
};
