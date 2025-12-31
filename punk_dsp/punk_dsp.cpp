#ifdef PUNK_DSP_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#include "punk_dsp.h"

// DSP C++ Files
#include "dsp/Dynamics/Compressor.cpp"
#include "dsp/Dynamics/Lifter.cpp"
#include "dsp/Dynamics/Gate.cpp"

#include "dsp/Distortion/Waveshaper.cpp"
#include "dsp/Distortion/TubeModel.cpp"

// GUI C++ Files
#include "gui/ExamplesLnF.cpp"

// UTILS C++ Files
#include "utils/PresetManager.cpp"