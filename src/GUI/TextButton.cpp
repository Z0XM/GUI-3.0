#include "TextButton.hpp"

using namespace gui;

TextButton::TextButton(const sf::Vector2f& size):
	Textbox(size, GUI_ID_TEXTBUTTON)
{
	actionEvent = ActionEvent::RELEASE;
}

void TextButton::switchProperties()
{
	auto fillColor = m_box.getFillColor();
	auto outlineColor = m_box.getOutlineColor();
	auto outlineThickness = m_box.getOutlineThickness();
	auto scaleFactor = getScale();

	m_box.setFillColor(m_second_FillColor);
	m_box.setOutlineColor(m_second_OutlineColor);
	m_box.setOutlineThickness(m_second_OutlineThickness);
	setScale(m_second_ScaleFactor);

	m_second_FillColor = fillColor;
	m_second_OutlineColor = outlineColor;
	m_second_OutlineThickness = outlineThickness;
	m_second_ScaleFactor = scaleFactor;
}

void TextButton::setBoxFillColor(const sf::Color& color)
{
	if (m_isSelected) m_second_FillColor = color;
	else m_box.setFillColor(color);
}
void TextButton::setBoxOutlineColor(const sf::Color& color)
{
	if (m_isSelected) m_second_OutlineColor = color;
	else m_box.setOutlineColor(color);
}
void TextButton::setBoxOutlineThickness(float thickness)
{
	if (m_isSelected) m_second_OutlineThickness = thickness;
	else m_box.setOutlineThickness(thickness);
}
void TextButton::setSelectionFillColor(const sf::Color& color)
{
	if (m_isSelected) m_box.setFillColor(color);
	else m_second_FillColor = color;
}
void TextButton::setSelectionOutlineColor(const sf::Color& color)
{
	if (m_isSelected) m_box.setOutlineColor(color);
	else m_second_OutlineColor = color;
}
void TextButton::setSelectionOutlineThickness(float thickness)
{
	if (m_isSelected) m_box.setOutlineThickness(thickness);
	else m_second_OutlineThickness = thickness;
}
void TextButton::setSelectionScale(const sf::Vector2f& scaleFactor)
{
	if (m_isSelected) setScale(scaleFactor);
	else m_second_ScaleFactor = scaleFactor;
}
void TextButton::activateSelection()
{
	m_isSelected = true;
	switchProperties();
}
void TextButton::deactivateSelection()
{
	m_isSelected = false;
	switchProperties();
}



