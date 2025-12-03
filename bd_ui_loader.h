/** BEGIN_JUCE_MODULE_DECLARATION
ID:               bd_ui_loader
vendor:           Bogren Digital
version:          1.0.0
name:             UI Loader
description:      A module for loading and displaying UI components based on generated UI metadata.
website:          https://bogrendigital.com
license:          MIT
dependencies:     juce_gui_basics, playfultones_smoothresizing, bd_image_resampler, bd_binary_asset_utilities
END_JUCE_MODULE_DECLARATION
*/
#pragma once
#define BD_UI_LOADER_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>

#include <bd_binary_asset_utilities/bd_binary_asset_utilities.h>
#include <bd_image_resampler/bd_image_resampler.h>
#include <playfultones_smoothresizing/playfultones_smoothresizing.h>

#include "src/UILoader.h"

#include "src/Helpers/BinaryAssetImageLoader.h"
#include "src/Helpers/HitBoxMaskTester.h"

#include "src/Components/ComboBox.h"
#include "src/Components/HooverableSwitchComponent.h"
#include "src/Components/ImageComponent.h"
#include "src/Components/KnobComponent.h"
#include "src/Components/PlaceholderComponent.h"
#include "src/Components/RadioButtonGroup.h"
#include "src/Components/SwitchComponent.h"
#include "src/Components/TweenableComponent.h"

#include "src/Factories/ComponentFactory.h"

#include "src/Factories/PlaceholderComponentFactory.h"

#include "src/Factories/ComponentFactoryRegistry.h"

#include "src/Factories/ComboBoxFactory.h"
#include "src/Factories/HooverableFactory.h"
#include "src/Factories/ImageComponentFactory.h"
#include "src/Factories/KnobComponentFactory.h"
#include "src/Factories/RadioButtonGroupFactory.h"
#include "src/Factories/SwitchFactory.h"
#include "src/Factories/TweenableComponentFactory.h"