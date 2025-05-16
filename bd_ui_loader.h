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
#include "src/Helpers/OriginalSizeReporter.h"
#include "src/Helpers/BinaryAssetLoading.h"
#include "src/Components/ImageComponent.h"
#include "src/Components/KnobComponent.h"
#include "src/Components/PlaceholderComponent.h"
#include "src/Components/TweenableComponent.h"
#include "src/Components/RadioButtonGroup.h"
#include "src/Factories/ComponentFactory.h"
#include "src/Factories/KnobComponentFactory.h"
#include "src/Factories/RadioButtonGroupFactory.h"
#include "src/Factories/TweenableComponentFactory.h"
#include "src/Factories/ImageComponentFactory.h"
#include "src/Factories/PlaceholderComponentFactory.h"