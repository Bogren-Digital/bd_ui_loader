#pragma once

namespace BogrenDigital::UILoading
{

/** @brief Factory for creating RadioButtonGroup instances from filmstrip images. */
class RadioButtonGroupFactory : public ComponentFactory
{
public:
    explicit RadioButtonGroupFactory(ImageLoader& imgLoader)
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

            juce::Image hitboxMask;
            if (metadata.hitboxMask.isNotEmpty())
            {
                hitboxMask = imageLoader.loadImageByFilename(metadata.hitboxMask);
            }

            auto* group = new RadioButtonGroup(metadata.name, buttonImages, metadata, maskImage, hitboxMask);

            if (metadata.fileNameSuffix2x.isNotEmpty())
            {
                auto images1x = imageLoader.loadImageSequence(metadata.fileNamePrefix, frameIndices, metadata.fileNameSuffix);
                auto images2x = imageLoader.loadImageSequence(metadata.fileNamePrefix, frameIndices, metadata.fileNameSuffix2x);
                group->setScaledImageSet (std::make_unique<ScaledImageSet> (images1x, images2x));
            }

            component = group;
        }
        else
        {
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};

} // namespace BogrenDigital::UILoading