#pragma once

namespace BogrenDigital::UILoading
{

class PlaceholderComponentFactory : public ComponentFactory
{
public:
    explicit PlaceholderComponentFactory(BinaryAssetImageLoader& imgLoader)
        : ComponentFactory(imgLoader)
    {
    }

    juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
    {
        return new PlaceholderComponent(metadata.name, metadata);
    }
};

} // namespace BogrenDigital::UILoading