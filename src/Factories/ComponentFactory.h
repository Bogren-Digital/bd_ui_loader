#pragma once

namespace BogrenDigital::UILoading {

struct ImageLoader;

/**
 * @brief Base interface for component factories.
 *
 * Factories create UI components from metadata. Each factory type
 * is responsible for creating a specific component type and loading
 * its required resources.
 *
 * The UILoader always calls the XmlElement overload. The default
 * implementation parses the standard ComponentMetadata fields and
 * dispatches to the metadata overload, which existing factories
 * override. Factories that need access to child elements or CDATA
 * can override the XmlElement overload directly.
 */
class ComponentFactory
{
public:
    ComponentFactory(ImageLoader& imageLoader)
        : imageLoader(imageLoader)
    {
    }

    virtual ~ComponentFactory() = default;

    /**
     * @brief Primary entry point — called by UILoader for every element.
     *
     * Default implementation parses flat metadata and forwards to the
     * ComponentMetadata overload. Override this to access child elements,
     * CDATA, or other structured XML content.
     */
    virtual juce::Component* createComponent(const juce::XmlElement* element);

    /**
     * @brief Legacy entry point for factories that only need flat metadata.
     *
     * Called by the default XmlElement overload. Returns a PlaceholderComponent
     * unless overridden.
     */
    virtual juce::Component* createComponent(const UILoader::ComponentMetadata& metadata);

protected:
    ImageLoader& imageLoader;
};

} // namespace BogrenDigital::UILoading