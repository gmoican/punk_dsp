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
#define PUNK_DSP_H_INCLUDED

// --- DSP ---
// Dynamics
#include "dsp/Dynamics/Compressor.h"
#include "dsp/Dynamics/Lifter.h"
#include "dsp/Dynamics/Gate.h"

// Distortion
#include "dsp/Distortion/Waveshaper.h"

// --- UTILS ---
#include "utils/PresetManager.h"
