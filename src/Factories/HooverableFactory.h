#pragma once

namespace BogrenDigital::UILoading {

/** @brief Factory for creating HooverableSwitchComponent instances with hover states. */
class HooverableFactory : public ComponentFactory
{
public:
    explicit HooverableFactory(BinaryAssetImageLoader& imgLoader)
        : ComponentFactory(imgLoader)
    {
    }

    juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
    {
        juce::Component* component = nullptr;
        auto buttonImages = juce::OwnedArray<juce::Image>();

        if (metadata.numberOfFrames > 1)
        {
            juce::Array<int> frameIndices;
            for (int i = 0; i < metadata.numberOfFrames; ++i)
                frameIndices.add(i);

            auto loadedImages = imageLoader.loadImageSequence(metadata.fileNamePrefix, frameIndices, metadata.fileNameSuffix);
            for (auto* image : loadedImages)
                buttonImages.add(new juce::Image(*image));
        }
        else
        {
            juce::String imageName = metadata.name + metadata.fileNameSuffix;
            juce::Image image = imageLoader.loadImageByFilename(imageName);
            buttonImages.add(new juce::Image(image));
        }

        if (buttonImages.size() > 0)
        {
            juce::Image hitboxMask;
            if (metadata.hitboxMask.isNotEmpty())
            {
                hitboxMask = imageLoader.loadImageByFilename(metadata.hitboxMask);
            }

            component = new HooverableSwitchComponent(metadata.name, buttonImages, metadata, hitboxMask);
        }
        else
        {
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};

} // namespace BogrenDigital::UILoading