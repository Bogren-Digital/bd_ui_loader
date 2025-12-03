#pragma once

namespace BogrenDigital::UILoading
{

    /** @brief Factory for creating KnobComponent instances from filmstrip images. */
    class KnobComponentFactory : public ComponentFactory
    {
    public:
        KnobComponentFactory(BinaryAssetImageLoader& imgLoader)
            : ComponentFactory(imgLoader)
        {
        }

        juce::Component* createComponent(const UILoader::ComponentMetadata& metadata) override
        {
            juce::Component* component = nullptr;

            auto knobImages = imageLoader.loadImageSequence(metadata.fileNamePrefix, metadata.numberOfFrames, metadata.fileNameSuffix);

            if (knobImages.size() > 0)
            {
                const auto maskImageName = metadata.fileNamePrefix + "mask" + metadata.fileNameSuffix;
                const auto maskImage = imageLoader.loadImageByFilename(maskImageName);

                juce::Image hitboxMask;
                if (metadata.hitboxMask.isNotEmpty())
                {
                    hitboxMask = imageLoader.loadImageByFilename(metadata.hitboxMask);
                }

                component = new KnobComponent(metadata.name, knobImages, metadata, maskImage, hitboxMask);

                if (auto* knob = dynamic_cast<KnobComponent*>(component))
                {
                    knob->setRange(0.0, 1.0);
                    knob->setValue(0.5, juce::dontSendNotification);
                }
            }
            else
            {
                component = new PlaceholderComponent(metadata.name, metadata);
            }

            return component;
        }
    };

} // namespace BogrenDigital::UILoading