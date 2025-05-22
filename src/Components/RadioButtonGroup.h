#pragma once

class RadioButtonGroup : public juce::Component, 
                         public PlayfulTones::ComponentResizer, 
                         public OriginalSizeReporter
{
private:
    // Custom LookAndFeel that makes toggle buttons invisible
    class InvisibleToggleLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        InvisibleToggleLookAndFeel() = default;
        
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            // Do nothing - this makes the toggle button completely invisible
            juce::ignoreUnused(g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        }
    };
    
public:
    RadioButtonGroup(const juce::String& name, juce::OwnedArray<juce::Image>& imagesToUse, UILoader::ComponentMetadata metadata, juce::Image maskImage)
    : juce::Component(name)
    , PlayfulTones::ComponentResizer(*dynamic_cast<juce::Component*>(this))
    , OriginalSizeReporter(std::move(metadata))
    , resamplingMask(std::move(maskImage))
    , useGuiResampler(metadata.useGuiResampler)
    {
        images.swapWith(imagesToUse); // Transfer ownership of images
        
        // Create the appropriate number of buttons based on the metadata
        for (int i = 0; i < metadata.numberOfFrames; ++i)
        {
            auto* button = new juce::ToggleButton(name + "_" + juce::String(i));
            button->setRadioGroupId(1); // All buttons in the same radio group
            button->onClick = [this, i] { setSelectedButtonIndex(i); };
            button->setLookAndFeel(&invisibleLookAndFeel); // Apply the invisible look and feel
            addAndMakeVisible(button);
            buttons.add(button);
        }
        
        // If we have buttons, select the first one by default
        if (buttons.size() > 0)
        {
            buttons[0]->setToggleState(true, juce::sendNotification);
            selectedButtonIndex = 0;
        }
        
        setOpaque(false);
    }
    
    ~RadioButtonGroup() override
    {
        // Make sure to reset the LookAndFeel for each button to avoid dangling pointers
        for (auto* button : buttons)
        {
            button->setLookAndFeel(nullptr);
        }
    }
    
    void resized() override
    {
        // Divide the space horizontally for each button
        int numButtons = buttons.size();
        if (numButtons > 0)
        {
            int buttonWidth = getWidth() / numButtons;
            for (int i = 0; i < numButtons; ++i)
            {
                buttons[i]->setBounds(i * buttonWidth, 0, buttonWidth, getHeight());
            }
        }
    }
    
    void paint(juce::Graphics& g) override
    {
        // Draw the image corresponding to the currently selected button
        if (selectedButtonIndex >= 0 && selectedButtonIndex < images.size() && 
            images[selectedButtonIndex] != nullptr && images[selectedButtonIndex]->isValid())
        {
            if(BogrenDigital::ImageResampler::shouldUseResampling(getLocalBounds(), useGuiResampler))
            {
                const auto resampledImage = BogrenDigital::ImageResampler::applyResize(
                    *images[selectedButtonIndex], resamplingMask, getWidth(), getHeight());
                g.drawImageAt(resampledImage, 0, 0);
            }
            else
            {
                g.drawImage(*images[selectedButtonIndex], getLocalBounds().toFloat());
            }
        }
        else
        {
            // Fallback if no valid image
            g.fillAll(juce::Colours::darkgrey.withAlpha(0.3f));
            g.setColour(juce::Colours::white);
            g.drawText("RadioButtonGroup: " + getName(), getLocalBounds(), 
                      juce::Justification::centred, true);
        }
    }
    
    // Getter for a specific button
    juce::ToggleButton* getButton(int index)
    {
        return (index >= 0 && index < buttons.size()) ? buttons[index] : nullptr;
    }
    
    // Get the currently selected button index
    int getSelectedButtonIndex() const
    {
        return selectedButtonIndex;
    }
    
    // Set the selected button by index
    void setSelectedButtonIndex(int index)
    {
        if (index >= 0 && index < buttons.size())
        {
            selectedButtonIndex = index;
            
            // Update button states (ensure only one is toggled on)
            for (int i = 0; i < buttons.size(); ++i)
            {
                buttons[i]->setToggleState(i == index, juce::dontSendNotification);
            }
            
            repaint(); // Redraw with the new selected image
        }
    }
    
private:
    juce::Image resamplingMask;
    InvisibleToggleLookAndFeel invisibleLookAndFeel;
    juce::OwnedArray<juce::Image> images;
    juce::OwnedArray<juce::ToggleButton> buttons;
    int selectedButtonIndex = -1;
    const bool useGuiResampler = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RadioButtonGroup)
};
