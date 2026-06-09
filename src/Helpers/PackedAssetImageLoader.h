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
     *
     * Decoded images are memoized via juce::ImageCache, keyed on the original
     * filename's hashCode64() (matching BinaryAssetImageLoader/FileAssetImageLoader);
     * this matters because PackedAssetSource::getBytes re-decrypts on every call.
     * Unlike BinaryAssetImageLoader it does NOT parallelize large sequences across
     * a thread pool yet (sequences decode serially) -- see the follow-up issue.
     */
    struct PackedAssetImageLoader : public ImageLoader
    {
        /// NOTE: PackedAssetSource holds a NON-owning std::span over the pak bytes,
        /// so the buffer behind `src`'s span must outlive this loader. The production
        /// path (pt::packedassets::createDefaultSource()) maps with process-static
        /// lifetime and is safe; only callers wrapping a temporary pak need care.
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

        // Decode a sequence by full filename, in parallel across a shared thread
        // pool for >10 frames (matching BinaryAssetImageLoader). Invalid frames
        // are dropped. The three public overloads build the names and delegate.
        [[nodiscard]] juce::OwnedArray<juce::Image> loadImageSequenceFromFilenames (
            const std::vector<juce::String>& filenames) const;

        // Shared guard + fetch used by loadOne and getStringFromAsset: returns
        // nullopt for a null source / empty name, else the decrypted bytes.
        [[nodiscard]] std::optional<std::vector<uint8_t>> fetchBytes (const juce::String& filename) const;

        std::shared_ptr<const pt::packedassets::PackedAssetSource> source;
    };
}
#endif
