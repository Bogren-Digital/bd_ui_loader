#pragma once

namespace BogrenDigital::UILoading {

struct BinaryAssetImageLoader;

/**
 * @brief Base interface for component factories.
 *
 * Factories create UI components from metadata. Each factory type
 * is responsible for creating a specific component type and loading
 * its required resources.
 */
class ComponentFactory
{
public:
    ComponentFactory(BinaryAssetImageLoader& imageLoader)
        : imageLoader(imageLoader)
    {
    }

    virtual ~ComponentFactory() = default;
    virtual juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) = 0;
protected:
    BinaryAssetImageLoader& imageLoader;
};

} // namespace BogrenDigital::UILoading