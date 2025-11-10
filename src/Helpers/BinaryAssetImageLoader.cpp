#pragma once

namespace BogrenDigital::UILoading
{
    BinaryAssetImageLoader::BinaryAssetImageLoader (const char* const* namedResourceListPtr,
        int numAssets,
        BinaryAssetUtilities::GetNamedResourceFunc getNamedResourcePtr,
        BinaryAssetUtilities::GetNamedResourceOriginalFilenameFunc getOriginalFilenamePtr)
        : BinaryAssetLoader (namedResourceListPtr, numAssets, getNamedResourcePtr, getOriginalFilenamePtr)
    {
    }


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

    juce::OwnedArray<juce::Image> BinaryAssetImageLoader::loadImageSequence (
        const juce::String& filePrefix,
        const juce::Array<int>& fileIndices,
        const juce::String& fileSuffix) const
    {
        juce::OwnedArray<juce::Image> images;
        
        for (int index : fileIndices)
        {
            juce::String filename = filePrefix + juce::String (index) + fileSuffix;
            juce::Image image = loadImageByFilename (filename);
            
            if (image.isValid())
            {
                images.add (new juce::Image (image));
            }
        }
        
        return images;
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
        juce::OwnedArray<juce::Image> images;
        
        for (const auto& fileName : fileNames)
        {
            juce::String fullFileName = filePrefix + fileName + fileSuffix;
            juce::Image image = loadImageByFilename (fullFileName);
            
            if (image.isValid())
            {
                images.add (new juce::Image (image));
            }
        }
        
        return images;
    }
}