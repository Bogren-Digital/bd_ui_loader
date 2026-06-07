#include "PackedAssetImageLoader.h"
#if BD_UI_LOADER_HAS_PACKED_ASSETS

namespace BogrenDigital::UILoading
{
    PackedAssetImageLoader::PackedAssetImageLoader (std::shared_ptr<const pt::packedassets::PackedAssetSource> src)
        : source (std::move (src))
    {
    }

    PackedAssetImageLoader::~PackedAssetImageLoader() = default;

    juce::Image PackedAssetImageLoader::loadOne (const juce::String& filename) const
    {
        if (source == nullptr || filename.isEmpty())
        {
            return {};
        }

        const auto bytes = source->getBytes (filename.toRawUTF8());

        if (! bytes)
        {
            // Valid for optional resources like masks; mirror BinaryAssetImageLoader.
            return {};
        }

        return juce::ImageFileFormat::loadFrom (bytes->data(), bytes->size());
    }

    juce::Image PackedAssetImageLoader::loadImageByFilename (const juce::String& filename) const
    {
        return loadOne (filename);
    }

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        int numberOfFrames,
        const juce::String& fileSuffix) const
    {
        juce::OwnedArray<juce::Image> images;

        for (int i = 0; i < numberOfFrames; ++i)
        {
            images.add (new juce::Image (loadOne (filePrefix + juce::String (i) + fileSuffix)));
        }

        return images;
    }

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<int>& fileIndices,
        const juce::String& fileSuffix) const
    {
        juce::OwnedArray<juce::Image> images;

        for (const auto index : fileIndices)
        {
            images.add (new juce::Image (loadOne (filePrefix + juce::String (index) + fileSuffix)));
        }

        return images;
    }

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<juce::String>& fileNames,
        const juce::String& fileSuffix) const
    {
        juce::OwnedArray<juce::Image> images;

        for (const auto& fileName : fileNames)
        {
            images.add (new juce::Image (loadOne (filePrefix + fileName + fileSuffix)));
        }

        return images;
    }

    juce::String PackedAssetImageLoader::getStringFromAsset (const juce::String& filename) const
    {
        if (source == nullptr || filename.isEmpty())
        {
            return {};
        }

        const auto bytes = source->getBytes (filename.toRawUTF8());

        if (! bytes)
        {
            return {};
        }

        return juce::String::fromUTF8 (reinterpret_cast<const char*> (bytes->data()),
            static_cast<int> (bytes->size()));
    }
}
#endif
