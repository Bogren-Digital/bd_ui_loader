#include "../../third_party/include/BS_thread_pool.hpp"

namespace BogrenDigital::UILoading
{
    struct BinaryAssetImageLoader::Impl
    {
        BS::thread_pool<> threadPool;

        Impl() : threadPool (static_cast<std::size_t> (juce::SystemStats::getNumCpus())) {}
    };

    BinaryAssetImageLoader::BinaryAssetImageLoader (const char* const* namedResourceListPtr,
        int numAssets,
        BinaryAssetUtilities::GetNamedResourceFunc getNamedResourcePtr,
        BinaryAssetUtilities::GetNamedResourceOriginalFilenameFunc getOriginalFilenamePtr)
        : BinaryAssetLoader (namedResourceListPtr, numAssets, getNamedResourcePtr, getOriginalFilenamePtr)
        , impl (std::make_unique<Impl>())
    {
    }

    BinaryAssetImageLoader::~BinaryAssetImageLoader() = default;

    juce::Image BinaryAssetImageLoader::loadImageFromBinaryData (const juce::String& resourceName) const
    {
        int dataSize = 0;
        const char* imageData = getNamedResource (resourceName.toRawUTF8(), dataSize);

        if (imageData == nullptr || dataSize <= 0)
        {
            // This is valid for optional resources like masks
            return {};
        }

        return juce::ImageFileFormat::loadFrom (imageData, static_cast<size_t> (dataSize));
    }

    juce::Image BinaryAssetImageLoader::loadImageByFilename (const juce::String& filename) const
    {
        if (filename.isEmpty())
        {
            return {};
        }

        return loadImageFromBinaryData (getResourceName (filename));
    }

    juce::OwnedArray<juce::Image> BinaryAssetImageLoader::loadImageSequenceFromFilenames (
        const std::vector<juce::String>& filenames) const
    {
        juce::OwnedArray<juce::Image> images;

        if (filenames.empty())
        {
            return images;
        }

        const int numImages = static_cast<int> (filenames.size());

        if (numImages <= 10)
        {
            for (const auto& filename : filenames)
            {
                if (auto image = loadImageByFilename (filename); image.isValid())
                {
                    images.add (new juce::Image (std::move (image)));
                }
            }

            return images;
        }

        std::vector<juce::Image> loadedImages (numImages);

        for (int i = 0; i < numImages; ++i)
        {
            impl->threadPool.detach_task ([this, &loadedImages, filename = filenames[i], index = i]
            {
                if (const auto image = loadImageByFilename (filename); image.isValid())
                {
                    loadedImages[index] = std::move (image);
                }
                else
                {
                    juce::Logger::writeToLog ("Could not load " + filename);
                }
            });
        }

        impl->threadPool.wait();

        for (auto& image : loadedImages)
        {
            if (image.isValid())
            {
                images.add (new juce::Image (std::move (image)));
            }
        }

        return images;
    }

    juce::OwnedArray<juce::Image> BinaryAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<int>& fileIndices,
        const juce::String& fileSuffix) const
    {
        std::vector<juce::String> filenames;
        filenames.reserve (fileIndices.size());

        for (const auto index : fileIndices)
        {
            filenames.push_back (filePrefix + juce::String (index) + fileSuffix);
        }

        return loadImageSequenceFromFilenames (filenames);
    }

    juce::OwnedArray<juce::Image> BinaryAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        int numberOfFrames,
        const juce::String& fileSuffix) const
    {
        juce::Array<int> indices;
        for (int i = 0; i < numberOfFrames; ++i)
        {
            indices.add (i);
        }

        return loadImageSequence (filePrefix, indices, fileSuffix);
    }

    juce::OwnedArray<juce::Image> BinaryAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<juce::String>& fileNames,
        const juce::String& fileSuffix) const
    {
        std::vector<juce::String> filenames;
        filenames.reserve (fileNames.size());

        for (const auto& fileName : fileNames)
        {
            filenames.push_back (filePrefix + fileName + fileSuffix);
        }

        return loadImageSequenceFromFilenames (filenames);
    }
}