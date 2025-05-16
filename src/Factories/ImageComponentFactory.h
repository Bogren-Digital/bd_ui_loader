class ImageComponentFactory : public ComponentFactory
{
public:
    explicit ImageComponentFactory(UILoader::ComponentMetadata m)
        : ComponentFactory(std::move(m))
    {
    }

    juce::Component* createComponent() override
    {
        juce::Image image = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(metadata.file));
            
        // Create the image component with the loaded image
        return new ImageComponent(metadata.name, image, metadata);
    }
};