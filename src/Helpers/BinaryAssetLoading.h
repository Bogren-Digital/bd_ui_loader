namespace BinaryAssetLoading
{
    static inline juce::String getResourceName(const juce::String& filename)
    {
        // Convert filename to BinaryData resource name (e.g., "Background.png" to "Background_png")
        return filename.replaceCharacter('.', '_');
    }

    static inline juce::Image loadImageFromBinaryData(const juce::String& resourceName)
    {
        // Get the image data from BinaryData
        int dataSize = 0;
        const char* imageData = BinaryData::getNamedResource(resourceName.toRawUTF8(), dataSize);
        
        // Create image from the binary data
        return (imageData != nullptr && dataSize > 0) ? 
            juce::ImageFileFormat::loadFrom(imageData, (size_t)dataSize) : 
            juce::Image();
    }
}