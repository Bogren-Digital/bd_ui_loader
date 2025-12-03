#pragma once

namespace BogrenDigital::UILoading
{

/** @brief Factory for creating TweenableComponent instances with position callbacks. */
class TweenableComponentFactory : public ComponentFactory
{
public:
    explicit TweenableComponentFactory(BinaryAssetImageLoader& imgLoader, UILoader& loader)
        : ComponentFactory(imgLoader)
        , uiLoader(loader)
    {
    }

    juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
    {
        juce::Image image = imageLoader.loadImageByFilename(metadata.file);

        const auto filename = metadata.file.upToLastOccurrenceOf(".", false, false);
        const auto extension = metadata.file.fromLastOccurrenceOf(".", true, false);
        const auto maskFilename = filename + "_mask" + extension;
        juce::Image maskImage = imageLoader.loadImageByFilename(maskFilename);

        juce::Image hitboxMask;
        if (metadata.hitboxMask.isNotEmpty())
        {
            hitboxMask = imageLoader.loadImageByFilename(metadata.hitboxMask);
        }

        TweenableComponent* component = new TweenableComponent(metadata.name, image, metadata, maskImage, hitboxMask);
        UILoader* uiBuilder = &uiLoader;

        // Set up the callback for position updates when the normalized value changes
        component->onNewPositionNeeded = [uiBuilder, component](float normalizedValue)
        {
            juce::ignoreUnused(normalizedValue);

            if(component != nullptr && uiBuilder != nullptr)
            {
                uiBuilder->applyLayoutToComponent(component);
            }
            else
            {
                jassertfalse; // Component should not be null here
            }
        };

        return component;
    }

private:
    UILoader& uiLoader;
};

} // namespace BogrenDigital::UILoading