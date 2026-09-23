#include "PluginEditor.h"
#include "Presets.h"

using namespace juce;

namespace ui
{
//==============================================================================
LookAndFeel::LookAndFeel()
{
    setColour (Slider::textBoxTextColourId, col::text);
    setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    setColour (Slider::textBoxBackgroundColourId, Colours::transparentBlack);
    setColour (Slider::textBoxHighlightColourId, col::band[2].withAlpha (0.4f));
    setColour (Label::textColourId, col::text);
    setColour (ComboBox::backgroundColourId, col::panelHi);
    setColour (ComboBox::outlineColourId, col::track);
    setColour (ComboBox::textColourId, col::text);
    setColour (ComboBox::arrowColourId, col::dim);
    setColour (PopupMenu::backgroundColourId, col::panel);
    setColour (PopupMenu::textColourId, col::text);
    setColour (PopupMenu::highlightedBackgroundColourId, col::panelHi);
    setColour (PopupMenu::highlightedTextColourId, col::band[2]);
    setColour (TooltipWindow::backgroundColourId, col::panelHi);
    setColour (TooltipWindow::textColourId, col::text);
    setColour (TooltipWindow::outlineColourId, col::track);
    setColour (TextEditor::backgroundColourId, col::panelHi);
    setColour (TextEditor::textColourId, col::text);
    setColour (TextEditor::highlightColourId, col::band[2].withAlpha (0.4f));
    setColour (CaretComponent::caretColourId, col::text);
}

void LookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float start, float end, Slider& s)
{
    const auto accent = s.findColour (Slider::rotarySliderFillColourId);
    const bool bipolar = (bool) s.getProperties().getWithDefault ("bipolar", false);
    auto area = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.0f);
    const float size = jmin (area.getWidth(), area.getHeight());
    area = area.withSizeKeepingCentre (size, size);
    const float r = size * 0.5f, cx = area.getCentreX(), cy = area.getCentreY();
    const float thick = jmax (3.0f, size * 0.07f);
    const float angle = start + pos * (end - start);

    // knob body
    g.setColour (col::panelHi);
    g.fillEllipse (area.reduced (thick * 1.8f));
    g.setColour (Colours::black.withAlpha (0.35f));
    g.drawEllipse (area.reduced (thick * 1.8f), 1.0f);

    Path track; track.addCentredArc (cx, cy, r - thick * 0.5f, r - thick * 0.5f, 0.0f, start, end, true);
    g.setColour (col::track);
    g.strokePath (track, PathStrokeType (thick, PathStrokeType::curved, PathStrokeType::rounded));

    const float from = bipolar ? (start + end) * 0.5f : start;
    if (std::abs (angle - from) > 0.001f)
    {
        Path val; val.addCentredArc (cx, cy, r - thick * 0.5f, r - thick * 0.5f, 0.0f, jmin (from, angle), jmax (from, angle), true);
        g.setColour (s.isEnabled() ? accent : col::dim);
        g.strokePath (val, PathStrokeType (thick, PathStrokeType::curved, PathStrokeType::rounded));
    }

    // pointer
    const float inner = r - thick * 2.6f;
    Point<float> p1 (cx + inner * 0.35f * std::sin (angle), cy - inner * 0.35f * std::cos (angle));
    Point<float> p2 (cx + inner * std::sin (angle),         cy - inner * std::cos (angle));
    g.setColour (col::text.withAlpha (s.isEnabled() ? 0.9f : 0.3f));
    g.drawLine ({ p1, p2 }, jmax (2.0f, size * 0.035f));
}

void LookAndFeel::drawToggleButton (Graphics& g, ToggleButton& b, bool highlighted, bool)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.5f);
    const auto accent = b.findColour (ToggleButton::tickColourId);
    const bool on = b.getToggleState();
    g.setColour (on ? accent.withAlpha (0.9f) : (highlighted ? col::panelHi.brighter (0.1f) : col::panelHi));
    g.fillRoundedRectangle (r, r.getHeight() * 0.5f);
    g.setColour (on ? accent : col::track);
    g.drawRoundedRectangle (r, r.getHeight() * 0.5f, 1.0f);
    g.setColour (on ? col::bg : (b.isEnabled() ? col::text : col::dim));
    g.setFont (FontOptions (12.0f, Font::bold));
    g.drawFittedText (b.getButtonText(), b.getLocalBounds(), Justification::centred, 1);
}

