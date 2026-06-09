#include "PackedAssetImageLoader.h"
#if BD_UI_LOADER_HAS_PACKED_ASSETS

namespace BogrenDigital::UILoading
{
    PackedAssetImageLoader::PackedAssetImageLoader (std::shared_ptr<const pt::packedassets::PackedAssetSource> src)
        : source (std::move (src))
    {
    }

    PackedAssetImageLoader::~PackedAssetImageLoader() = default;

    std::optional<std::vector<uint8_t>> PackedAssetImageLoader::fetchBytes (const juce::String& filename) const
    {
        if (source == nullptr || filename.isEmpty())
        {
            return std::nullopt;
        }

        return source->getBytes (filename.toRawUTF8());
    }

    juce::Image PackedAssetImageLoader::loadOne (const juce::String& filename) const
    {
        // Memoize decoded images, like BinaryAssetImageLoader/FileAssetImageLoader.
        // PackedAssetSource::getBytes re-decrypts on every call, and the UI
        // factories load the same filmstrip 2-3x at construction, so without this
        // each frame would be decrypted AND decoded repeatedly. Keyed on the
        // ORIGINAL filename (the pak stores originals verbatim).
        const auto hashCode = filename.hashCode64();

        if (auto cached = juce::ImageCache::getFromHashCode (hashCode); cached.isValid())
        {
            return cached;
        }

        const auto bytes = fetchBytes (filename);

        if (! bytes)
        {
            // Valid for optional resources like masks; matches the sibling loaders.
            return {};
        }

        auto image = juce::ImageFileFormat::loadFrom (bytes->data(), bytes->size());

        if (image.isValid())
        {
            juce::ImageCache::addImageToCache (image, hashCode);
        }

        return image;
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
            if (auto image = loadOne (filePrefix + juce::String (i) + fileSuffix); image.isValid())
            {
                images.add (new juce::Image (std::move (image)));
            }
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
            if (auto image = loadOne (filePrefix + juce::String (index) + fileSuffix); image.isValid())
            {
                images.add (new juce::Image (std::move (image)));
            }
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
            if (auto image = loadOne (filePrefix + fileName + fileSuffix); image.isValid())
            {
                images.add (new juce::Image (std::move (image)));
            }
        }

        return images;
    }

    juce::String PackedAssetImageLoader::getStringFromAsset (const juce::String& filename) const
    {
        const auto bytes = fetchBytes (filename);

        if (! bytes)
        {
            return {};
        }

        return juce::String::fromUTF8 (reinterpret_cast<const char*> (bytes->data()),
            static_cast<int> (bytes->size()));
    }
}
#endif
