/*******************************************************************************
 BEGIN_JUCE_MODULE_DECLARATION

  ID:               punk_dsp
  vendor:           punkarra4
  version:          1.0.0
  name:             Punk DSP
  description:      Utility classes for DSP, GUI, and general utilities
  website:          https://github.com/gmoican/punk_dsp/
  license:          MIT License
  minimumCppStandard: 17

  dependencies:     juce_audio_basics, juce_gui_basics
  OSXFrameworks:
  iOSFrameworks:
  linuxLibs:
  windowsLibs:

 END_JUCE_MODULE_DECLARATION
*******************************************************************************/

#pragma once

// Module namespace
namespace punk_dsp
{
    // --- DSP ---
    // Dynamics
    #include "source/dsp/Dynamics/Compressor.h"
    #include "source/dsp/Dynamics/Lifter.h"
    #include "source/dsp/Dynamics/Gate.h"

    // Distortion
    #include "source/dsp/Distortion/Waveshaper.h"

    // --- GUI ---
    #include "source/gui/LevelMeter.h"

    // --- UTILS ---
    #include "source/utils/PresetManager.h"
}