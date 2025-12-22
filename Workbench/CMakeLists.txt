# ==============================================================================
# 1. Project Setup
# ==============================================================================
set(PROJECT_NAME "Workbench")
set(MAJOR_VERSION 1)
set(CURRENT_VERSION 1.0.0)
set(COMPANY_NAME "punkarra4")
set(BUNDLE_ID "com.punkarra4.Workbench")

# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS AU VST3)

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# ==============================================================================
# 2. JUCE Plugin Declaration
# ==============================================================================
juce_add_plugin("${PROJECT_NAME}"
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"

    COPY_PLUGIN_AFTER_BUILD TRUE

    PLUGIN_MANUFACTURER_CODE Punk

    PLUGIN_CODE P008
    FORMATS "${FORMATS}"

    PRODUCT_NAME "${PRODUCT_NAME} v${MAJOR_VERSION}"
)

# ==============================================================================
# 3. Shared Code Library (The Engine)
# Using STATIC ensures code is compiled once and linked to all formats.
# ==============================================================================
add_library(WorkbenchShared STATIC)

# Explicitly list your files. Avoid GLOB if possible for build stability.
target_sources(WorkbenchShared PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
)

# ==============================================================================
# 4. CLAP Extension Integration
# ==============================================================================
if(TARGET clap-juce-extensions)
    clap_juce_extensions_plugin(TARGET "${PROJECT_NAME}"
        CLAP_ID "${BUNDLE_ID}"
        CLAP_FEATURES audio-effect)
endif()

# ==============================================================================
# 5. Compiler Settings & Definitions
# ==============================================================================
target_compile_definitions(WorkbenchShared PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    PRODUCT_NAME_WITHOUT_VERSION="Workbench"
)

# ==============================================================================
# 6. Linking (The Magic Line)
# ==============================================================================
target_link_libraries(WorkbenchShared 
    PUBLIC
        punk_dsp              # <--- This links your custom module!
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
    PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Link the plugin wrappers (VST3/AU/Standalone) to your Shared Code
target_link_libraries("${PROJECT_NAME}" PRIVATE WorkbenchShared)
