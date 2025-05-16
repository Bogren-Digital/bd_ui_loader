class HooverableFactory : public ComponentFactory
{
public:
    explicit HooverableFactory(UILoader::ComponentMetadata m)
        : ComponentFactory(std::move(m))
    {
    }

    juce::Component* createComponent() override
    {
        juce::Component* component = nullptr;
        
        // Create an array to hold all the button frame images
        auto buttonImages = juce::OwnedArray<juce::Image>();
            
        // Load all frame images
        for (int i = 0; i < metadata.numberOfFrames; ++i)
        {
            // Construct the image name: prefix + index + suffix
            juce::String imageName = metadata.numberOfFrames > 1 ? metadata.fileNamePrefix + juce::String(i) + metadata.fileNameSuffix : metadata.name + metadata.fileNameSuffix;
            juce::Image image = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(imageName));
            buttonImages.add(new juce::Image(image));
        }
        
        // Create the switch component with the loaded images
        if (buttonImages.size() > 0)
        {
            component = new HooverableSwitchComponent(metadata.name, buttonImages, metadata);
        }
        else
        {
            // Fallback if no images could be loaded
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};