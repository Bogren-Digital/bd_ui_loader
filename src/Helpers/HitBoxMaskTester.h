#pragma once

namespace BogrenDigital::UILoading
{
    /**
     * @brief Stateless utility for testing if a point hits within a mask image.
     *
     * Used by components to implement custom hit testing based on mask images.
     * Scales coordinates from component space to mask image space and tests
     * if the mask has sufficient alpha at that position.
     */
    class HitBoxMaskTester
    {
    public:
        HitBoxMaskTester() = delete;

        /**
         * @brief Tests if a point is within the hitbox defined by a mask image.
         *
         * @param component The component to test
         * @param x The x coordinate to test (in component local coordinates)
         * @param y The y coordinate to test (in component local coordinates)
         * @param maskImage The mask image defining the hitbox (white/opaque = hit, transparent = no hit)
         * @param alphaThreshold Minimum alpha value (0-255) to consider a hit (default: 127)
         * @return true if the point is within the mask, false otherwise
         *
         * If maskImage is invalid, returns true (fallback to default rectangular hit test).
         * Coordinates are scaled proportionally from component bounds to mask dimensions.
         */
        static bool hitTest(const juce::Component& component,
                          int x,
                          int y,
                          const juce::Image& maskImage,
                          juce::uint8 alphaThreshold = 127)
        {

            const auto componentBounds = component.getLocalBounds();

            auto interceptsMouseClicks = false;
            auto interceptsMouseClicksInChildren = false;
            component.getInterceptsMouseClicks (interceptsMouseClicks, interceptsMouseClicksInChildren);

            if (!interceptsMouseClicks || !componentBounds.contains(x, y))
                return false;

            if (maskImage.isNull())
                return true;

            const float scaleX = static_cast<float>(maskImage.getWidth()) / static_cast<float>(componentBounds.getWidth());
            const float scaleY = static_cast<float>(maskImage.getHeight()) / static_cast<float>(componentBounds.getHeight());

            const int maskX = static_cast<int>(x * scaleX);
            const int maskY = static_cast<int>(y * scaleY);

            if (maskX < 0 || maskX >= maskImage.getWidth() || maskY < 0 || maskY >= maskImage.getHeight())
                return false;

            const auto pixel = maskImage.getPixelAt(maskX, maskY);
            return pixel.getAlpha() > alphaThreshold;
        }

    private:
        JUCE_DECLARE_NON_COPYABLE(HitBoxMaskTester)
    };

} // namespace BogrenDigital::UILoading
