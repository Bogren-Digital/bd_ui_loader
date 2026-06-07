#include <catch2/catch_test_macros.hpp>

#include <bd_ui_loader/bd_ui_loader.h>
#include <playfultones_packedassets/playfultones_packedassets.h>

// pack() / InputEntry are packer-side helpers not surfaced by the umbrella
// header (production code only consumes a PackedAssetSource). The test target
// adds the module root to its include path so this resolves.
#include "src/Packer.h"

#include <memory>
#include <vector>
#include <cstdint>

// The whole point of the loader is __has_include auto-detection. In this test
// target the packed module IS on the include path, so the gate must be on.
#ifndef BD_UI_LOADER_HAS_PACKED_ASSETS
    #error "BD_UI_LOADER_HAS_PACKED_ASSETS should be defined when packed module is present"
#endif

using namespace BogrenDigital::UILoading;

namespace
{
    // Encode a known-size PNG via JUCE so the expected dimensions are explicit.
    std::vector<uint8_t> encodePng (int w, int h)
    {
        juce::Image img (juce::Image::ARGB, w, h, true);
        juce::PNGImageFormat fmt;
        juce::MemoryOutputStream os;
        fmt.writeImageToStream (img, os);
        const auto* d = static_cast<const uint8_t*> (os.getData());
        return { d, d + os.getDataSize() };
    }

    // PackedAssetSource holds a NON-owning std::span over the pak bytes, so the
    // pak buffer must outlive the source. Keep the packed bytes in a heap buffer
    // (stable address) whose lifetime is tied to the returned source via the
    // shared_ptr's deleter, so the span never dangles.
    std::shared_ptr<const pt::packedassets::PackedAssetSource> buildSource (
        const std::vector<pt::packedassets::InputEntry>& entries)
    {
        pt::packedassets::Key key {};
        key[0] = 17;
        key[5] = 99;

        auto pak = std::make_shared<std::vector<uint8_t>> (pt::packedassets::pack (entries, key));
        auto* src = new pt::packedassets::PackedAssetSource (*pak, key);

        // Custom deleter keeps `pak` alive for as long as the source exists.
        return std::shared_ptr<const pt::packedassets::PackedAssetSource> (
            src, [pak] (const pt::packedassets::PackedAssetSource* p) { delete p; });
    }
}

TEST_CASE ("PackedAssetImageLoader decodes an image by original filename")
{
    const int w = 12, h = 7;
    std::vector<pt::packedassets::InputEntry> entries {
        { "Volume Knob_0.png", encodePng (w, h) },
        { "metadata.xml", { '<', 'h', 'e', 'l', 'l', 'o', '/', '>' } }
    };

    PackedAssetImageLoader loader (buildSource (entries));

    const auto image = loader.loadImageByFilename ("Volume Knob_0.png");
    REQUIRE (image.isValid());
    REQUIRE (image.getWidth() == w);
    REQUIRE (image.getHeight() == h);
}

TEST_CASE ("PackedAssetImageLoader looks up the ORIGINAL filename without mangling")
{
    // A BinaryData-style loader would mangle dots->underscores; the packed
    // loader must NOT. Confirm the dotted name resolves and a mangled one does not.
    const int w = 5, h = 9;
    std::vector<pt::packedassets::InputEntry> entries {
        { "knob.png", encodePng (w, h) }
    };

    PackedAssetImageLoader loader (buildSource (entries));

    REQUIRE (loader.loadImageByFilename ("knob.png").isValid());
    REQUIRE_FALSE (loader.loadImageByFilename ("knob_png").isValid()); // mangled -> miss
}

TEST_CASE ("PackedAssetImageLoader getStringFromAsset round-trips text")
{
    const juce::String xml = "<hello/>";
    std::vector<pt::packedassets::InputEntry> entries {
        { "metadata.xml", { '<', 'h', 'e', 'l', 'l', 'o', '/', '>' } }
    };

    PackedAssetImageLoader loader (buildSource (entries));

    REQUIRE (loader.getStringFromAsset ("metadata.xml") == xml);
    REQUIRE (loader.getStringFromAsset ("missing.xml").isEmpty());
}

TEST_CASE ("PackedAssetImageLoader loadImageSequence builds prefix+index+suffix names")
{
    const int w = 8, h = 4;
    std::vector<pt::packedassets::InputEntry> entries {
        { "Knob_0.png", encodePng (w, h) },
        { "Knob_1.png", encodePng (w, h) },
        { "Knob_2.png", encodePng (w, h) }
    };

    PackedAssetImageLoader loader (buildSource (entries));

    auto byCount = loader.loadImageSequence ("Knob_", 3, ".png");
    REQUIRE (byCount.size() == 3);
    for (auto* img : byCount)
    {
        REQUIRE (img->isValid());
        REQUIRE (img->getWidth() == w);
    }

    juce::Array<int> indices { 0, 2 };
    auto byIndices = loader.loadImageSequence ("Knob_", indices, ".png");
    REQUIRE (byIndices.size() == 2);
    REQUIRE (byIndices[0]->isValid());
    REQUIRE (byIndices[1]->isValid());

    juce::Array<juce::String> names { "0", "1" };
    auto byNames = loader.loadImageSequence ("Knob_", names, ".png");
    REQUIRE (byNames.size() == 2);
    REQUIRE (byNames[0]->isValid());
    REQUIRE (byNames[1]->isValid());
}

TEST_CASE ("PackedAssetImageLoader loadImageSequence drops missing frames")
{
    // Mirror the sibling ImageLoaders: missing entries are NOT added, so the
    // returned array size equals the count of PRESENT valid frames, not the
    // requested count. Consumers (e.g. KnobComponentFactory) rely on .size()
    // to decide frame count / @2x fallback.
    const int w = 8, h = 4;
    std::vector<pt::packedassets::InputEntry> entries {
        { "Knob_0.png", encodePng (w, h) },
        { "Knob_2.png", encodePng (w, h) }
        // Knob_1.png deliberately absent.
    };

    PackedAssetImageLoader loader (buildSource (entries));

    // Request 3 frames; only 2 are present -> size must be 2.
    auto byCount = loader.loadImageSequence ("Knob_", 3, ".png");
    REQUIRE (byCount.size() == 2);
    for (auto* img : byCount)
    {
        REQUIRE (img->isValid());
    }

    // Indices including a missing one drop it.
    juce::Array<int> indices { 0, 1, 2 };
    auto byIndices = loader.loadImageSequence ("Knob_", indices, ".png");
    REQUIRE (byIndices.size() == 2);

    // Names including a missing one drop it.
    juce::Array<juce::String> names { "0", "1", "2" };
    auto byNames = loader.loadImageSequence ("Knob_", names, ".png");
    REQUIRE (byNames.size() == 2);

    // A fully-missing sequence yields an empty array (size 0).
    auto allMissing = loader.loadImageSequence ("NoSuchKnob_", 5, ".png");
    REQUIRE (allMissing.isEmpty());
}

TEST_CASE ("PackedAssetImageLoader tolerates a null source")
{
    PackedAssetImageLoader loader (nullptr);
    REQUIRE_FALSE (loader.loadImageByFilename ("x.png").isValid());
    REQUIRE (loader.getStringFromAsset ("x.xml").isEmpty());
}