Label* LookAndFeel::createSliderTextBox (Slider& s)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (s);
    l->setFont (FontOptions (12.0f));
    l->setColour (Label::textColourId, col::dim);
    l->setColour (Label::outlineColourId, Colours::transparentBlack);
    l->setColour (Label::backgroundColourId, Colours::transparentBlack);
    return l;
}

Font LookAndFeel::getComboBoxFont (ComboBox&) { return FontOptions (14.0f); }
Font LookAndFeel::getPopupMenuFont()          { return FontOptions (14.0f); }

//==============================================================================
Knob::Knob (AudioProcessorValueTreeState& s, const String& id, const String& cap, Colour accent, bool bipolar, const String& tip)
{
    slider.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (Slider::TextBoxBelow, false, 80, 16);
    slider.setColour (Slider::rotarySliderFillColourId, accent);
    slider.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    slider.setColour (Slider::textBoxBackgroundColourId, Colours::transparentBlack);
    slider.setColour (Slider::textBoxTextColourId, col::dim);
    slider.getProperties().set ("bipolar", bipolar);
    slider.setTooltip (tip);
    slider.setPopupMenuEnabled (true);
    addAndMakeVisible (slider);

    caption.setText (cap, dontSendNotification);
    caption.setJustificationType (Justification::centred);
    caption.setFont (FontOptions (12.5f, Font::bold));
    caption.setColour (Label::textColourId, col::text.withAlpha (0.85f));
    caption.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (caption);

    att = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (s, id, slider);
    // double-click resets to default
    if (auto* p = s.getParameter (id))
        slider.setDoubleClickReturnValue (true, p->convertFrom0to1 (p->getDefaultValue()));
}

void Knob::resized()
{
    auto r = getLocalBounds();
    caption.setBounds (r.removeFromTop (16));
    slider.setBounds (r);
}

Toggle::Toggle (AudioProcessorValueTreeState& s, const String& id, const String& label, Colour accent, const String& tip)
{
    setButtonText (label);
    setColour (ToggleButton::tickColourId, accent);
    setTooltip (tip);
    att = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment> (s, id, *this);
}

//==============================================================================
static const float kMinF = 20.0f, kMaxF = 20000.0f;

CrossoverDisplay::CrossoverDisplay (TransientBloomProcessor& p) : proc (p)
{
    const char* ids[3] = { "x1", "x2", "x3" };
    for (int i = 0; i < 3; ++i)
    {
        xp[i] = proc.state.getParameter (ids[i]);
        att[i] = std::make_unique<ParameterAttachment> (*xp[i], [this, i] (float v) { xv[i] = v; repaint(); });
        att[i]->sendInitialUpdate();
    }
    setTooltip ("Drag the handles to move the crossover points. Double-click a handle to reset it.");
}

float CrossoverDisplay::freqToX (float f) const
{
    return (float) getWidth() * std::log (f / kMinF) / std::log (kMaxF / kMinF);
}
float CrossoverDisplay::xToFreq (float x) const
{
    return kMinF * std::pow (kMaxF / kMinF, jlimit (0.0f, 1.0f, x / (float) getWidth()));
}
int CrossoverDisplay::numBands() const { return 2 + (int) proc.state.getRawParameterValue ("bands")->load(); }

int CrossoverDisplay::handleAt (float x) const
{
    int best = -1; float bestD = 10.0f;
    for (int i = 0; i < numBands() - 1; ++i)
    {
        const float d = std::abs (freqToX (xv[i]) - x);
        if (d < bestD) { bestD = d; best = i; }
    }
    return best;
}

void CrossoverDisplay::tick()
{
    for (int b = 0; b < 4; ++b)
    {
        const float v = proc.engine.meterGainDb[b].load();
        meter[b] = std::abs (v) > std::abs (meter[b]) ? v : meter[b] * 0.85f + v * 0.15f; // fast rise, slow fall
    }
    repaint();
}

