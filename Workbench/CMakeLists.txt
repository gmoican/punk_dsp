list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (PamplejuceVersion)

set(PROJECT_NAME "Workbench")
set(MAJOR_VERSION 1)
set(CURRENT_VERSION 1.0.0)
set(COMPANY_NAME "punkarra4")
set(BUNDLE_ID "com.punkarra4.Workbench")

# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS AU VST3 Standalone)

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

juce_add_plugin("${PROJECT_NAME}"
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"

    COPY_PLUGIN_AFTER_BUILD TRUE

    PLUGIN_MANUFACTURER_CODE Punk

    PLUGIN_CODE P008
    FORMATS "${FORMATS}"

    PRODUCT_NAME "${PRODUCT_NAME} v${MAJOR_VERSION}"
    )

add_library(WorkbenchShared STATIC)

clap_juce_extensions_plugin(TARGET "${PROJECT_NAME}"
    CLAP_ID "${BUNDLE_ID}"
    CLAP_FEATURES audio-effect)

# Explicitly list source files (Better for IDEs and build stability)
target_sources(WorkbenchShared PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
)

include(XcodePrettify)

# Handle CLAP
clap_juce_extensions_plugin(TARGET "${PROJECT_NAME}"
    CLAP_ID "${BUNDLE_ID}"
    CLAP_FEATURES audio-effect)

# Link everything to the Shared Library
target_link_libraries(WorkbenchShared 
    PUBLIC
        punk_dsp              # Your custom module
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
    PRIVATE
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
)

# Link the plugin target to the shared code
target_link_libraries("${PROJECT_NAME}" PRIVATE WorkbenchShared)

# Definitions
target_compile_definitions(WorkbenchShared PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    PRODUCT_NAME_WITHOUT_VERSION="Workbench"
)
