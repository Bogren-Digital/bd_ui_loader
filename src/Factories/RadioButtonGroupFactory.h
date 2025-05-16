class RadioButtonGroupFactory : public ComponentFactory
{
public:
    explicit RadioButtonGroupFactory(UILoader::ComponentMetadata m)
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
            juce::String imageName = metadata.fileNamePrefix + juce::String(i) + metadata.fileNameSuffix;
            juce::Image image = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(imageName));
            buttonImages.add(new juce::Image(image));
        }
        
        // Create the radio button group component with the loaded images
        if (buttonImages.size() > 0)
        {
            component = new RadioButtonGroup(metadata.name, buttonImages, metadata);
        }
        else
        {
            // Fallback if no images could be loaded
            component = new PlaceholderComponent(metadata.name, metadata);
        }

        return component;
    }
};