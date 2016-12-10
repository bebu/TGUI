/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_PROGRESS_BAR_HPP
#define TGUI_PROGRESS_BAR_HPP


#include <TGUI/Widgets/Label.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class ProgressBarRenderer;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Progress bar widget
    ///
    /// Signals:
    ///     - ValueChanged (The value of the progress bar has changed)
    ///         * Optional parameter int: New value
    ///         * Uses Callback member 'value'
    ///
    ///     - Full (The new value equals the maximum value)
    ///         * Optional parameter int: The maximum value
    ///         * Uses Callback member 'value'
    ///
    ///     - Inherited signals from ClickableWidget
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API ProgressBar : public ClickableWidget
    {
    public:

        typedef std::shared_ptr<ProgressBar> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const ProgressBar> ConstPtr; ///< Shared constant widget pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The fill direction of the progress bar
        ///
        /// Determines in which direction the progress bar is filled when value goes from minimum to maximum.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class FillDirection
        {
            LeftToRight,  ///< Start filling at the left side and go to the right
            RightToLeft,  ///< Start filling at the right side and go to the left
            TopToBottom,  ///< Start filling at the top an go downward
            BottomToTop   ///< Start filling at the bottom and go upward
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ProgressBar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new progress bar widget
        ///
        /// @return The new progress bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static ProgressBar::Ptr create();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another progress bar
        ///
        /// @param progressBar  The other progress bar
        ///
        /// @return The new progress bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static ProgressBar::Ptr copy(ProgressBar::ConstPtr progressBar);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer, which gives access to functions that determine how the widget is displayed
        ///
        /// @return Reference to the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<ProgressBarRenderer> getRenderer() const
        {
            return std::static_pointer_cast<ProgressBarRenderer>(m_renderer);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Set the position of the widget
        ///
        /// This function completely overwrites the previous position.
        /// See the move function to apply an offset based on the previous position instead.
        /// The default position of a transformable widget is (0, 0).
        ///
        /// @param position  New position
        ///
        /// @see move, getPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setPosition(const Layout2d& position) override;
        using Transformable::setPosition;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the progress bar.
        ///
        /// @param size  The new size of the progress bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Transformable::setSize;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the full size of the progress bar
        ///
        /// The size returned by this function includes the borders.
        ///
        /// @return Full size of the progress bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual sf::Vector2f getFullSize() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the font of the text in the widget.
        ///
        /// @param font  The new font.
        ///
        /// When you don't call this function then the font from the parent widget will be used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setFont(const Font& font) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a minimum value.
        ///
        /// @param minimum  The new minimum value
        ///
        /// When the value is too small then it will be changed to this minimum.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMinimum(unsigned int minimum);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a maximum value.
        ///
        /// @param maximum  The new maximum value
        ///
        /// When the value is too big then it will be changed to this maximum.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaximum(unsigned int maximum);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the current value.
        ///
        /// @param value  The new value
        ///
        /// The value can't be smaller than the minimum or bigger than the maximum.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setValue(unsigned int value);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the minimum value.
        ///
        /// @return The current minimum value
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getMinimum() const
        {
            return m_minimum;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the maximum value.
        ///
        /// @return The current maximum value
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getMaximum() const
        {
            return m_maximum;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the current value.
        ///
        /// @return The current value
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getValue() const
        {
            return m_value;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Increment the value.
        ///
        /// @return the new value
        ///
        /// The value can never exceed the maximum.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int incrementValue();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the caption of the progress bar.
        ///
        /// @param text  New text to draw on top of the progress bar
        ///
        /// This text will be centered in the progress bar. It could e.g. contain the progress.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setText(const sf::String& text);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the caption of the progress bar.
        ///
        /// @return Text that is drawn on top of the progress bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::String getText() const
        {
            return m_textBack.getText();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the character size of the text.
        ///
        /// @param size  The new text size.
        ///              If the size is set to 0 then the text will be auto-sized to fit inside the progress bar.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextSize(unsigned int size);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the character size of the text.
        ///
        /// @return The current text size.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int getTextSize() const
        {
            return m_textBack.getTextSize();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the fill direction of the progress bar
        ///
        /// @param direction  In which direction is the progress bar filled when the value goes from minimum to maximum?
        ///
        /// By default the progress bar is filled from left to right.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setFillDirection(FillDirection direction);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the fill direction of the progress bar
        ///
        /// @return In which direction is the progress bar filled when the value goes from minimum to maximum?
        ///
        /// By default the progress bar is filled from left to right.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FillDirection getFillDirection()
        {
            return m_fillDirection;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the opacity of the widget.
        ///
        /// @param opacity  The opacity of the widget. 0 means completely transparent, while 1 (default) means fully opaque.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setOpacity(float opacity) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the distance between the position where the widget is drawn and where the widget is placed
        ///
        /// This is basically the width and height of the optional borders drawn around widgets.
        ///
        /// @return Offset of the widget
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual sf::Vector2f getWidgetOffset() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Reload the widget
        ///
        /// @param primary    Primary parameter for the loader
        /// @param secondary  Secondary parameter for the loader
        /// @param force      Try to only change the looks of the widget and not alter the widget itself when false
        ///
        /// @throw Exception when the connected theme could not create the widget
        ///
        /// When primary is an empty string the built-in white theme will be used.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void reload(const std::string& primary = "", const std::string& secondary = "", bool force = false) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<ProgressBar>(*this);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // When the value changes, or when the minimum/maximum limits change then a smaller of bigger piece of the front image
        // must be drawn. This function is called to calculate the size of the piece to draw.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void recalculateSize();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        unsigned int m_minimum = 0;
        unsigned int m_maximum = 100;
        unsigned int m_value = 0;

        Label m_textBack;
        Label m_textFront;
        unsigned int m_textSize = 0;

        sf::FloatRect m_frontRect;

        FillDirection m_fillDirection = FillDirection::LeftToRight;

        friend class ProgressBarRenderer;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API ProgressBarRenderer : public WidgetRenderer, public WidgetBorders
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param progressBar  The progress bar that is connected to the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ProgressBarRenderer(ProgressBar* progressBar) : m_progressBar{progressBar} {}


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change a property of the renderer
        ///
        /// @param property  The property that you would like to change
        /// @param value     The new serialized value that you like to assign to the property
        ///
        /// @throw Exception when deserialization fails or when the widget does not have this property.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setProperty(std::string property, const std::string& value) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change a property of the renderer
        ///
        /// @param property  The property that you would like to change
        /// @param value     The new value that you like to assign to the property.
        ///                  The ObjectConverter is implicitly constructed from the possible value types.
        ///
        /// @throw Exception for unknown properties or when value was of a wrong type.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setProperty(std::string property, ObjectConverter&& value) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieve the value of a certain property
        ///
        /// @param property  The property that you would like to retrieve
        ///
        /// @return The value inside a ObjectConverter object which you can extract with the correct get function or
        ///         an ObjectConverter object with type ObjectConverter::Type::None when the property did not exist.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ObjectConverter getProperty(std::string property) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get a map with all properties and their values
        ///
        /// @return Property-value pairs of the renderer
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::map<std::string, ObjectConverter> getPropertyValuePairs() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the font of the text.
        ///
        /// When you don't call this function then the global font will be use.
        /// This global font can be changed with the setGlobalFont function from the parent.
        ///
        /// @param font  The new font
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextFont(std::shared_ptr<sf::Font> font);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the text that is optionally displayed on top of the progress bar.
        ///
        /// @param color  The new text color
        ///
        /// This changes both the back and front text colors.
        ///
        /// @see setTextColorBack
        /// @see setTextColorFront
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextColor(const Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the text that is optionally displayed on top of the progress bar.
        ///
        /// @param color  The new text color that is displayed on top of the background color/image.
        ///
        /// This color is displayed on top of the unfilled part. The front text color will be used on top of the filled part.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextColorBack(const Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the text that is optionally displayed on top of the progress bar.
        ///
        /// @param color  The new text color that is displayed on top of the foreground color/image.
        ///
        /// This color is displayed on top of the filled part. The back text color will be used on top of the unfilled part.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextColorFront(const Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the background color of the progress bar.
        ///
        /// @param color  The new background color
        ///
        /// This is the color that you see in the part of the progress bar that is not filled.
        ///
        /// Note that this color is ignored when you set an image as background.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBackgroundColor(const Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the foreground color of the progress bar.
        ///
        /// @param color  The new foreground color
        ///
        /// This is the color that is used to fill the progress bar and is drawn on top of the background color.
        ///
        /// Note that this color is ignored when you set an image as foreground.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setForegroundColor(const Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the border color.
        ///
        /// @param color  The color that is used for the borders that are optionally drawn around the progress bar
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBorderColor(const Color& color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the background image of the progress bar
        ///
        /// @param texture  The new background texture
        ///
        /// When this image and the front image are set, the background and foreground color properties will be ignored.
        /// Pass an empty string to unset the image, in this case the color properties will be used again.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBackTexture(const Texture& texture);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the foreground image of the progress bar
        ///
        /// @param texture  The new foreground texture
        ///
        /// When this image and the back image are set, the background and foreground color properties will be ignored.
        /// Pass an empty string to unset the image, in this case the color properties will be used again.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setFrontTexture(const Texture& texture);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Draws the widget on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::shared_ptr<WidgetRenderer> clone(Widget* widget) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        ProgressBar* m_progressBar;

        Texture  m_textureBack;
        Texture  m_textureFront;

        sf::Color m_textColorFront;
        sf::Color m_textColorBack;

        sf::Color m_backgroundColor;
        sf::Color m_foregroundColor;

        sf::Color m_borderColor;

        friend class ProgressBar;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_PROGRESS_BAR_HPP
