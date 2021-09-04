#include "Button.hpp"

using namespace gui;
Button::Button(const sf::Vector2f& size)
	:Entity(GUI_ID_BUTTON)
{
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
void Button::updateShape()
{
	if (m_isSelected) {
		m_shape.setFillColor(m_selected.fillColor);
		m_shape.setOutlineColor(m_selected.outlineColor);
		m_shape.setOutlineThickness(m_selected.outlineThickness);
		m_shape.setScale(m_selected.scaleFactor);
	}
	else {
		m_shape.setFillColor(m_original.fillColor);
		m_shape.setOutlineColor(m_original.outlineColor);
		m_shape.setOutlineThickness(m_original.outlineThickness);
		m_shape.setScale(m_original.scaleFactor);
	}
}
void Button::setFillColor(const sf::Color& color)
{
	m_original.fillColor = color;
	updateShape();
}
void Button::setOutlineColor(const sf::Color& color)
{
	m_original.outlineColor = color;
	updateShape();
}
void Button::setOutlineThickness(float thickness)
{
	m_original.outlineThickness = thickness;
	updateShape();
}
void gui::Button::setScale(const sf::Vector2f& scaleFactor)
{
	m_original.scaleFactor = scaleFactor;
	updateShape();
}
void Button::setSelectionFillColor(const sf::Color& color)
{
	m_selected.fillColor = color;
	updateShape();
}
void Button::setSelectionOutlineColor(const sf::Color& color)
{
	m_selected.outlineColor = color;
	updateShape();
}
void Button::setSelectionOutlineThickness(float thickness)
{
	m_selected.outlineThickness = thickness;
	updateShape();
}
void gui::Button::setSelectionScale(const sf::Vector2f& scaleFactor)
{
	m_selected.scaleFactor = scaleFactor;
	updateShape();
}
sf::FloatRect gui::Button::getLocalBounds() const
{
	return m_shape.getLocalBounds();
}
sf::FloatRect gui::Button::getGlobalBounds() const
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
	updateShape();
}
void Button::deactivateSelection()
{
	m_isSelected = false;
	updateShape();
}
void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isActive()) {
		states.transform *= getTransform();
		target.draw(m_shape, states);
	}
}