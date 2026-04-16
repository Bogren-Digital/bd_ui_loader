#pragma once

namespace BogrenDigital::UILoading {

class ComboBoxFactory : public ComponentFactory
{
public:
    explicit ComboBoxFactory(ImageLoader& imgLoader)
        : ComponentFactory(imgLoader)
    {
    }

    juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
    {
        return new ComboBox(metadata);
    }
};

} // namespace BogrenDigital::UILoading