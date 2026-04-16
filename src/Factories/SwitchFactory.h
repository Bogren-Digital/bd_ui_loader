#pragma once

namespace BogrenDigital::UILoading
{

/** @brief Factory for creating SwitchComponent instances from filmstrip images. */
class SwitchFactory : public ComponentFactory
{
public:
    SwitchFactory(ImageLoader& imgLoader)
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

            juce::Image hitboxMask;
            if (metadata.hitboxMask.isNotEmpty())
            {
                hitboxMask = imageLoader.loadImageByFilename(metadata.hitboxMask);
            }

            auto* switchComp = new SwitchComponent(metadata.name, buttonImages, metadata, maskImage, hitboxMask);

            if (metadata.fileNameSuffix2x.isNotEmpty())
            {
                auto images1x = imageLoader.loadImageSequence(metadata.fileNamePrefix, metadata.numberOfFrames, metadata.fileNameSuffix);
                auto images2x = imageLoader.loadImageSequence(metadata.fileNamePrefix, metadata.numberOfFrames, metadata.fileNameSuffix2x);
                switchComp->setScaledImageSet (std::make_unique<ScaledImageSet> (images1x, images2x));
            }

            component = switchComp;
        }
        else
        {
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};

} // namespace BogrenDigital::UILoading