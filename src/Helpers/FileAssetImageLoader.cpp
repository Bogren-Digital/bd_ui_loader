namespace BogrenDigital::UILoading
{

FileAssetImageLoader::FileAssetImageLoader (const juce::File& directory)
    : assetDirectory (directory)
{
    jassert (assetDirectory.isDirectory());
}

FileAssetImageLoader::~FileAssetImageLoader() = default;

juce::Image FileAssetImageLoader::loadImageByFilename (const juce::String& filename) const
{
    if (filename.isEmpty())
        return {};

    const auto file = assetDirectory.getChildFile (filename);

    if (! file.existsAsFile())
        return {};

    const auto hashCode = file.getFullPathName().hashCode64();

    if (auto cachedImage = juce::ImageCache::getFromHashCode (hashCode); cachedImage.isValid())
        return cachedImage;

    auto image = juce::ImageFileFormat::loadFrom (file);

    if (image.isValid())
        juce::ImageCache::addImageToCache (image, hashCode);

    return image;
}

juce::String FileAssetImageLoader::getStringFromAsset (const juce::String& filename) const
{
    const auto file = assetDirectory.getChildFile (filename);

    if (! file.existsAsFile())
        return {};

    return file.loadFileAsString();
}

juce::OwnedArray<juce::Image> FileAssetImageLoader::loadImageSequenceFromFilenames (
    const std::vector<juce::String>& filenames) const
{
    juce::OwnedArray<juce::Image> images;

    for (const auto& filename : filenames)
    {
        if (auto image = loadImageByFilename (filename); image.isValid())
            images.add (new juce::Image (std::move (image)));
    }

    return images;
}

juce::OwnedArray<juce::Image> FileAssetImageLoader::loadImageSequence (
    const juce::String& filePrefix,
    int numberOfFrames,
    const juce::String& fileSuffix) const
{
    juce::Array<int> indices;
    for (int i = 0; i < numberOfFrames; ++i)
        indices.add (i);

    return loadImageSequence (filePrefix, indices, fileSuffix);
}

juce::OwnedArray<juce::Image> FileAssetImageLoader::loadImageSequence (
    const juce::String& filePrefix,
    const juce::Array<int>& fileIndices,
    const juce::String& fileSuffix) const
{
    std::vector<juce::String> filenames;
    filenames.reserve (fileIndices.size());

    for (const auto index : fileIndices)
        filenames.push_back (filePrefix + juce::String (index) + fileSuffix);

    return loadImageSequenceFromFilenames (filenames);
}

juce::OwnedArray<juce::Image> FileAssetImageLoader::loadImageSequence (
    const juce::String& filePrefix,
    const juce::Array<juce::String>& fileNames,
    const juce::String& fileSuffix) const
{
    std::vector<juce::String> filenames;
    filenames.reserve (fileNames.size());

    for (const auto& fileName : fileNames)
        filenames.push_back (filePrefix + fileName + fileSuffix);

    return loadImageSequenceFromFilenames (filenames);
}

} // namespace BogrenDigital::UILoading
