#pragma once

namespace BogrenDigital::UILoading {

/** @brief Factory for creating static ImageComponent instances. */
class ImageComponentFactory : public ComponentFactory
{
public:
    ImageComponentFactory(BinaryAssetImageLoader& imgLoader)
        : ComponentFactory(imgLoader)
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

        return new ImageComponent(metadata.name, image, metadata, maskImage, hitboxMask);
    }
};

} // namespace BogrenDigital::UILoading