void CrossoverDisplay::paint (Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (col::panel);
    g.fillRoundedRectangle (r, 8.0f);

    const int nb = numBands();
    float edges[5] = { 0.0f };
    for (int i = 0; i < nb - 1; ++i) edges[i + 1] = freqToX (xv[i]);
    edges[nb] = r.getWidth();

    const float midY = r.getCentreY();
    const float dbToPx = (r.getHeight() * 0.42f) / 18.0f;

    for (int b = 0; b < nb; ++b)
    {
        auto band = Rectangle<float> (edges[b], 0.0f, jmax (0.0f, edges[b + 1] - edges[b]), r.getHeight());
        g.setColour (col::band[b].withAlpha (0.10f));
        g.fillRect (band);

        // live gain bar: up = transient boost, down = cut
        const float px = jlimit (-r.getHeight() * 0.45f, r.getHeight() * 0.45f, meter[b] * dbToPx);
        auto bar = band.reduced (jmin (18.0f, band.getWidth() * 0.25f), 0.0f);
        bar = px >= 0 ? bar.withTop (midY - px).withBottom (midY) : bar.withTop (midY).withBottom (midY - px);
        g.setColour (col::band[b].withAlpha (0.55f));
        g.fillRect (bar);

        g.setColour (col::band[b]);
        g.setFont (FontOptions (11.0f, Font::bold));
        g.drawText (String (meter[b] >= 0 ? "+" : "") + String (meter[b], 1) + " dB",
                    band.withHeight (18.0f).translated (0, 4.0f).toNearestInt(), Justification::centred);
    }

    // frequency grid
    g.setFont (FontOptions (10.0f));
    for (float f : { 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f })
    {
        const float x = freqToX (f);
        g.setColour (Colours::white.withAlpha (0.05f));
        g.drawVerticalLine ((int) x, 0.0f, r.getHeight());
        g.setColour (col::dim.withAlpha (0.7f));
        g.drawText (f >= 1000 ? String ((int) (f / 1000)) + "k" : String ((int) f),
                    Rectangle<float> (x + 3, r.getBottom() - 15, 30, 12).toNearestInt(), Justification::left);
    }
    g.setColour (Colours::white.withAlpha (0.12f));
    g.drawHorizontalLine ((int) midY, 0.0f, r.getWidth());

    // handles
    for (int i = 0; i < nb - 1; ++i)
    {
        const float x = freqToX (xv[i]);
        const bool hot = (i == dragging || i == hover);
        g.setColour (col::text.withAlpha (hot ? 1.0f : 0.6f));
        g.fillRect (x - 1.0f, 0.0f, 2.0f, r.getHeight());
        g.fillRoundedRectangle (x - 6.0f, midY - 14.0f, 12.0f, 28.0f, 4.0f);
        g.setColour (col::bg);
        g.setFont (FontOptions (11.0f, Font::bold));
        const String label = xv[i] >= 1000 ? String (xv[i] / 1000.0f, 2) + " kHz" : String ((int) xv[i]) + " Hz";
        g.setColour (col::text);
        g.drawText (label, Rectangle<float> (x + 8, midY - 8, 70, 16).toNearestInt(), Justification::left);
    }
}

void CrossoverDisplay::mouseDown (const MouseEvent& e)
{
    dragging = handleAt ((float) e.x);
    if (dragging >= 0) att[dragging]->beginGesture();
}

void CrossoverDisplay::mouseDrag (const MouseEvent& e)
{
    if (dragging < 0) return;
    const auto range = xp[dragging]->getNormalisableRange();
    const float f = jlimit (range.start, range.end, xToFreq ((float) e.x));
    att[dragging]->setValueAsPartOfGesture (f);
}

void CrossoverDisplay::mouseUp (const MouseEvent&)
{
    if (dragging >= 0) att[dragging]->endGesture();
    dragging = -1;
}

void CrossoverDisplay::mouseMove (const MouseEvent& e)
{
    const int h = handleAt ((float) e.x);
    if (h != hover) { hover = h; setMouseCursor (h >= 0 ? MouseCursor::LeftRightResizeCursor : MouseCursor::NormalCursor); repaint(); }
}

void CrossoverDisplay::mouseDoubleClick (const MouseEvent& e)
{
    const int h = handleAt ((float) e.x);
    if (h >= 0) att[h]->setValueAsCompleteGesture (xp[h]->convertFrom0to1 (xp[h]->getDefaultValue()));
}

