#pragma once

class SwitchComponent : public juce::ToggleButton, 
                        public PlayfulTones::ComponentResizer, 
                        public OriginalSizeReporter
{
private:
    // Custom LookAndFeel that draws toggle buttons using the provided images
    class SwitchLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        SwitchLookAndFeel() = default;
        
        void setImages(juce::OwnedArray<juce::Image>* imgs) { images = imgs; }
        void setMaskImage(juce::Image* mask) { maskImage = mask; }
        void setResamplingEnabled(bool enabled) { resamplingEnabled = enabled; }
        
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            // Ignore highlighted/down states as per requirements
            juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            
            // Check the toggle state and draw the appropriate image
            if (images != nullptr)
            {
                int imageIndex = button.getToggleState() ? 1 : 0;
                if (imageIndex < images->size() && (*images)[imageIndex] != nullptr && (*images)[imageIndex]->isValid())
                {
                    if (BogrenDigital::ImageResampler::shouldUseResampling(button.getLocalBounds(), resamplingEnabled))
                    {
                        // Resample the image if needed
                        auto resampledImage = BogrenDigital::ImageResampler::applyResize(
                            *(*images)[imageIndex], *maskImage, button.getWidth(), button.getHeight());
                        g.drawImageAt(resampledImage, 0, 0);
                    }
                    else
                    {
                        // Draw the image directly without resampling
                        g.drawImage(*(*images)[imageIndex], button.getLocalBounds().toFloat());
                    }
                    return;
                }
            }
            // Fallback if no valid image or images array is empty
            g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
            g.setColour(juce::Colours::white);
            g.drawText("Switch: " + button.getName(), button.getLocalBounds(), 
                      juce::Justification::centred, true);
        }
        
    private:
        juce::OwnedArray<juce::Image>* images = nullptr;
        juce::Image* maskImage = nullptr;
        bool resamplingEnabled = false;
    };
    
public:
    SwitchComponent(const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, 
                   juce::Image mask)
    : juce::ToggleButton(name)
    , PlayfulTones::ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    , maskImage(std::move(mask))
    {
        images.swapWith(imagesToUse); // Transfer ownership of images
        
        // Set our custom look and feel
        switchLookAndFeel.setImages(&images);
        switchLookAndFeel.setMaskImage(&maskImage);
        switchLookAndFeel.setResamplingEnabled(metadata.useGuiResampler);
        setLookAndFeel(&switchLookAndFeel);
        
        // Default state is off (first image)
        setToggleState(false, juce::dontSendNotification);
        
        setOpaque(false);
    }
    
    ~SwitchComponent() override
    {
        // Reset the LookAndFeel to avoid dangling pointers
        setLookAndFeel(nullptr);
    }
    
private:
    juce::OwnedArray<juce::Image> images;
    SwitchLookAndFeel switchLookAndFeel;
    juce::Image maskImage;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchComponent)
};
