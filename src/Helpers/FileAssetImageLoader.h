#pragma once

namespace BogrenDigital::UILoading
{
    /**
     * @brief Loads images and string assets from a filesystem directory.
     *
     * Drop-in alternative to BinaryAssetImageLoader for installations
     * that place UI assets in /Library/Application Support/ or similar.
     */
    struct FileAssetImageLoader : public ImageLoader
    {
        explicit FileAssetImageLoader (const juce::File& assetDirectory);
        ~FileAssetImageLoader() override;

        [[nodiscard]] juce::Image loadImageByFilename (const juce::String& filename) const override;

        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            int numberOfFrames,
            const juce::String& fileSuffix) const override;

        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            const juce::Array<int>& fileIndices,
            const juce::String& fileSuffix) const override;

        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            const juce::Array<juce::String>& fileNames,
            const juce::String& fileSuffix) const override;

        [[nodiscard]] juce::String getStringFromAsset (const juce::String& filename) const override;

    private:
        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequenceFromFilenames (
            const std::vector<juce::String>& filenames) const;

        juce::File assetDirectory;
    };
}