//==============================================================================
BandColumn::BandColumn (AudioProcessorValueTreeState& s, int i)
    : index (i),
      attack  (s, pid::band (i, "atk"),   "ATTACK",   col::band[i], true,  "Boost (+) or cut (-) the hit/transient of this band."),
      sustain (s, pid::band (i, "sus"),   "SUSTAIN",  col::band[i], true,  "Boost (+) or cut (-) the tail/body after the hit."),
      atime   (s, pid::band (i, "atime"), "ATK TIME", col::band[i], false, "How long the 'hit' window lasts. Short (2-6 ms) = sharp click/crack; longer (10-30 ms) = fatter punch."),
      stime   (s, pid::band (i, "stime"), "SUS TIME", col::band[i], false, "How far into the tail the sustain control reaches. ~90-150 ms suits 170-175 BPM."),
      send    (s, pid::band (i, "send"),  "VERB SEND",col::verb[0], false, "How much of this band's transient is sent to the reverbs. Only the hit feeds the reverbs, not the whole signal."),
      gain    (s, pid::band (i, "gain"),  "GAIN",     col::band[i], true,  "Band output level."),
      solo    (s, pid::band (i, "solo"),  "SOLO",     col::band[i], "Listen to this band only."),
      bypass  (s, pid::band (i, "byp"),   "BYPASS",   col::dim,     "Disable shaping on this band.")
{
    for (auto* c : std::initializer_list<Component*> { &attack, &sustain, &atime, &stime, &send, &gain, &solo, &bypass })
        addAndMakeVisible (c);
}

void BandColumn::setActive (bool a)
{
    if (a == active) return;
    active = a;
    setEnabled (a);
    setAlpha (a ? 1.0f : 0.35f);
    repaint();
}

void BandColumn::paint (Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (col::panel);
    g.fillRoundedRectangle (r, 8.0f);
    g.setColour (col::band[index]);
    g.fillRoundedRectangle (r.removeFromTop (4.0f).reduced (10.0f, 0.0f), 2.0f);
    g.setFont (FontOptions (15.0f, Font::bold));
    g.drawText (active ? title : "OFF", getLocalBounds().removeFromTop (30).reduced (12, 0).translated (0, 4), Justification::centredLeft);
}

void BandColumn::resized()
{
    auto r = getLocalBounds().reduced (8);
    r.removeFromTop (28);
    auto buttons = r.removeFromTop (22);
    bypass.setBounds (buttons.removeFromRight (70));
    buttons.removeFromRight (6);
    solo.setBounds (buttons.removeFromRight (56));
    r.removeFromTop (4);

    auto big = r.removeFromTop (120);
    attack.setBounds (big.removeFromLeft (big.getWidth() / 2));
    sustain.setBounds (big);

    auto small = r;
    const int w = small.getWidth() / 4;
    atime.setBounds (small.removeFromLeft (w));
    stime.setBounds (small.removeFromLeft (w));
    send.setBounds (small.removeFromLeft (w));
    gain.setBounds (small);
}

//==============================================================================
VerbPanel::VerbPanel (AudioProcessorValueTreeState& s, int w) : which (w)
{
    const auto c = col::verb[w];
    struct K { const char* id; const char* cap; const char* tip; };
    const K ks[] = {
        { "level",   "LEVEL",     "Reverb return level." },
        { "size",    "SIZE",      "Room size." },
        { "decay",   "DECAY",     "Reverb tail length (RT60)." },
        { "shimmer", "SHIMMER",   "Octave-up shimmer fed back into the tail." },
        { "tone",    "TONE",      "High-frequency damping inside the reverb. Lower = darker." },
        { "lowcut",  "LOW CUT",   "Removes low end before the reverb so it stays clean and doesn't muddy the kick/sub." },
        { "predelay","PRE-DELAY", "Gap between the hit and the reverb. Keeps the transient clear." },
        { "width",   "WIDTH",     "Stereo width of the reverb." },
        { "thresh",  "THRESHOLD", "Transient level needed to open the gate. Lower = more hits trigger it." },
        { "hold",    "HOLD",      "How long the gate stays fully open after a hit." },
        { "release", "RELEASE",   "How fast the gate closes after the hold." },
        { "depth",   "GATE DEPTH","100% = hard gated reverb. Lower values let some of the natural tail through." },
    };
    for (auto& k : ks)
        addAndMakeVisible (knobs.add (new Knob (s, pid::verb (w, k.id), k.cap, c, false, k.tip)));
}

