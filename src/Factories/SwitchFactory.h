#pragma once

namespace BogrenDigital::UILoading
{

/** @brief Factory for creating SwitchComponent instances from filmstrip images. */
class SwitchFactory : public ComponentFactory
{
public:
    SwitchFactory(BinaryAssetImageLoader& imgLoader)
        : ComponentFactory(imgLoader)
    {
    }

    juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
    {
        juce::Component* component = nullptr;

        auto buttonImages = imageLoader.loadImageSequence(metadata.fileNamePrefix, metadata.numberOfFrames, metadata.fileNameSuffix);

        if (buttonImages.size() > 0)
        {
            const auto maskImageName = metadata.fileNamePrefix + "mask" + metadata.fileNameSuffix;
            const auto maskImage = imageLoader.loadImageByFilename(maskImageName);

            component = new SwitchComponent(metadata.name, buttonImages, metadata, maskImage);
        }
        else
        {
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};

} // namespace BogrenDigital::UILoading