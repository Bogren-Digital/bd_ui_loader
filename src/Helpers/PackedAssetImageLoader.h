#pragma once
#if __has_include(<playfultones_packedassets/playfultones_packedassets.h>)
    #include <playfultones_packedassets/playfultones_packedassets.h>
    #define BD_UI_LOADER_HAS_PACKED_ASSETS 1

    #include "ImageLoader.h"

namespace BogrenDigital::UILoading
{
    /**
     * @brief ImageLoader implementation backed by a packed-asset source.
     *
     * Looks assets up by their ORIGINAL filename (no name mangling): the pak
     * stores originals verbatim, so a sequence filename is simply
     * `filePrefix + index/name + fileSuffix`, looked up directly via
     * PackedAssetSource::getBytes. Missing assets yield an empty juce::Image
     * (mirroring BinaryAssetImageLoader's behaviour for optional resources).
     */
    struct PackedAssetImageLoader : public ImageLoader
    {
        explicit PackedAssetImageLoader (std::shared_ptr<const pt::packedassets::PackedAssetSource> src);
        ~PackedAssetImageLoader() override;

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
        [[nodiscard]] juce::Image loadOne (const juce::String& filename) const;

        std::shared_ptr<const pt::packedassets::PackedAssetSource> source;
    };
}
#endif
