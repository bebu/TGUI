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


#include <TGUI/ToolTip.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Animation.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    void addAnimation(std::vector<std::shared_ptr<tgui::priv::Animation>>& existingAnimations, std::shared_ptr<tgui::priv::Animation> newAnimation)
    {
        auto type = newAnimation->getType();

        // If another animation is already running with the same type then instantly finish it
        unsigned int i = 0;
        while (i < existingAnimations.size())
        {
            if (existingAnimations[i]->getType() == type)
            {
                existingAnimations[i]->finish();
                existingAnimations.erase(existingAnimations.begin() + i);
            }
            else
                ++i;
        }

        existingAnimations.push_back(newAnimation);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        m_callback.widget = this;

        addSignal<sf::Vector2f>("PositionChanged");
        addSignal<sf::Vector2f>("SizeChanged");
        addSignal("Focused");
        addSignal("Unfocused");
        addSignal("MouseEntered");
        addSignal("MouseLeft");

        m_renderer->subscribe(this, [this](const std::string& property, ObjectConverter& value){ rendererChangedCallback(property, value); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        if (m_renderer)
            m_renderer->unsubscribe(this);

        if (m_position.x.getImpl()->parentWidget == this)
            m_position.x.getImpl()->parentWidget = nullptr;
        if (m_position.y.getImpl()->parentWidget == this)
            m_position.y.getImpl()->parentWidget = nullptr;
        if (m_size.x.getImpl()->parentWidget == this)
            m_size.x.getImpl()->parentWidget = nullptr;
        if (m_size.y.getImpl()->parentWidget == this)
            m_size.y.getImpl()->parentWidget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        sf::Drawable                   {other},
        Transformable                  {other},
        SignalWidgetBase               {other},
        enable_shared_from_this<Widget>{other},
        m_type                         (other.m_type), // Did not compile in VS2013 when using braces
        m_disabledBlockingMouseEvents  {other.m_disabledBlockingMouseEvents},
        m_enabled                      {other.m_enabled},
        m_visible                      {other.m_visible},
        m_parent                       {nullptr},
        m_allowFocus                   {other.m_allowFocus},
        m_draggableWidget              {other.m_draggableWidget},
        m_containerWidget              {other.m_containerWidget},
        m_toolTip                      {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_renderer                     {other.m_renderer}
    {
        m_callback.widget = this;

        m_position.x.getImpl()->parentWidget = this;
        m_position.x.getImpl()->recalculate();

        m_position.y.getImpl()->parentWidget = this;
        m_position.y.getImpl()->recalculate();

        m_size.x.getImpl()->parentWidget = this;
        m_size.x.getImpl()->recalculate();

        m_size.y.getImpl()->parentWidget = this;
        m_size.y.getImpl()->recalculate();

        if (m_renderer)
            m_renderer->subscribe(this, [this](const std::string& property, ObjectConverter& value){ rendererChangedCallback(property, value); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        sf::Drawable                   {std::move(other)},
        Transformable                  {std::move(other)},
        SignalWidgetBase               {std::move(other)},
        enable_shared_from_this<Widget>{std::move(other)},
        m_type                         (std::move(other.m_type)), // Did not compile in VS2013 when using braces
        m_disabledBlockingMouseEvents  {std::move(other.m_disabledBlockingMouseEvents)},
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {nullptr},
        m_mouseHover                   {std::move(other.m_mouseHover)},
        m_mouseDown                    {std::move(other.m_mouseDown)},
        m_focused                      {std::move(other.m_focused)},
        m_allowFocus                   {std::move(other.m_allowFocus)},
        m_animationTimeElapsed         {std::move(other.m_animationTimeElapsed)},
        m_draggableWidget              {std::move(other.m_draggableWidget)},
        m_containerWidget              {std::move(other.m_containerWidget)},
        m_toolTip                      {std::move(other.m_toolTip)},
        m_renderer                     {other.m_renderer},
        m_showAnimations               (std::move(other.m_showAnimations)) // Did not compile in VS2013 when using braces
    {
        m_callback.widget = this;

        m_position.x.getImpl()->parentWidget = this;
        m_position.x.getImpl()->recalculate();

        m_position.y.getImpl()->parentWidget = this;
        m_position.y.getImpl()->recalculate();

        m_size.x.getImpl()->parentWidget = this;
        m_size.x.getImpl()->recalculate();

        m_size.y.getImpl()->parentWidget = this;
        m_size.y.getImpl()->recalculate();

        if (m_renderer)
        {
            other.m_renderer->unsubscribe(&other);
            m_renderer->subscribe(this, [this](const std::string& property, ObjectConverter& value){ rendererChangedCallback(property, value); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            if (m_renderer)
                m_renderer->unsubscribe(this);

            sf::Drawable::operator=(other);
            Transformable::operator=(other);
            SignalWidgetBase::operator=(other);
            enable_shared_from_this::operator=(other);

            m_callback.widget      = this;
            m_type                 = other.m_type;
            m_disabledBlockingMouseEvents = other.m_disabledBlockingMouseEvents;
            m_enabled              = other.m_enabled;
            m_visible              = other.m_visible;
            m_parent               = nullptr;
            m_mouseHover           = false;
            m_mouseDown            = false;
            m_focused              = false;
            m_allowFocus           = other.m_allowFocus;
            m_animationTimeElapsed = {};
            m_draggableWidget      = other.m_draggableWidget;
            m_containerWidget      = other.m_containerWidget;
            m_toolTip              = other.m_toolTip ? other.m_toolTip->clone() : nullptr;
            m_renderer             = other.m_renderer;
            m_showAnimations       = {};

            m_position.x.getImpl()->parentWidget = this;
            m_position.x.getImpl()->recalculate();

            m_position.y.getImpl()->parentWidget = this;
            m_position.y.getImpl()->recalculate();

            m_size.x.getImpl()->parentWidget = this;
            m_size.x.getImpl()->recalculate();

            m_size.y.getImpl()->parentWidget = this;
            m_size.y.getImpl()->recalculate();

            if (m_renderer)
                m_renderer->subscribe(this, [this](const std::string& property, ObjectConverter& value){ rendererChangedCallback(property, value); });
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(Widget&& other)
    {
        if (this != &other)
        {
            if (m_renderer)
                m_renderer->unsubscribe(this);
            if (other.m_renderer)
                other.m_renderer->unsubscribe(&other);

            sf::Drawable::operator=(std::move(other));
            Transformable::operator=(std::move(other));
            SignalWidgetBase::operator=(std::move(other));
            enable_shared_from_this::operator=(std::move(other));

            m_callback.widget      = this;
            m_type                 = std::move(other.m_type);
            m_disabledBlockingMouseEvents = std::move(other.m_disabledBlockingMouseEvents);
            m_enabled              = std::move(other.m_enabled);
            m_visible              = std::move(other.m_visible);
            m_parent               = nullptr;
            m_mouseHover           = std::move(other.m_mouseHover);
            m_mouseDown            = std::move(other.m_mouseDown);
            m_focused              = std::move(other.m_focused);
            m_animationTimeElapsed = std::move(other.m_animationTimeElapsed);
            m_allowFocus           = std::move(other.m_allowFocus);
            m_draggableWidget      = std::move(other.m_draggableWidget);
            m_containerWidget      = std::move(other.m_containerWidget);
            m_toolTip              = std::move(other.m_toolTip);
            m_renderer             = std::move(other.m_renderer);
            m_showAnimations       = std::move(other.m_showAnimations);

            m_position.x.getImpl()->parentWidget = this;
            m_position.x.getImpl()->recalculate();

            m_position.y.getImpl()->parentWidget = this;
            m_position.y.getImpl()->recalculate();

            m_size.x.getImpl()->parentWidget = this;
            m_size.x.getImpl()->recalculate();

            m_size.y.getImpl()->parentWidget = this;
            m_size.y.getImpl()->recalculate();

            if (m_renderer)
                m_renderer->subscribe(this, [this](const std::string& property, ObjectConverter& value){ rendererChangedCallback(property, value); });
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRenderer(std::shared_ptr<RendererData> rendererData)
    {
        if (m_renderer)
            m_renderer->unsubscribe(this);

        m_renderer->m_data = rendererData;

        if (m_renderer)
            m_renderer->subscribe(this, [this](const std::string& property, ObjectConverter& value){ rendererChangedCallback(property, value); });

        for (auto& pair : rendererData->propertyValuePairs)
            rendererChanged(pair.first, pair.second);

        // Try to keep a font
        if (m_parent && !m_renderer->getFont() && m_parent->getRenderer()->getFont())
            m_renderer->setFont(m_parent->getRenderer()->getFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getRenderer() const
    {
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        if (position.x.getImpl()->parentWidget != this)
        {
            position.x.getImpl()->parentWidget = this;
            position.x.getImpl()->recalculate();
        }
        if (position.y.getImpl()->parentWidget != this)
        {
            position.y.getImpl()->parentWidget = this;
            position.y.getImpl()->recalculate();
        }

        Transformable::setPosition(position);

        m_callback.position = getPosition();
        sendSignal("PositionChanged", getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        if (size.x.getImpl()->parentWidget != this)
        {
            size.x.getImpl()->parentWidget = this;
            size.x.getImpl()->recalculate();
        }
        if (size.y.getImpl()->parentWidget != this)
        {
            size.y.getImpl()->parentWidget = this;
            size.y.getImpl()->recalculate();
        }

        Transformable::setSize(size);

        m_callback.size = getSize();
        sendSignal("SizeChanged", getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getAbsolutePosition() const
    {
        if (m_parent)
            return m_parent->getAbsolutePosition() + m_parent->getChildWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Widget::getWidgetOffset() const
    {
        return sf::Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::show()
    {
        m_visible = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::showWithEffect(ShowAnimationType type, sf::Time duration)
    {
        show();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), 0.f, getRenderer()->getOpacity(), duration));
                getRenderer()->setOpacity(0);
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), getPosition() + (getSize() / 2.f), getPosition(), duration));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), sf::Vector2f{0, 0}, getSize(), duration));
                setPosition(getPosition() + (getSize() / 2.f));
                setSize(0, 0);
                break;
            }
            case ShowAnimationType::SlideFromLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{-getFullSize().x, getPosition().y}, getPosition(), duration));
                setPosition({-getFullSize().x, getPosition().y});
                break;
            }
            case ShowAnimationType::SlideFromRight:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{getParent()->getSize().x + getWidgetOffset().x, getPosition().y}, getPosition(), duration));
                    setPosition({getParent()->getSize().x + getWidgetOffset().x, getPosition().y});
                }
                else
                    sf::err() << "TGUI Warning: showWithEffect(SlideFromRight) does not work before widget has a parent." << std::endl;

                break;
            }
            case ShowAnimationType::SlideFromTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{getPosition().x, -getFullSize().y}, getPosition(), duration));
                setPosition({getPosition().x, -getFullSize().y});
                break;
            }
            case ShowAnimationType::SlideFromBottom:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), sf::Vector2f{getPosition().x, getParent()->getSize().y + getWidgetOffset().y}, getPosition(), duration));
                    setPosition({getPosition().x, getParent()->getSize().y + getWidgetOffset().y});
                }
                else
                    sf::err() << "TGUI Warning: showWithEffect(SlideFromBottom) does not work before widget has a parent." << std::endl;

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hide()
    {
        m_visible = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hideWithEffect(ShowAnimationType type, sf::Time duration)
    {
        auto position = getPosition();
        auto size = getSize();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                float opacity = getRenderer()->getOpacity();
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), getRenderer()->getOpacity(), 0.f, duration, [=](){ hide(); getRenderer()->setOpacity(opacity); }));
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, position + (size / 2.f), duration, [=](){ hide(); setPosition(position); setSize(size); }));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), size, sf::Vector2f{0, 0}, duration, [=](){ hide(); setPosition(position); setSize(size); }));
                break;
            }
            case ShowAnimationType::SlideToRight:
            {
                if (getParent())
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{getParent()->getSize().x + getWidgetOffset().x, position.y}, duration, [=](){ hide(); setPosition(position); }));
                else
                    sf::err() << "TGUI Warning: hideWithEffect(SlideToRight) does not work before widget has a parent." << std::endl;

                break;
            }
            case ShowAnimationType::SlideToLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{-getFullSize().x, position.y}, duration, [=](){ hide(); setPosition(position); }));
                break;
            }
            case ShowAnimationType::SlideToBottom:
            {
                if (getParent())
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{position.x, getParent()->getSize().y + getWidgetOffset().y}, duration, [=](){ hide(); setPosition(position); }));
                else
                    sf::err() << "TGUI Warning: hideWithEffect(SlideToBottom) does not work before widget has a parent." << std::endl;

                break;
            }
            case ShowAnimationType::SlideToTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, sf::Vector2f{position.x, -getFullSize().y}, duration, [=](){ hide(); setPosition(position); }));
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::enable()
    {
        m_enabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::disable(bool blockMouseEvents)
    {
        m_enabled = false;
        m_disabledBlockingMouseEvents = blockMouseEvents;

        // Change the mouse button state.
        m_mouseHover = false;
        m_mouseDown = false;

        // If the widget is focused then it must be unfocused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::focus()
    {
        if (m_parent)
            m_parent->focusWidget(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unfocus()
    {
        if (m_focused && m_parent)
            m_parent->unfocusWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Widget::getWidgetType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToFront()
    {
        if (m_parent)
            m_parent->moveWidgetToFront(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToBack()
    {
        if (m_parent)
            m_parent->moveWidgetToBack(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setToolTip(Widget::Ptr toolTip)
    {
        m_toolTip = toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::getToolTip()
    {
        return m_toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        m_parent = parent;
        if (m_parent)
        {
            m_position.x.getImpl()->recalculate();
            m_position.y.getImpl()->recalculate();
            m_size.x.getImpl()->recalculate();
            m_size.y.getImpl()->recalculate();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update(sf::Time elapsedTime)
    {
        m_animationTimeElapsed += elapsedTime;

        for (unsigned int i = 0; i < m_showAnimations.size();)
        {
            if (m_showAnimations[i]->update(elapsedTime))
                m_showAnimations.erase(m_showAnimations.begin() + i);
            else
                i++;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMousePressed(sf::Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(sf::Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(sf::Vector2f)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const sf::Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(sf::Uint32)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseWheelMoved(int, int, int)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetFocused()
    {
        sendSignal("Focused");

        // Make sure the parent is also focused
        if (m_parent)
            m_parent->focus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::widgetUnfocused()
    {
        sendSignal("Unfocused");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::askToolTip(sf::Vector2f mousePos)
    {
        if (m_toolTip && mouseOnWidget(mousePos))
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const std::string& property, ObjectConverter&)
    {
        if ((property != "opacity") && (property != "font"))
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        sendSignal("MouseEntered");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        sendSignal("MouseLeft");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const std::string& property, ObjectConverter& value)
    {
        rendererChanged(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isDisabledBlockingMouseEvents() const
    {
        return m_disabledBlockingMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawRectangleShape(sf::RenderTarget& target,
                                    const sf::RenderStates& states,
                                    sf::Vector2f size,
                                    sf::Color color) const
    {
        sf::RectangleShape shape{size};

        if (getRenderer()->getOpacity() < 1)
            shape.setFillColor(calcColorOpacity(color, getRenderer()->getOpacity()));
        else
            shape.setFillColor(color);

        target.draw(shape, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawBorders(sf::RenderTarget& target,
                             const sf::RenderStates& states,
                             const Borders& borders,
                             sf::Vector2f size,
                             sf::Color color) const
    {
        sf::RectangleShape border;
        border.setFillColor(calcColorOpacity(color, getRenderer()->getOpacity()));

        // If size is too small then draw entire size as border
        if ((size.x <= borders.left + borders.right) || (size.y <= borders.top + borders.bottom))
        {
            border.setSize({size.x, size.y});
            target.draw(border, states);
        }
        else // Draw borders in the normal way
        {
            // Draw left border
            border.setSize({borders.left, size.y - borders.bottom});
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x - borders.left, borders.top});
            border.setPosition(borders.left, 0);
            target.draw(border, states);

            // Draw right border
            border.setSize({borders.right, size.y - borders.top});
            border.setPosition(size.x - borders.right, borders.top);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x - borders.right, borders.bottom});
            border.setPosition(0, size.y - borders.bottom);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
