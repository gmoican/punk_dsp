cmake_minimum_required(VERSION 3.25)

project("Punk DSP" VERSION 1.0.0)

# 1. Include JUCE (assuming it's a submodule or installed)
add_subdirectory(JUCE) 
add_subdirectory(clap-juce-extensions EXCLUDE_FROM_ALL)

# 2. Add your custom module to the JUCE search path
juce_add_module(PunkDspModule/punk_dsp)

# 3. Add the workbench plugin sub-folder
add_subdirectory(Workbench)