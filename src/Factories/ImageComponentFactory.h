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

        const auto filename = metadata.file.upToLastOccurrenceOf(".", false, false);
        const auto extension = metadata.file.fromLastOccurrenceOf(".", true, false);
        const auto maskFilename = filename + "_mask" + extension;
        juce::Image maskImage = BinaryAssetLoading::loadImageFromBinaryData(BinaryAssetLoading::getResourceName(maskFilename));
            
        // Create the image component with the loaded image
        return new ImageComponent(metadata.name, image, metadata, maskImage);
    }
};