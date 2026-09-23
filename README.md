# Transient Bloom

A multiband transient shaper with two shimmer reverbs (short and long). The reverbs are fed and gated only by the transients the shaper detects, so your snare crack can bloom while the kick, sub and drum bodies stay dry and tight.

Default settings are tuned for drum & bass at 170–175 BPM. There are also presets for jungle, neuro, liquid, halftime, hip-hop, rock and non-drum material.

---

## Get the plugin (no coding needed)

GitHub builds the Windows plugin for you for free. You only need to do this once.

1. **Make a free GitHub account** at https://github.com/signup.
2. **Create a new repository.** Click **+** (top right), then **New repository**. Name it `TransientBloom`, choose **Private** or Public, and click **Create repository**.
3. **Upload the project.** On the new repo page, click **uploading an existing file**. Unzip `TransientBloom.zip` on your PC, open the `TransientBloom` folder, select **everything inside it** (including the `.github` folder), and drag it into the browser. Then click **Commit changes**.
   - If Windows hides the `.github` folder, open File Explorer and turn on **View → Show → Hidden items**.
   - If the `.github` folder still doesn't upload: in your repo, click **Add file → Create new file**, name it `.github/workflows/build-windows.yml`, paste in the contents of that file from the zip, and commit.
4. **Wait for the build.** Open the **Actions** tab. A run called *Build Windows VST3* starts on its own and takes about 5–10 minutes. It's done when it shows a green tick.
5. **Download.** Click the finished run, scroll to **Artifacts**, and download **TransientBloom-Windows-VST3** (a zip).
6. **Install.** Unzip it and copy the **`Transient Bloom.vst3`** folder into
   `C:\Program Files\Common Files\VST3\`
7. **Rescan plugins in your DAW.**
   - FL Studio: Options → Manage plugins → Find plugins.
   - Ableton: Preferences → Plug-ins → Rescan.

   Transient Bloom then appears under Effects.

The zip also contains **`Transient Bloom.exe`**, a standalone version you can run without a DAW to try it quickly.

> Windows may warn that the plugin is from an "unknown publisher" because it isn't code-signed. That's normal for self-built plugins.

---

## How it works

```
input ─► crossover (2–4 bands, Linkwitz-Riley, phase-coherent)
          │
          ├─ band 1 ─► transient shaper ─┐
          ├─ band 2 ─► transient shaper ─┤──► sum ──► mix ──► + reverbs ──► output / soft clip
          ├─ band 3 ─► transient shaper ─┤
          └─ band 4 ─► transient shaper ─┘
                     │ (only the detected HIT of each band × Verb Send)
                     ▼
          ┌─ SHORT shimmer verb ─► gate ─┐
          └─ LONG  shimmer verb  ─► gate ─┴──► added to output
                ▲ gate opens only when a transient crosses the threshold
```

- **Transient shaper.** A level-independent differential-envelope detector, like a classic transient designer. Each band is shaped separately, and stereo detection is linked so the stereo image doesn't shift.
- **Reverb send = the hit.** Each band sends only its transient portion to the reverbs, meaning the part the shaper is boosting. Set **Verb Send** to 0% on the low band and the kick and sub never reach the reverb.
- **Gate.** The reverb outputs are gated by the same transient detector. Use **Hold** and **Release** for classic gated verb, or lower **Gate Depth** to let some of the natural tail through.
- **Clean shimmer.** An octave-up pitch shift sits inside the reverb's feedback loop, with a low cut on the input, damping in the loop, and a safety limiter so the tail can't run away.

## Controls

**Top bar**

| Control | What it does |
|---|---|
| Preset | 12 factory presets |
| Bands | 2, 3 or 4 bands |
| Soft Clip | Catches boosted peaks smoothly (on by default) |
| Shaper Mix | Blends the unshaped and shaped signal. It's phase-matched, so there's no comb filtering. The reverbs are always added on top. |
| Output | Final output level |

**Crossover display.** Drag the white handles to move the crossover points, and double-click a handle to reset it. The coloured bars show the live gain change per band: up means boosting, down means cutting.

**Per band**

| Control | What it does | DnB tip |
|---|---|---|
| Attack | Boosts (+) or cuts (−) the hit | Snare mid band +40–70% for crack |
| Sustain | Boosts (+) or cuts (−) the tail | −20 to −40% keeps rollers tight at 174 BPM |
| Atk Time | Length of the "hit" window | 2–6 ms = click and crack, 10–20 ms = fat kick punch |
| Sus Time | How far into the tail Sustain reaches | ~90–150 ms fits 16ths at 170–175 BPM |
| Verb Send | How much of this band's hit feeds the reverbs | 0% on the low band keeps the sub dry |
| Gain | Band output level | |
| Solo / Bypass | Listen to one band, or disable shaping on a band | |

**Short and long shimmer verbs:** Level, Size, Decay, Shimmer (octave-up amount), Tone (damping), Low Cut, Pre-Delay, Width, Threshold (how loud a transient must be to open the gate), Hold, Release and Gate Depth (100% is fully gated, lower lets the tail ring).

## Quick recipes

- **Crack the snare, keep the kick dry:** 3 bands, low band Verb Send 0%, mid band Attack +60%, Atk Time 4 ms. Short verb at Level 40%, Hold 40 ms, Release 70 ms.
- **Huge halftime snare:** Long verb at Size 90%, Decay 4 s, Hold 400 ms, Gate Depth 90%.
- **Gentle glue on a busy break:** negative Attack and slight positive Sustain on all bands, Shaper Mix 60%.

---

## For developers

- JUCE 8, CMake. `cmake -S . -B build && cmake --build build --config Release`
- The DSP is plain C++ in `Source/dsp/` with no JUCE dependency. There's a headless test in `tests/EngineTest.cpp`:
  `g++ -O2 -std=c++17 -ISource tests/EngineTest.cpp -o enginetest && ./enginetest`
- Passes pluginval at strictness level 10, including GUI tests.
- To build for macOS (AU/VST3), add `AU` to `TB_FORMATS` in CMakeLists.txt and build on a Mac or with a `macos-latest` runner.
