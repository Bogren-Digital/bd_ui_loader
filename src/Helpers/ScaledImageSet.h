#pragma once

namespace BogrenDigital::UILoading
{

/**
 * @brief Holds 1x and 2x image variants, selecting which to draw based on display scale.
 *
 * At paint time, detects the component's display scale factor.
 * Scale > 1.0 draws from the 2x set; scale <= 1.0 draws from the 1x set.
 * Images are drawn with stretchToFit into the component's float bounds
 * (stored in component properties by UILoader).
 */
class ScaledImageSet
{
public:
    ScaledImageSet (juce::OwnedArray<juce::Image>& images1xToUse,
                    juce::OwnedArray<juce::Image>& images2xToUse)
    {
        images1x.swapWith (images1xToUse);
        images2x.swapWith (images2xToUse);
    }

    /** Single-image constructor for IMAGE components (Background, etc.) */
    ScaledImageSet (const juce::Image& image1x, const juce::Image& image2x)
    {
        images1x.add (new juce::Image (image1x));
        images2x.add (new juce::Image (image2x));
    }

    void drawImage (juce::Graphics& g, int imageIndex, juce::Component& component)
    {
        auto& imageSet = shouldUse2x (component) ? images2x : images1x;

        if (imageIndex < 0 || imageIndex >= imageSet.size())
            return;

        auto* image = imageSet[imageIndex];
        if (image == nullptr || ! image->isValid())
            return;

        g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
        g.drawImage (*image, getFloatRect (component), juce::RectanglePlacement::stretchToFit);
    }

    bool hasImages() const { return images1x.size() > 0; }
    int size() const { return images1x.size(); }

private:
    static bool shouldUse2x (juce::Component& component)
    {
        auto& displays = juce::Desktop::getInstance().getDisplays();

        if (component.isShowing() && component.getScreenBounds().getWidth() > 0)
        {
            if (const auto* display = displays.getDisplayForPoint (component.getScreenBounds().getCentre()))
                return display->scale > 1.0;
        }

        if (auto* topLevel = component.getTopLevelComponent())
        {
            if (topLevel->isOnDesktop() && topLevel->getScreenBounds().getWidth() > 0)
            {
                if (const auto* display = displays.getDisplayForPoint (topLevel->getScreenBounds().getCentre()))
                    return display->scale > 1.0;
            }
        }

        // Default to 1x if we can't detect
        return false;
    }

    static juce::Rectangle<float> getFloatRect (juce::Component& component)
    {
        const auto& props = component.getProperties();
        if (props.contains ("floatX") && props.contains ("floatY")
            && props.contains ("floatW") && props.contains ("floatH"))
        {
            return juce::Rectangle<float> (
                static_cast<float> (static_cast<double> (props["floatX"])),
                static_cast<float> (static_cast<double> (props["floatY"])),
                static_cast<float> (static_cast<double> (props["floatW"])),
                static_cast<float> (static_cast<double> (props["floatH"])));
        }
        return component.getLocalBounds().toFloat();
    }

    juce::OwnedArray<juce::Image> images1x;
    juce::OwnedArray<juce::Image> images2x;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaledImageSet)
};

} // namespace BogrenDigital::UILoading
