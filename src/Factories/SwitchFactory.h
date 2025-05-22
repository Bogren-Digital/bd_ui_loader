class SwitchFactory : public ComponentFactory
{
public:
    explicit SwitchFactory(UILoader::ComponentMetadata m)
        : ComponentFactory(std::move(m))
    {
    }

    juce::Component* createComponent() override
    {
        juce::Component* component = nullptr;
        
        // Create an array to hold all the button frame images
        auto buttonImages = juce::OwnedArray<juce::Image>();
        const auto maskImageName = metadata.fileNamePrefix + "mask" + metadata.fileNameSuffix;
        const auto maskImage = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(maskImageName));
            
        // Load all frame images
        for (int i = 0; i < metadata.numberOfFrames; ++i)
        {
            // Construct the image name: prefix + index + suffix
            juce::String imageName = metadata.fileNamePrefix + juce::String(i) + metadata.fileNameSuffix;
            juce::Image image = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(imageName));
            buttonImages.add(new juce::Image(image));
        }
        
        // Create the switch component with the loaded images
        if (buttonImages.size() > 0)
        {
            component = new SwitchComponent(metadata.name, buttonImages, metadata, maskImage);
        }
        else
        {
            // Fallback if no images could be loaded
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};