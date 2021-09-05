#include "Button.hpp"

using namespace gui;
Button::Button(const sf::Vector2f& size)
	:Entity(GUI_ID_BUTTON)
{
	actionEvent = ActionEvent::RELEASE;
}
void Button::setPointCount(std::size_t count)
{
	m_shape.setPointCount(count);
}
std::size_t Button::getPointCount() const
{
	return m_shape.getPointCount();
}
void Button::setPoint(std::size_t index, const sf::Vector2f& point)
{
	m_shape.setPoint(index, point);
}
sf::Vector2f Button::getPoint(std::size_t index) const
{
	return m_shape.getPoint(index);
}
void Button::switchProperties()
{
	auto fillColor = m_shape.getFillColor();
	auto outlineColor = m_shape.getOutlineColor();
	auto outlineThickness = m_shape.getOutlineThickness();
	auto scaleFactor = getScale();

	m_shape.setFillColor(m_second_FillColor);
	m_shape.setOutlineColor(m_second_OutlineColor);
	m_shape.setOutlineThickness(m_second_OutlineThickness);
	setScale(m_second_ScaleFactor);

	m_second_FillColor = fillColor;
	m_second_OutlineColor = outlineColor;
	m_second_OutlineThickness = outlineThickness;
	m_second_ScaleFactor = scaleFactor;
}

void Button::setFillColor(const sf::Color& color)
{
	if (m_isSelected) m_second_FillColor = color;
	else m_shape.setFillColor(color);
}
void Button::setOutlineColor(const sf::Color& color)
{
	if (m_isSelected) m_second_OutlineColor = color;
	else m_shape.setOutlineColor(color);
}
void Button::setOutlineThickness(float thickness)
{
	if (m_isSelected) m_second_OutlineThickness = thickness;
	else m_shape.setOutlineThickness(thickness);
}
void Button::setSelectionFillColor(const sf::Color& color)
{
	if (m_isSelected) m_shape.setFillColor(color);
	else m_second_FillColor = color;
}
void Button::setSelectionOutlineColor(const sf::Color& color)
{
	if (m_isSelected) m_shape.setOutlineColor(color);
	else m_second_OutlineColor = color;
}
void Button::setSelectionOutlineThickness(float thickness)
{
	if (m_isSelected) m_shape.setOutlineThickness(thickness);
	else m_second_OutlineThickness = thickness;
}
void Button::setSelectionScale(const sf::Vector2f& scaleFactor)
{
	if (m_isSelected) setScale(scaleFactor);
	else m_second_ScaleFactor = scaleFactor;
}
void Button::setTexture(const sf::Texture* texture, bool resetRect)
{
	m_shape.setTexture(texture, resetRect);
}
void Button::setTextureRect(const sf::IntRect& rect)
{
	m_shape.setTextureRect(rect);
}
const sf::Texture* Button::getTexture() const
{
	return m_shape.getTexture();
}
const sf::IntRect& Button::getTextureRect() const
{
	return m_shape.getTextureRect();
}
sf::FloatRect Button::getLocalBounds() const
{
	return m_shape.getLocalBounds();
}
sf::FloatRect Button::getGlobalBounds() const
{
	return getTransform().transformRect(m_shape.getGlobalBounds());
}
bool Button::contains(const sf::Vector2f& point) const
{
	return getLocalBounds().contains(getInverseTransform().transformPoint(point));
}
Entity* Button::isHit(const sf::Vector2f& point)
{
	if (isActive()) {
		return contains(point) ? this : nullptr;
	}
}
void Button::activateSelection()
{
	m_isSelected = true;
	switchProperties();
}
void Button::deactivateSelection()
{
	m_isSelected = false;
	switchProperties();
}
void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isActive()) {
		states.transform *= getTransform();
		target.draw(m_shape, states);
	}
}