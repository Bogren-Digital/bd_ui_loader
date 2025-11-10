#ifdef BD_UI_LOADER_H_INCLUDED
    /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
    #error "Incorrect use of module cpp file"
#endif

#include "bd_ui_loader/bd_ui_loader.h"

#include "src/Components/ComboBox.cpp"
#include "src/Components/HooverableSwitchComponent.cpp"
#include "src/Components/ImageComponent.cpp"
#include "src/Components/KnobComponent.cpp"
#include "src/Components/PlaceholderComponent.cpp"
#include "src/Components/RadioButtonGroup.cpp"
#include "src/Components/SwitchComponent.cpp"
#include "src/Components/TweenableComponent.cpp"

#include "src/Factories/ComboBoxFactory.cpp"
#include "src/Factories/ComponentFactoryRegistry.cpp"
#include "src/Factories/HooverableFactory.cpp"
#include "src/Factories/ImageComponentFactory.cpp"
#include "src/Factories/KnobComponentFactory.cpp"
#include "src/Factories/PlaceholderComponentFactory.cpp"
#include "src/Factories/RadioButtonGroupFactory.cpp"
#include "src/Factories/SwitchFactory.cpp"
#include "src/Factories/TweenableComponentFactory.cpp"
#include "src/Helpers/BinaryAssetImageLoader.cpp"
#include "src/UILoader.cpp"
