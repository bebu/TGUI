/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_type = "MenuBar";
        m_callback.widgetType = "MenuBar";

        addSignal<std::vector<sf::String>, sf::String>("MenuItemClicked");

        m_renderer = aurora::makeCopied<MenuBarRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBackgroundColor(sf::Color::White);
        getRenderer()->setTextColor(sf::Color::Black);
        getRenderer()->setSelectedBackgroundColor({0, 110, 255});
        getRenderer()->setSelectedTextColor(sf::Color::White);
        getRenderer()->setDistanceToSide(4);

        setSize({0, 20});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::copy(MenuBar::ConstPtr menuBar)
    {
        if (menuBar)
            return std::static_pointer_cast<MenuBar>(menuBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->getTextureBackground().setSize(getSize());

        setTextSize(findBestTextSize(getRenderer()->getFont(), getSize().y * 0.8f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu newMenu;
        newMenu.text.setFont(getRenderer()->getFont());
        newMenu.text.setColor(getRenderer()->getTextColor());
        newMenu.text.setOpacity(getRenderer()->getOpacity());
        newMenu.text.setCharacterSize(m_textSize);
        newMenu.text.setString(text);
        m_menus.push_back(std::move(newMenu));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& text, sf::String menu)
    {
        if (menu.isEmpty())
        {
            if (m_menus.empty())
                return false;
            else
                menu = m_menus.back().text.getString();
        }

        sf::Vector2f pos = getPosition();

        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_menus[i].text.getString() == menu)
            {
                Text menuItem;
                menuItem.setFont(getRenderer()->getFont());
                menuItem.setColor(getRenderer()->getTextColor());
                menuItem.setOpacity(getRenderer()->getOpacity());
                menuItem.setCharacterSize(m_textSize);
                menuItem.setString(text);

                m_menus[i].menuItems.push_back(std::move(menuItem));
                return true;
            }

            pos.x += m_menus[i].text.getSize().x + 2 * getRenderer()->getDistanceToSide();
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenu(const sf::String& menu)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then remove it
            if (m_menus[i].text.getString() == menu)
            {
                m_menus.erase(m_menus.begin() + i);

                // The menu was removed, so it can't remain open
                if (m_visibleMenu == static_cast<int>(i))
                    m_visibleMenu = -1;

                return true;
            }
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const sf::String& menu, const sf::String& menuItem)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then search for the menu item
            if (m_menus[i].text.getString() == menu)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    // If this is the menu item then remove it
                    if (m_menus[i].menuItems[j].getString() == menuItem)
                    {
                        m_menus[i].menuItems.erase(m_menus[i].menuItems.begin() + j);

                        // The item can't still be selected
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            m_menus[i].selectedMenuItem = -1;

                        return true;
                    }
                }
            }
        }

        // could not find menu item
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::removeAllMenus()
    {
        m_menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_textSize = size;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                m_menus[i].menuItems[j].setCharacterSize(m_textSize);

            m_menus[i].text.setCharacterSize(m_textSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(float minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<sf::String, std::vector<sf::String>> MenuBar::getMenus() const
    {
        std::map<sf::String, std::vector<sf::String>> menus;

        for (auto& menu : m_menus)
        {
            std::vector<sf::String> items;
            for (auto& item : menu.menuItems)
                items.push_back(item.getString());

            menus[menu.text.getString()] = items;
        }

        return menus;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setParent(Container* parent)
    {
        Widget::setParent(parent);

        if ((m_parent != nullptr) && (getSize().x == 0))
            setSize(bindWidth(m_parent->shared_from_this()), m_size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(float x, float y) const
    {
        // Check if the mouse is on top of the menu bar
        if (sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(x, y))
            return true;
        else
        {
            // Check if there is a menu open
            if (m_visibleMenu != -1)
            {
                // Search the left position of the open menu
                float left = 0;
                for (int i = 0; i < m_visibleMenu; ++i)
                    left += m_menus[i].text.getSize().x + (2 * getRenderer()->getDistanceToSide());

                // Find out what the width of the menu should be
                float width = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[m_visibleMenu].menuItems.size(); ++j)
                {
                    if (width < m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * getRenderer()->getDistanceToSide()))
                        width = m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * getRenderer()->getDistanceToSide());
                }

                // Check if the mouse is on top of the open menu
                if (sf::FloatRect{left, getSize().y, width, getSize().y * m_menus[m_visibleMenu].menuItems.size()}.contains(x, y))
                    return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(float x, float y)
    {
        // Check if a menu should be opened or closed
        if (y < getSize().y)
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * getRenderer()->getDistanceToSide());
                if (x < menuWidth)
                {
                    // Close the menu when it was already open
                    if (m_visibleMenu == static_cast<int>(i))
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());
                            m_menus[m_visibleMenu].selectedMenuItem = -1;
                        }

                        m_menus[m_visibleMenu].text.setColor(getRenderer()->getTextColor());
                        m_visibleMenu = -1;
                    }

                    // If this menu can be opened then do so
                    else if (!m_menus[i].menuItems.empty())
                    {
                        if (getRenderer()->getSelectedTextColor().isSet())
                            m_menus[i].text.setColor(getRenderer()->getSelectedTextColor());
                        else
                            m_menus[i].text.setColor(getRenderer()->getTextColor());

                        m_visibleMenu = static_cast<int>(i);
                    }

                    break;
                }
            }
        }

        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(float, float y)
    {
        if (m_mouseDown)
        {
            // Check if the mouse is on top of one of the menus
            if (y >= getSize().y)
            {
                unsigned int selectedMenuItem = static_cast<unsigned int>((y - getSize().y) / getSize().y);

                if (selectedMenuItem < m_menus[m_visibleMenu].menuItems.size())
                {
                    m_callback.index = m_visibleMenu;
                    m_callback.text = m_menus[m_visibleMenu].menuItems[selectedMenuItem].getString();

                    sendSignal("MenuItemClicked",
                               std::vector<sf::String>{m_menus[m_visibleMenu].text.getString(), m_menus[m_visibleMenu].menuItems[selectedMenuItem].getString()},
                               m_menus[m_visibleMenu].menuItems[selectedMenuItem].getString());

                    closeVisibleMenu();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (y < getSize().y)
        {
            // Don't open a menu without having clicked first
            if (m_visibleMenu != -1)
            {
                // Loop through the menus to check if the mouse is on top of them
                float menuWidth = 0;
                for (unsigned int i = 0; i < m_menus.size(); ++i)
                {
                    menuWidth += m_menus[i].text.getSize().x + (2 * getRenderer()->getDistanceToSide());
                    if (x < menuWidth)
                    {
                        // Check if the menu is already open
                        if (m_visibleMenu == static_cast<int>(i))
                        {
                            // If one of the menu items is selected then unselect it
                            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                            {
                                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());
                                m_menus[m_visibleMenu].selectedMenuItem = -1;
                            }
                        }
                        else // The menu isn't open yet
                        {
                            // If there is another menu open then close it first
                            closeVisibleMenu();

                            // If this menu can be opened then do so
                            if (!m_menus[i].menuItems.empty())
                            {
                                if (getRenderer()->getSelectedTextColor().isSet())
                                    m_menus[i].text.setColor(getRenderer()->getSelectedTextColor());
                                else
                                    m_menus[i].text.setColor(getRenderer()->getTextColor());

                                m_visibleMenu = static_cast<int>(i);
                            }
                        }
                        break;
                    }
                }
            }
        }
        else // The mouse is on top of one of the menus
        {
            // Calculate on what menu item the mouse is located
            int selectedMenuItem = static_cast<int>((y - getSize().y) / getSize().y);

            // Check if the mouse is on a different item than before
            if (selectedMenuItem != m_menus[m_visibleMenu].selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());

                // Mark the item below the mouse as selected
                m_menus[m_visibleMenu].selectedMenuItem = selectedMenuItem;
                if (getRenderer()->getSelectedTextColor().isSet())
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getSelectedTextColor());
                else
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        if (!m_mouseDown)
            closeVisibleMenu();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseLeftWidget()
    {
        // Menu items which are selected on mouse hover should not remain selected now that the mouse has left
        if ((m_visibleMenu != -1) && (m_menus[m_visibleMenu].selectedMenuItem != -1))
        {
            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());
            m_menus[m_visibleMenu].selectedMenuItem = -1;
        }

        Widget::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeVisibleMenu()
    {
        // Check if there is still a menu open
        if (m_visibleMenu != -1)
        {
            // If an item in that menu was selected then unselect it first
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
            {
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_menus[m_visibleMenu].text.setColor(getRenderer()->getTextColor());
            m_visibleMenu = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if (property == "textcolor")
        {
            Color textColor = value.getColor();
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    if (m_menus[i].selectedMenuItem != static_cast<int>(j))
                        m_menus[i].menuItems[j].setColor(textColor);
                }

                m_menus[i].text.setColor(textColor);
            }

            if ((m_visibleMenu != -1) && (m_menus[m_visibleMenu].selectedMenuItem != -1) && getRenderer()->getSelectedTextColor().isSet())
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getSelectedTextColor());
        }
        else if (property == "selectedtextcolor")
        {
            if (m_visibleMenu != -1)
            {
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                {
                    if (value.getColor().isSet())
                        m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(value.getColor());
                    else
                        m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(getRenderer()->getTextColor());
                }
            }
        }
        else if ((property == "texturebackground") || (property == "textureitembackground") || (property == "textureselecteditembackground"))
        {
            value.getTexture().setSize(getSize());
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setOpacity(opacity);

                m_menus[i].text.setOpacity(opacity);
            }
        }
        else if (property == "font")
        {
            Font font = value.getFont();

            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setFont(font);

                m_menus[i].text.setFont(font);
            }

            setTextSize(findBestTextSize(font, getSize().y * 0.8f));
        }
        else if ((property != "backgroundcolor")
              && (property != "selectedbackgroundcolor")
              && (property != "distancetoside"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        sf::RenderStates textStates = states;

        // Draw the background
        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().draw(target, states);
        else
            drawRectangleShape(target, states, getSize(), getRenderer()->getBackgroundColor());

        // Draw the menu backgrounds
        Texture backgroundTexture = getRenderer()->getTextureItemBackground();
        float distanceToSide = getRenderer()->getDistanceToSide();
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // Is the menu open?
            if (m_visibleMenu == static_cast<int>(i))
            {
                sf::RenderStates oldStates = states;

                // Find out what the width of the menu should be
                float menuWidth = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    menuWidth = std::max(menuWidth, m_menus[i].menuItems[j].getSize().x + (2 * distanceToSide));

                Texture selectedBackgroundTexture = getRenderer()->getTextureSelectedItemBackground();
                if (selectedBackgroundTexture.isLoaded() && backgroundTexture.isLoaded())
                {
                    selectedBackgroundTexture.setSize({m_menus[i].text.getSize().x + (2 * distanceToSide), getSize().y});
                    selectedBackgroundTexture.draw(target, states);

                    backgroundTexture.setSize({menuWidth, getSize().y});
                    selectedBackgroundTexture.setSize({menuWidth, getSize().y});
                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        states.transform.translate({0, getSize().y});
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            selectedBackgroundTexture.draw(target, states);
                        else
                            backgroundTexture.draw(target, states);
                    }
                }
                else if (backgroundTexture.isLoaded())
                {
                    backgroundTexture.setSize({m_menus[i].text.getSize().x + (2 * distanceToSide), getSize().y});
                    backgroundTexture.draw(target, states);

                    backgroundTexture.setSize({menuWidth, getSize().y});
                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        states.transform.translate({0, getSize().y});
                        backgroundTexture.draw(target, states);
                    }
                }
                else // No textures where loaded
                {
                    Color backgroundColor = getRenderer()->getBackgroundColor();
                    Color selectedBackgroundColor = getRenderer()->getSelectedBackgroundColor();

                    if (selectedBackgroundColor.isSet())
                        drawRectangleShape(target, states, {m_menus[i].text.getSize().x + (2 * distanceToSide), getSize().y}, selectedBackgroundColor);
                    else
                        drawRectangleShape(target, states, {m_menus[i].text.getSize().x + (2 * distanceToSide), getSize().y}, backgroundColor);

                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        states.transform.translate({0, getSize().y});
                        if ((m_menus[i].selectedMenuItem == static_cast<int>(j)) && selectedBackgroundColor.isSet())
                            drawRectangleShape(target, states, {menuWidth, getSize().y}, selectedBackgroundColor);
                        else
                            drawRectangleShape(target, states, {menuWidth, getSize().y}, backgroundColor);
                    }
                }

                states = oldStates;
            }
            else // This menu is not open
            {
                if (backgroundTexture.isLoaded())
                {
                    backgroundTexture.setSize({m_menus[i].text.getSize().x + (2 * distanceToSide), getSize().y});
                    backgroundTexture.draw(target, states);
                }
            }

            states.transform.translate({m_menus[i].text.getSize().x + (2 * distanceToSide), 0});
        }

        // Draw the texts of the menus
        if (!m_menus.empty())
        {
            textStates.transform.translate({distanceToSide, (getSize().y - m_menus[0].text.getSize().y) / 2.f});
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                m_menus[i].text.draw(target, textStates);

                // Draw the menu items when the menu is open
                if (m_visibleMenu == static_cast<int>(i))
                {
                    sf::RenderStates oldStates = textStates;

                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        textStates.transform.translate({0, getSize().y});
                        m_menus[i].menuItems[j].draw(target, textStates);
                    }

                    textStates = oldStates;
                }

                textStates.transform.translate({m_menus[i].text.getSize().x + (2 * distanceToSide), 0});
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
