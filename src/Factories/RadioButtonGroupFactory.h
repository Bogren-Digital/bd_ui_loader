#pragma once

namespace BogrenDigital::UILoading
{

/** @brief Factory for creating RadioButtonGroup instances from filmstrip images. */
class RadioButtonGroupFactory : public ComponentFactory
{
public:
    explicit RadioButtonGroupFactory(BinaryAssetImageLoader& imgLoader)
        : ComponentFactory(imgLoader)
    {
    }

    juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
    {
        juce::Component* component = nullptr;
        auto buttonImages = juce::OwnedArray<juce::Image>();

        juce::Array<int> frameIndices;
        for (int i = 0; i < metadata.numberOfFrames; ++i)
            frameIndices.add(i);

        auto loadedImages = imageLoader.loadImageSequence(metadata.fileNamePrefix, frameIndices, metadata.fileNameSuffix);
        for (auto* image : loadedImages)
            buttonImages.add(new juce::Image(*image));

        if (buttonImages.size() > 0)
        {
            const auto maskImageName = metadata.fileNamePrefix + "mask" + metadata.fileNameSuffix;
            const auto maskImage = imageLoader.loadImageByFilename(maskImageName);

            component = new RadioButtonGroup(metadata.name, buttonImages, metadata, maskImage);
        }
        else
        {
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};

} // namespace BogrenDigital::UILoading