#pragma once

namespace BogrenDigital::UILoading
{
    /**
     * @brief Utility for loading images from BinaryData collections.
     * 
     * Inherits from BinaryAssetLoader to reuse core binary resource iteration logic.
     * Provides methods to load individual images or iterate through image collections.
     */
    struct BinaryAssetImageLoader : public BinaryAssetUtilities::BinaryAssetLoader
    {
        /**
         * @param namedResourceList Array of resource names
         * @param namedResourceListSize Number of elements in the namedResourceList array
         * @param getNamedResource Function to retrieve binary data by resource name
         * @param getOriginalFilename Function to retrieve original filename by resource name
         */
        BinaryAssetImageLoader (const char* const* namedResourceList,
            int namedResourceListSize,
            BinaryAssetUtilities::GetNamedResourceFunc getNamedResource,
            BinaryAssetUtilities::GetNamedResourceOriginalFilenameFunc getOriginalFilename);

        /**
         * @brief Loads an image from binary data by resource name.
         * 
         * @param resourceName The resource name (e.g., "Background_png")
         * @return The loaded image, or an empty image if not found
         */
        [[nodiscard]] juce::Image loadImageFromBinaryData (const juce::String& resourceName) const;

        /**
         * @brief Loads an image from binary data by filename.
         * 
         * This method converts the filename to a resource name and then loads the image.
         * 
         * @param filename The original filename (e.g., "Background.png")
         * @return The loaded image, or an empty image if not found
         */
        [[nodiscard]] juce::Image loadImageByFilename (const juce::String& filename) const;

        /**
         * @brief Loads multiple images matching a pattern.
         * 
         * Useful for loading animation frames or button states.
         * 
         * @param filePrefix The file prefix to match
         * @param fileIndices The indices to load (e.g., {0, 1, 2} for "button_0.png", "button_1.png", "button_2.png")
         * @param fileSuffix The file suffix (including extension)
         * @return An array of loaded images
         */
        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            const juce::Array<int>& fileIndices,
            const juce::String& fileSuffix) const;

        /**
         * @brief Loads multiple images matching a pattern with sequential numbering.
         * 
         * @param filePrefix The file prefix to match
         * @param numberOfFrames The number of frames to load (0 to numberOfFrames-1)
         * @param fileSuffix The file suffix (including extension)
         * @return An array of loaded images
         */
        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            int numberOfFrames,
            const juce::String& fileSuffix) const;

        /**
         * @brief Loads multiple images using specific filenames.
         * 
         * @param filePrefix The file prefix to prepend
         * @param fileNames Array of specific filenames (without prefix/suffix)
         * @param fileSuffix The file suffix (including extension) to append
         * @return An array of loaded images
         */
        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequence (
            const juce::String& filePrefix,
            const juce::Array<juce::String>& fileNames,
            const juce::String& fileSuffix) const;
    };
}