void VerbPanel::paint (Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (col::panel);
    g.fillRoundedRectangle (r, 8.0f);
    g.setColour (col::verb[which]);
    g.fillRoundedRectangle (r.removeFromTop (4.0f).reduced (10.0f, 0.0f), 2.0f);
    g.setFont (FontOptions (15.0f, Font::bold));
    g.drawText (which == 0 ? "SHORT SHIMMER VERB" : "LONG SHIMMER VERB", getLocalBounds().removeFromTop (30).reduced (12, 0).translated (0, 4), Justification::centredLeft);

    // gate LED
    auto led = ledArea.toFloat();
    g.setColour (col::track);
    g.fillEllipse (led);
    g.setColour (col::verb[which].withAlpha (0.15f + 0.85f * gate));
    g.fillEllipse (led.reduced (2.0f));
    g.setColour (col::dim);
    g.setFont (FontOptions (11.0f, Font::bold));
    g.drawText ("GATE", ledArea.translated (-40, 0).withWidth (36), Justification::centredRight);
}

void VerbPanel::resized()
{
    auto r = getLocalBounds().reduced (8);
    auto top = r.removeFromTop (28);
    ledArea = top.removeFromRight (18).withSizeKeepingCentre (14, 14).translated (-4, 2);
    r.removeFromTop (4);
    const int rowH = r.getHeight() / 2;
    for (int row = 0; row < 2; ++row)
    {
        auto line = r.removeFromTop (rowH);
        const int w = line.getWidth() / 6;
        for (int i = 0; i < 6; ++i)
            knobs[row * 6 + i]->setBounds (line.removeFromLeft (w));
    }
}
} // namespace ui

//==============================================================================
TransientBloomEditor::Content::Content (TransientBloomProcessor& p)
    : proc (p),
      mix  (p.state, "mix", "SHAPER MIX", ui::col::band[2], false, "Blend between the unshaped and shaped drums (phase-matched, no comb filtering). Reverbs are always added on top."),
      out  (p.state, "out", "OUTPUT",     ui::col::text,    true,  "Output level."),
      clip (p.state, "clip", "SOFT CLIP", ui::col::band[1], "Soft-clips the output so boosted transients can't clip harshly. Adds a little DnB-style grit when pushed."),
      xover (p),
      shortVerb (p.state, 0), longVerb (p.state, 1)
{
    const auto& ps = getFactoryPresets();
    for (int i = 0; i < (int) ps.size(); ++i) presets.addItem (ps[(size_t) i].name, i + 1);
    presets.setSelectedId (proc.getCurrentProgram() + 1, dontSendNotification);
    presets.setTooltip ("Factory presets");
    presets.onChange = [this]
    {
        const int idx = presets.getSelectedId() - 1;
        if (idx >= 0 && idx != proc.getCurrentProgram())
        {
            proc.setCurrentProgram (idx);
            proc.updateHostDisplay (juce::AudioProcessor::ChangeDetails().withProgramChanged (true));
        }
        else if (idx >= 0) proc.setCurrentProgram (idx); // re-select = reload
    };
    addAndMakeVisible (presets);

    bandsBox.addItemList ({ "2 Bands", "3 Bands", "4 Bands" }, 1);
    bandsBox.setTooltip ("Number of frequency bands");
    bandsAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (p.state, "bands", bandsBox);
    addAndMakeVisible (bandsBox);

    for (auto* c : std::initializer_list<juce::Component*> { &mix, &out, &clip, &xover, &shortVerb, &longVerb })
        addAndMakeVisible (c);
    for (int b = 0; b < tb::kMaxBands; ++b)
        addAndMakeVisible (columns.add (new ui::BandColumn (p.state, b)));
}

