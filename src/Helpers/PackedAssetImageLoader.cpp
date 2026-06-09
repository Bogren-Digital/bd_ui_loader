#include "PackedAssetImageLoader.h"
#if BD_UI_LOADER_HAS_PACKED_ASSETS

#include "../../third_party/include/BS_thread_pool.hpp"
#include <vector>

namespace BogrenDigital::UILoading
{
    namespace
    {
        // Process-wide pool shared by all PackedAssetImageLoader instances for
        // parallel sequence decode (mirrors BinaryAssetImageLoader's pool). Sized
        // to the CPU count; without it large filmstrips decode serially on one
        // core, which dominated cold-boot time vs the BinaryData path.
        struct PackedImageLoadingThreadPool
        {
            BS::thread_pool<> threadPool;
            PackedImageLoadingThreadPool()
                : threadPool (static_cast<std::size_t> (juce::SystemStats::getNumCpus())) {}
        };
        juce::SharedResourcePointer<PackedImageLoadingThreadPool> packedImageThreadPool;
    }

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

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequenceFromFilenames (
        const std::vector<juce::String>& filenames) const
    {
        juce::OwnedArray<juce::Image> images;
        const int n = static_cast<int> (filenames.size());
        if (n == 0)
            return images;

        // Small sequences: serial (the thread-pool dispatch isn't worth it).
        if (n <= 10)
        {
            for (const auto& f : filenames)
                if (auto image = loadOne (f); image.isValid())
                    images.add (new juce::Image (std::move (image)));
            return images;
        }

        // Large filmstrips: decode in parallel. loadOne is thread-safe -- getBytes
        // decrypts fresh with no shared mutable state, and juce::ImageCache is
        // internally synchronised.
        std::vector<juce::Image> decoded (static_cast<size_t> (n));
        for (int i = 0; i < n; ++i)
            packedImageThreadPool->threadPool.detach_task ([this, &decoded, f = filenames[(size_t) i], i] {
                if (auto image = loadOne (f); image.isValid())
                    decoded[(size_t) i] = std::move (image);
            });
        packedImageThreadPool->threadPool.wait();

        for (auto& image : decoded)
            if (image.isValid())
                images.add (new juce::Image (std::move (image)));

        return images;
    }

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        int numberOfFrames,
        const juce::String& fileSuffix) const
    {
        std::vector<juce::String> filenames;
        filenames.reserve (static_cast<size_t> (juce::jmax (0, numberOfFrames)));
        for (int i = 0; i < numberOfFrames; ++i)
            filenames.push_back (filePrefix + juce::String (i) + fileSuffix);
        return loadImageSequenceFromFilenames (filenames);
    }

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<int>& fileIndices,
        const juce::String& fileSuffix) const
    {
        std::vector<juce::String> filenames;
        filenames.reserve (static_cast<size_t> (fileIndices.size()));
        for (const auto index : fileIndices)
            filenames.push_back (filePrefix + juce::String (index) + fileSuffix);
        return loadImageSequenceFromFilenames (filenames);
    }

    juce::OwnedArray<juce::Image> PackedAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<juce::String>& fileNames,
        const juce::String& fileSuffix) const
    {
        std::vector<juce::String> filenames;
        filenames.reserve (static_cast<size_t> (fileNames.size()));
        for (const auto& fileName : fileNames)
            filenames.push_back (filePrefix + fileName + fileSuffix);
        return loadImageSequenceFromFilenames (filenames);
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
