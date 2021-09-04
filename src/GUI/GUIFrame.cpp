#include "GUIFrame.hpp"
#include "Page.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "TextButton.hpp"
#include "Dropdown.hpp"

#include <iostream>

using namespace gui;

// set the initial count to 0
unsigned int Entity::item_count = 0;

Entity::Entity(unsigned int class_id)
{
	/*
	* generate unique id
	* first 8 bits -> class id
	* last 24 bits item count at the time of construction of entity
	*/
	m_id = (class_id << 24) + ++item_count;

	// set defaults
	m_isSelected = false;
	action = nullptr;
	actionEvent = ActionEvent::NONE;
	m_active = true;
	m_functionalParent = nullptr;
}
Entity::~Entity()
{

}
unsigned int Entity::getID() const
{
	return m_id;
}
unsigned int Entity::getClassID(const Entity& entity)
{
	// get the 8 most significan bits
	return entity.getID() >> 24;
}
Functional* gui::Entity::getFunctionalParent() const
{
	return m_functionalParent;
}
bool gui::Entity::isSelected()
{
	return m_isSelected;
}
void Entity::setActive()
{
	m_active = true;
}
void Entity::setInactive()
{
	m_active = false;
}
bool Entity::isActive() const
{
	return m_active;
}
void Entity::callAction() const
{
	if(isActive())
		action();
}
bool Entity::hasAction() const
{
	return action != nullptr;
}
void Entity::setAction(std::function<void()> func)
{
	action = func;
}

void Entity::setFunctionalParentForSubVariables(Functional* parent)
{

}

void gui::setFunctionalParent(Entity& entity, Functional* parent)
{
	entity.m_functionalParent = parent;

	entity.setFunctionalParentForSubVariables(parent);
}


std::unordered_map<std::string, unsigned int> Frame::m_nameMap;

Frame::Frame()
{
	// set defaults
	m_window = nullptr;
	m_mouseHoveringOn = nullptr;
	m_clicked = nullptr;

	m_functional_object = FunctionalObject::FRAME;
}
Frame::~Frame()
{

}

void Frame::setWindow(sf::RenderWindow& window)
{
	m_window = &window;
}

void Frame::addEntity(Entity& entity)
{
	// insert in map
	m_entityMap.insert(std::make_pair(entity.getID(), &entity));	
	
	setFunctionalParent(entity, this);
}

void Frame::removeEntity(Entity& entity)
{
	// set entity's frame to null
	//setEntityFrame(entity, nullptr);

	// if entity's name exists remove it
	std::string name = getName(entity.getID());
	if (name != "")removeName(name);

	// erase entity from map
	m_entityMap.erase(entity.getID());
}

void Frame::removeEntity(unsigned int id)
{
	m_entityMap.erase(id);
}

void Frame::setName(const Entity& entity, const std::string& name)
{
	m_nameMap[name] = entity.getID();
}

void Frame::removeName(const std::string& name)
{
	m_nameMap.erase(name);
}

Entity* Frame::getByID(unsigned int id) const
{
	return m_entityMap.at(id);
}

