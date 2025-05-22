class KnobComponentFactory : public ComponentFactory
{
public:
    explicit KnobComponentFactory(UILoader::ComponentMetadata m)
        : ComponentFactory(std::move(m))
    {
    }

    juce::Component* createComponent() override
    {
        juce::Component* component = nullptr;
        
        // Create an array to hold all the knob frame images
        auto knobImages = juce::OwnedArray<juce::Image>();

        const auto maskImageName = metadata.fileNamePrefix + "mask" + metadata.fileNameSuffix;
        const auto maskImage = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(maskImageName));
            
        // Load all frame images
        for (int i = 0; i < metadata.numberOfFrames; ++i)
        {
            // Construct the image name: prefix + index + suffix
            juce::String imageName = metadata.fileNamePrefix + juce::String(i) + metadata.fileNameSuffix;
            juce::Image image = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(imageName));
            knobImages.add(new juce::Image(image));
        }
        
        // Create the knob component with the loaded images
        if (knobImages.size() > 0)
        {
            component = new KnobComponent(metadata.name, knobImages, metadata, maskImage);
            
            // Configure the slider ranges
            if (auto* knob = dynamic_cast<KnobComponent*>(component))
            {
                knob->setRange(0.0, 1.0);
                knob->setValue(0.5, juce::dontSendNotification);
            }
        }
        else
        {
            // Fallback if no images could be loaded
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};