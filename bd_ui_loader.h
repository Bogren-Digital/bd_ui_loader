/** BEGIN_JUCE_MODULE_DECLARATION
ID:               bd_ui_loader
vendor:           Bogren Digital
version:          1.0.0
name:             UI Loader
description:      A module for loading and displaying UI components based on generated UI metadata.
website:          https://bogrendigital.com
license:          Proprietary
dependencies:     juce_gui_basics, playfultones_smoothresizing
END_JUCE_MODULE_DECLARATION
*/
#pragma once
#define BD_UI_LOADER_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>
#include <playfultones_smoothresizing/playfultones_smoothresizing.h>
#include "BinaryData.h"

#include "src/UILoader.h"