Entity* Frame::getByName(const std::string& name) const
{
	return m_entityMap.at(m_nameMap.at(name));
}
unsigned int Frame::getIDByName(const std::string& name)
{
	return m_nameMap.at(name);
}
std::string Frame::getName(unsigned int id)
{
	for (auto it = m_nameMap.begin(); it != m_nameMap.end(); it++) {
		if (it->second == id)return it->first;
	}
	return "";
}
sf::Vector2f Frame::getMousePosition() const
{
	return m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
}
sf::Vector2f Frame::getLastMouseOffset() const
{
	return getMousePosition() - m_lastMousePos;
}
bool Frame::contains(const sf::Vector2f& point) const
{
	return getLocalBounds().contains(point);
}
sf::FloatRect Frame::getLocalBounds() const
{
	return sf::FloatRect(0, 0, m_window->getSize().x, m_window->getSize().y);
}
void Frame::update()
{
	// only work if window has focus
	if (m_window->hasFocus()) {
		// only work if mouse is inside the window
		if(!contains(getMousePosition())){
			if (m_mouseHoveringOn != nullptr)m_mouseHoveringOn->deactivateSelection();
			m_mouseHoveringOn = nullptr;
			m_clicked = nullptr;
		}
		else {
			// if mouse is moved while being held on something
			if (m_clicked != nullptr && m_clicked->actionEvent == Entity::ActionEvent::MOUSEHELD && m_clicked->hasAction()) {
				// if functional parent is frame or nullptr then object does not depend bounds (control by other entities) 
				if(m_clicked->getFunctionalParent() == nullptr ||  m_clicked->getFunctionalParent()->getFunctionalFrame() == FunctionalObject::FRAME) {
					m_clicked->callAction();
				}
				// if functional parent is page then its bounds are decided by the local bounds visible on the functional parent of its functional parent
				else if(m_clicked->getFunctionalParent()->getFunctionalFrame() == FunctionalObject::PAGE &&
					m_clicked->getFunctionalParent()->getLocalBounds().contains(
						((Page*)(m_clicked->getFunctionalParent()))->getInverseTransform().transformPoint(
							((Page*)(m_clicked->getFunctionalParent()))->getFunctionalParent()->getMousePosition()
						)
					))
					m_clicked->callAction();
				// if functional parent is dropdown then its bounds are decided by the local bounds visible on the functional parent of its functional parent
				else if (m_clicked->getFunctionalParent()->getFunctionalFrame() == FunctionalObject::DROPDOWN &&
					m_clicked->getFunctionalParent()->getLocalBounds().contains(
						((Dropdown*)(m_clicked->getFunctionalParent()))->getInverseTransform().transformPoint(
							((Dropdown*)(m_clicked->getFunctionalParent()))->getFunctionalParent()->getMousePosition()
						)
					))
					m_clicked->callAction();
				// if out of bounds
				else {
					m_mouseHoveringOn->deactivateSelection();
					m_clicked = nullptr;
					m_mouseHoveringOn = nullptr;
				}
			}
			else {
				// search through all entities if mouse is in window
				Entity* currentMouseHoveringOn = nullptr;
				for (auto it = m_entityMap.begin(); it != m_entityMap.end() && currentMouseHoveringOn == nullptr; it++) {
					currentMouseHoveringOn = it->second->isHit(getMousePosition());
				}

				// if mouse leaves previously pointed entity
				if (m_mouseHoveringOn != currentMouseHoveringOn)
				{
					if (m_mouseHoveringOn != nullptr) {
						m_mouseHoveringOn->deactivateSelection();

						//if entity has action event as mouse hover
						if (m_mouseHoveringOn->actionEvent == Entity::ActionEvent::MOUSEHOVER && m_mouseHoveringOn->hasAction())
							m_mouseHoveringOn->action();
					}
					m_mouseHoveringOn = currentMouseHoveringOn;

					if (m_mouseHoveringOn != nullptr) {
						m_mouseHoveringOn->activateSelection();

						//if entity has action event as mouse hover
						if (m_mouseHoveringOn->actionEvent == Entity::ActionEvent::MOUSEHOVER && m_mouseHoveringOn->hasAction())
							m_mouseHoveringOn->action();
					}
				}
			}
		}
		//update last mouse pos
		m_lastMousePos = getMousePosition();
	}
}
bool Frame::pollEvents(sf::Event e)
{
	// if an entity is clicked
	if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
		m_clicked = m_mouseHoveringOn;

		if (m_clicked != nullptr && m_clicked->actionEvent == Entity::ActionEvent::PRESS && m_clicked->hasAction())
			m_clicked->callAction();

		return true;
	}
	// if an entity is released
	else if (e.type == sf::Event::MouseButtonReleased) {
		if (m_clicked != nullptr && m_clicked == m_mouseHoveringOn && e.mouseButton.button == sf::Mouse::Left)
		{
			if (m_clicked->actionEvent == Entity::ActionEvent::RELEASE && m_clicked->hasAction())
				m_clicked->callAction();
		}
		// textboxes whose input is enabled dont lose thier click unless something else is clicked
		if(m_clicked != nullptr && (Entity::getClassID(*m_clicked) != GUI_ID_TEXTBOX || !((Textbox*)m_clicked)->isInputEnabled()))
			m_clicked = nullptr;

		return true;
	}
	// if mouse wheel is scrolled while mouse is over a page or dropdown
	else if (e.type == sf::Event::MouseWheelScrolled) {
		bool wasEventPolled = false;
		// poll events in pages and dropdowns
		for (auto it = m_entityMap.begin(); it != m_entityMap.end() && !wasEventPolled; it++) {
			if (Entity::getClassID(*it->second) == GUI_ID_PAGE && ((Page*)it->second)->contains(getMousePosition()))
				wasEventPolled = ((Page*)it->second)->pollEvents(e);
			else if (Entity::getClassID(*it->second) == GUI_ID_DROPDOWN && ((Dropdown*)it->second)->containsExcludingHeader(getMousePosition()))
				wasEventPolled = ((Dropdown*)it->second)->pollEvents(e);
		}
	}
	// if text is entered while textbox is seleccted
	else if (m_clicked != nullptr && Entity::getClassID(*m_clicked) == GUI_ID_TEXTBOX && ((Textbox*)m_clicked)->isInputEnabled() && e.type == sf::Event::TextEntered) {
		Textbox& textbox = *((Textbox*)m_clicked);
		char c = e.text.unicode;
		if (c == 13) textbox.setString(textbox.getString() + '\n'); // enter is pressed
		else if (c == 8) { // backspace is pressed
			std::string str(textbox.getString());
			if (str.size() > 0) {
				str.erase(str.size() - 1);
				textbox.setString(str);
			}
		}
		else textbox.setString(textbox.getString() + c); // normal characters or numbers
	}

	return false;
}
void Frame::draw()
{
	for (auto it = m_entityMap.begin(); it != m_entityMap.end(); it++) {
		it->second->draw(*m_window);
	}
}

Functional::FunctionalObject Functional::getFunctionalFrame()
{
	return m_functional_object;
}
