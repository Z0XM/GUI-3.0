#pragma once
#include "GUIFrame.hpp"

namespace gui
{
class Button : public Entity, public sf::Transformable {
public:

	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// \param size -> size of the box
	/// 
	////////////////////////////////////////////////////////////
	Button(const sf::Vector2f& size = sf::Vector2f(0, 0));

	////////////////////////////////////////////////////////////
	/// \brief Set the number of points of the polygon
	///		   count must be greater than 2 to define a valid shape.
	///
	/// \param count New number of points of the polygon
	///
	////////////////////////////////////////////////////////////
	void setPointCount(std::size_t count);

	////////////////////////////////////////////////////////////
	/// \brief Get the number of points of the polygon
	///
	/// \return Number of points of the polygon
	///
	////////////////////////////////////////////////////////////
	virtual std::size_t getPointCount() const;

	////////////////////////////////////////////////////////////
	/// \brief Set the position of a point
	///
	/// Don't forget that the polygon must remain convex, and
	/// the points need to stay ordered!
	/// setPointCount must be called first in order to set the total
	/// number of points. The result is undefined if \a index is out
	/// of the valid range.
	///
	/// \param index Index of the point to change, in range [0 .. getPointCount() - 1]
	/// \param point New position of the point
	///
	////////////////////////////////////////////////////////////
	void setPoint(std::size_t index, const sf::Vector2f& point);

	////////////////////////////////////////////////////////////
	/// \brief Get the position of a point
	///
	/// The returned point is in local coordinates, that is,
	/// the shape's transforms (position, rotation, scale) are
	/// not taken into account.
	/// The result is undefined if \a index is out of the valid range.
	///
	/// \param index Index of the point to get, in range [0 .. getPointCount() - 1]
	///
	/// \return Position of the index-th point of the polygon
	///
	////////////////////////////////////////////////////////////
	virtual sf::Vector2f getPoint(std::size_t index) const;

	////////////////////////////////////////////////////////////
	/// \brief Set the fill color of the shape
	///
	/// By default, the shape's fill color is opaque white
	/// 
	/// \param color -> New fill color of the shape
	///
	////////////////////////////////////////////////////////////
	void setFillColor(const sf::Color& color);

	////////////////////////////////////////////////////////////
	/// \brief Set the outline color of the shape
	///
	/// By default, the shape's outline color is White.
	///
	/// \param color -> New outline color of the shape
	///
	////////////////////////////////////////////////////////////
	void setOutlineColor(const sf::Color& color);

	////////////////////////////////////////////////////////////
	/// \brief Set the thickness of the shape's outline
	///
	/// By default, the outline thickness is 0.
	///
	/// Be aware that using a negative value for the outline
	/// thickness will cause distorted rendering.
	///
	/// \param thickness -> New outline thickness, in pixels
	///
	////////////////////////////////////////////////////////////
	void setOutlineThickness(float thickness);

	////////////////////////////////////////////////////////////
	/// \brief Set the scale factor of the shape
	/// 
	/// \param scaleFactor -> new scale factor
	///
	////////////////////////////////////////////////////////////
	void setScale(const sf::Vector2f& scaleFactor);

	////////////////////////////////////////////////////////////
	/// \brief Set the color of the button shape upon selection
	/// 
	/// \param color -> new color
	///
	////////////////////////////////////////////////////////////
	void setSelectionFillColor(const sf::Color& color);

	////////////////////////////////////////////////////////////
	/// \brief Set the color of the outline of button shape upon selection
	/// 
	/// \param color -> new color
	///
	////////////////////////////////////////////////////////////
	void setSelectionOutlineColor(const sf::Color& color);

	////////////////////////////////////////////////////////////
	/// \brief Set the thickness of the outline of button shape upon selection
	/// 
	/// \param color -> new thickness
	///
	////////////////////////////////////////////////////////////
	void setSelectionOutlineThickness(float thickness);

	////////////////////////////////////////////////////////////
	/// \brief Set the scale factor of the shape upon selection
	/// 
	/// \param scaleFactor -> new scale factor
	///
	////////////////////////////////////////////////////////////
	void setSelectionScale(const sf::Vector2f& scaleFactor);

	////////////////////////////////////////////////////////////
	/// \brief Get the local bounding rectangle of the entity
	///
	/// The returned rectangle is in local coordinates, which means
	/// that it ignores the transformations (translation, rotation,
	/// scale, ...) that are applied to the entity.
	/// In other words, this function returns the bounds of the
	/// entity in the entity's coordinate system.
	///
	/// \return Local bounding rectangle of the entity
	///
	////////////////////////////////////////////////////////////
	sf::FloatRect getLocalBounds() const;

	////////////////////////////////////////////////////////////
	/// \brief Get the global bounding rectangle of the entity
	///
	/// The returned rectangle is in global coordinates, which means
	/// that it takes into account the transformations (translation,
	/// rotation, scale, ...) that are applied to the entity.
	/// In other words, this function returns the bounds of the
	/// text in the global 2D world's coordinate system.
	///
	/// \return Global bounding rectangle of the entity
	///
	////////////////////////////////////////////////////////////
	sf::FloatRect getGlobalBounds() const;

	////////////////////////////////////////////////////////////
	/// \brief Check if the point is inside the object
	///		   Definition of if the point is inside the object may vary with different gui objects
	/// 
	/// \return true if point is inside the object, false otherwise
	/// 
	/// \param point -> Position of the point
	/// 
	////////////////////////////////////////////////////////////
	virtual bool contains(const sf::Vector2f& point) const override;

	////////////////////////////////////////////////////////////
	/// \brief Get the entity if the point is over the entity
	/// 
	/// \return Pointer to the entity if pointed by the point, nullptr otherwise
	/// 
	/// \param point -> Position of the point
	/// 
	////////////////////////////////////////////////////////////
	virtual Entity* isHit(const sf::Vector2f& point) override;

	////////////////////////////////////////////////////////////
	/// \brief Actions to take upon selection of the object
	///
	////////////////////////////////////////////////////////////
	virtual void activateSelection() override;

	////////////////////////////////////////////////////////////
	/// \brief Actions to take upon deselection of the object
	///
	////////////////////////////////////////////////////////////
	virtual void deactivateSelection() override;

	////////////////////////////////////////////////////////////
	/// \brief Draws the object to the target applying its states
	/// 
	/// \param target -> Render target to draw to
	/// \param states -> Current render states
	/// 
	////////////////////////////////////////////////////////////
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;

private:
	////////////////////////////////////////////////////////////
	/// \brief update contents of box with original or selected property
	///
	////////////////////////////////////////////////////////////
	void updateShape();


	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	/// \brief Property to change how button looks based on selection and deselection
	/// 
	////////////////////////////////////////////////////////////
	struct SelectionProperty {
		sf::Color fillColor;		/// < color of the shape
		sf::Color outlineColor;		/// < color of the outline of the shape
		float outlineThickness;		/// < thickness of the outline of the shape
		sf::Vector2f scaleFactor;	/// < scale factor of the shape
	};
	SelectionProperty m_selected;   /// < selection property
	SelectionProperty m_original;   /// < deselection property

	sf::ConvexShape m_shape;          /// < shape of the button
};

} // namespace gui