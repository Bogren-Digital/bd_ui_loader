#pragma once
#include <unordered_map>

namespace BogrenDigital::UILoading
{
    class AspectRatioListener;
    class ComponentFactory;
    class ComponentFactoryRegistry;

    struct BinaryAssetImageLoader;

    /**
     * @brief Main UI loading and layout management system.
     *
     * Loads UI components from XML metadata and manages their layout,
     * resizing, and factory registration. Maintains aspect ratio based
     * on bitmap dimensions.
     */
    class UILoader
    {
    public:
        UILoader(juce::Component& parentComponent, BogrenDigital::UILoading::BinaryAssetImageLoader& imageLoader);
        ~UILoader();

        /**
         * @brief Loads UI from XML metadata file and applies layout.
         * @param xmlFileName Name of the XML file in binary resources
         */
        void loadUI(const juce::String& xmlFileName);

        /** @brief Registers all available component factories with the registry. */
        void registerComponentFactories();

        /** @brief Applies layout to all loaded components. */
        void applyLayout();

        /** @brief Applies layout to a specific component based on its metadata properties. */
        void applyLayoutToComponent(juce::Component* component);

        /** @brief Maintains the aspect ratio of the parent component based on bitmap dimensions. */
        void applyProportionalResize();

        // X-Macro for ComponentMetadata fields - single source of truth
        // Format: FIELD_TYPE(fieldName, defaultValue)
        #define COMPONENT_METADATA_FIELDS \
            STRING_FIELD(type, "") \
            STRING_FIELD(name, "") \
            STRING_FIELD(file, "") \
            STRING_FIELD(fileNamePrefix, "") \
            STRING_FIELD(fileNameSuffix, "") \
            STRING_FIELD(imageType, "") \
            INT_FIELD(x, 0) \
            INT_FIELD(y, 0) \
            INT_FIELD(width, 0) \
            INT_FIELD(height, 0) \
            INT_FIELD(numberOfFrames, 0) \
            INT_FIELD(minX, 0) \
            INT_FIELD(minY, 0) \
            INT_FIELD(maxX, 0) \
            INT_FIELD(maxY, 0)

        /**
         * @brief Metadata describing a UI component's properties.
         *
         * Parsed from XML attributes and used by component factories
         * to create and configure components. Supports builder pattern
         * for fluent configuration.
         */
        struct ComponentMetadata
        {
            // Generate field declarations using the macro
            #define STRING_FIELD(fieldName, defaultValue) juce::String fieldName = defaultValue;
            #define INT_FIELD(fieldName, defaultValue) int fieldName = defaultValue;
            #define BOOL_FIELD(fieldName, defaultValue) bool fieldName = defaultValue;
            COMPONENT_METADATA_FIELDS
            #undef STRING_FIELD
            #undef INT_FIELD
            #undef BOOL_FIELD

            ComponentMetadata() = default;
            ComponentMetadata(const ComponentMetadata& other) = default;
            ComponentMetadata& operator=(const ComponentMetadata& other) = default;
            ComponentMetadata& withType(const juce::String& t) { type = t; return *this; }
            ComponentMetadata& withName(const juce::String& n) { name = n; return *this; }
            ComponentMetadata& withFile(const juce::String& f) { file = f; return *this; }
            ComponentMetadata& withX(int value) { x = value; return *this; }
            ComponentMetadata& withY(int value) { y = value; return *this; }
            ComponentMetadata& withWidth(int value) { width = value; return *this; }
            ComponentMetadata& withHeight(int value) { height = value; return *this; }
            ComponentMetadata& withImageType(const juce::String& ct) { imageType = ct; return *this; }
            ComponentMetadata& withFileNamePrefix(const juce::String& fnp) { fileNamePrefix = fnp; return *this; }
            ComponentMetadata& withFileNameSuffix(const juce::String& fns) { fileNameSuffix = fns; return *this; }
            ComponentMetadata& withNumberOfFrames(int nof) { numberOfFrames = nof; return *this; }
            ComponentMetadata& withMinX(int value) { minX = value; return *this; }
            ComponentMetadata& withMinY(int value) { minY = value; return *this; }
            ComponentMetadata& withMaxX(int value) { maxX = value; return *this; }
            ComponentMetadata& withMaxY(int value) { maxY = value; return *this; }
        };

        /** @brief Copies all metadata fields to a component's properties for later retrieval. */
        static void applyMetadataToProperties(juce::Component* component, const ComponentMetadata& metadata)
        {
            if (component == nullptr)
                return;

            auto& props = component->getProperties();

            #define STRING_FIELD(fieldName, defaultValue) props.set(#fieldName, metadata.fieldName);
            #define INT_FIELD(fieldName, defaultValue) props.set(#fieldName, metadata.fieldName);
            #define BOOL_FIELD(fieldName, defaultValue) props.set(#fieldName, metadata.fieldName);
            COMPONENT_METADATA_FIELDS
            #undef STRING_FIELD
            #undef INT_FIELD
            #undef BOOL_FIELD
        }

        /**
         * @brief Layout information derived from bitmap metadata.
         *
         * Tracks the original bitmap dimensions and coordinate spaces
         * for layout transformations.
         */
        struct BitmapLayout
        {
            int width = 0;
            int height = 0;
            juce::Rectangle<float> sourceBounds { 0.0f, 0.0f, 0.0f, 0.0f };
            juce::Rectangle<float> targetBounds { 0.0f, 0.0f, 0.0f, 0.0f };

            void setDimensions(int w, int h)
            {
                width = w;
                height = h;
                sourceBounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
            }

            double getAspectRatio() const
            {
                return height > 0 ? static_cast<double>(width) / static_cast<double>(height) : 1.0;
            }
        };

        BogrenDigital::UILoading::BinaryAssetImageLoader& getImageLoader() { return imageLoader; }

        ComponentFactoryRegistry& getComponentFactoryRegistry() { return *componentFactoryRegistry; }

        const std::unordered_map<juce::String, juce::Component*>& getComponentsByName() const { return componentsByName; }

        juce::Component* getComponentByName(const juce::String& name) const
        {
            auto it = componentsByName.find(name);
            return it != componentsByName.end() ? it->second : nullptr;
        }

        int getMetadataWidth() const { return bitmapLayout.width; }
        int getMetadataHeight() const { return bitmapLayout.height; }

    private:
        void parseXML(const juce::String& xmlContent);
        ComponentMetadata parseElement(juce::XmlElement* element);

        /** @brief Calculates transformed bounds for a component based on coordinate space mapping. */
        static juce::Rectangle<float> calculateTransformedBounds(
            const juce::Rectangle<float>& sourceBounds,
            const juce::Rectangle<float>& targetBounds,
            const juce::Rectangle<float>& componentSourceBounds);

        juce::Component& parentComponent;
        BogrenDigital::UILoading::BinaryAssetImageLoader& imageLoader;
        juce::OwnedArray<juce::Component> components;
        std::unordered_map<juce::String, juce::Component*> componentsByName;

        std::unique_ptr<AspectRatioListener> aspectRatioListener;

        BitmapLayout bitmapLayout;

        std::unique_ptr<ComponentFactoryRegistry> componentFactoryRegistry;
    };
}
