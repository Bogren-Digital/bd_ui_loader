#pragma once

namespace BogrenDigital::UILoading
{
    /**
     * @brief Abstract interface for loading images and string assets.
     *
     * Concrete implementations load from JUCE BinaryData (BinaryAssetImageLoader)
     * or from a filesystem directory (FileAssetImageLoader).
     */
    struct ImageLoader
    {
        virtual ~ImageLoader() = default;

        [[nodiscard]] virtual juce::Image loadImageByFilename (const juce::String& filename) const = 0;

        [[nodiscard]] virtual juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            int numberOfFrames,
            const juce::String& fileSuffix) const = 0;

        [[nodiscard]] virtual juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            const juce::Array<int>& fileIndices,
            const juce::String& fileSuffix) const = 0;

        [[nodiscard]] virtual juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            const juce::Array<juce::String>& fileNames,
            const juce::String& fileSuffix) const = 0;

        [[nodiscard]] virtual juce::String getStringFromAsset (const juce::String& filename) const = 0;
    };
}