void TransientBloomEditor::Content::paint (juce::Graphics& g)
{
    using namespace juce;
    g.fillAll (ui::col::bg);
    auto header = getLocalBounds().removeFromTop (64).reduced (18, 0);
    g.setColour (ui::col::text);
    g.setFont (FontOptions (26.0f, Font::bold));
    g.drawText ("TRANSIENT", header, Justification::centredLeft);
    const int w = GlyphArrangement::getStringWidthInt (FontOptions (26.0f, Font::bold), "TRANSIENT ");
    g.setColour (ui::col::verb[0]);
    g.setFont (FontOptions (26.0f, Font::plain));
    g.drawText ("BLOOM", header.withTrimmedLeft (w), Justification::centredLeft);
    g.setColour (ui::col::dim);
    g.setFont (FontOptions (11.0f));
    g.drawText ("multiband transient shaper  +  transient-gated shimmer verbs", header.withTrimmedLeft (w + 90).withWidth (340), Justification::centredLeft);
}

void TransientBloomEditor::Content::resized()
{
    auto r = getLocalBounds();
    auto header = r.removeFromTop (64).reduced (16, 0);
    // right side of header: output, mix, clip, bands, presets
    out.setBounds (header.removeFromRight (74).withTrimmedTop (2));
    mix.setBounds (header.removeFromRight (80).withTrimmedTop (2));
    header.removeFromRight (8);
    clip.setBounds (header.removeFromRight (92).withSizeKeepingCentre (92, 26));
    header.removeFromRight (10);
    bandsBox.setBounds (header.removeFromRight (100).withSizeKeepingCentre (100, 28));
    header.removeFromRight (10);
    presets.setBounds (header.removeFromRight (200).withSizeKeepingCentre (200, 28));

    r = r.reduced (16, 0);
    r.removeFromTop (4);
    xover.setBounds (r.removeFromTop (110));
    r.removeFromTop (10);

    auto bandsArea = r.removeFromTop (270);
    const int gap = 10;
    const int cw = (bandsArea.getWidth() - gap * 3) / 4;
    for (int b = 0; b < 4; ++b)
    {
        columns[b]->setBounds (bandsArea.removeFromLeft (cw));
        bandsArea.removeFromLeft (gap);
    }
    r.removeFromTop (10);
    auto verbs = r.removeFromTop (256);
    const int vw = (verbs.getWidth() - 12) / 2;
    shortVerb.setBounds (verbs.removeFromLeft (vw));
    verbs.removeFromLeft (12);
    longVerb.setBounds (verbs);
}

//==============================================================================
TransientBloomEditor::TransientBloomEditor (TransientBloomProcessor& p)
    : AudioProcessorEditor (p), proc (p), content (p)
{
    setLookAndFeel (&lnf);
    addAndMakeVisible (content);
    content.setBounds (0, 0, baseW, baseH);

    setResizable (true, true);
    setResizeLimits (baseW * 6 / 10, baseH * 6 / 10, baseW * 2, baseH * 2);
    getConstrainer()->setFixedAspectRatio ((double) baseW / (double) baseH);
    setSize (baseW, baseH);

    refreshBandTitles();
    startTimerHz (30);
}

TransientBloomEditor::~TransientBloomEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void TransientBloomEditor::paint (juce::Graphics& g) { g.fillAll (ui::col::bg); }

void TransientBloomEditor::resized()
{
    const float s = (float) getWidth() / (float) baseW;
    content.setTransform (juce::AffineTransform::scale (s));
}

void TransientBloomEditor::refreshBandTitles()
{
    const int nb = 2 + (int) proc.state.getRawParameterValue ("bands")->load();
    static const char* names2[] = { "LOW", "HIGH" };
    static const char* names3[] = { "LOW", "MID", "HIGH" };
    static const char* names4[] = { "LOW", "LOW-MID", "HIGH-MID", "HIGH" };
    for (int b = 0; b < 4; ++b)
    {
        const char* n = b >= nb ? "OFF" : nb == 2 ? names2[b] : nb == 3 ? names3[b] : names4[b];
        content.columns[b]->setTitle (n);
        content.columns[b]->setActive (b < nb);
    }
    content.lastBands = nb;
}

void TransientBloomEditor::timerCallback()
{
    const int nb = 2 + (int) proc.state.getRawParameterValue ("bands")->load();
    if (nb != content.lastBands) refreshBandTitles();

    if (content.presets.getSelectedId() != proc.getCurrentProgram() + 1)
        content.presets.setSelectedId (proc.getCurrentProgram() + 1, juce::dontSendNotification);

    content.xover.tick();
    content.shortVerb.setGate (proc.engine.meterGate[0].load());
    content.longVerb.setGate (proc.engine.meterGate[1].load());
}
