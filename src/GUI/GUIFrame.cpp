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
	m_selected = false;
	action = nullptr;
	actionEvent = ActionEvent::NONE;
	m_active = true;
	m_functionalParent = nullptr;
}

Entity::Entity(const Entity& entity)
{
	m_id = (getClassID(entity) << 24) + ++item_count;

	m_selected = entity.m_selected;
	action = entity.action;
	actionEvent = entity.actionEvent;
	m_active = entity.m_active;
	m_functionalParent = entity.m_functionalParent;

	setPosition(entity.getPosition());
	setScale(entity.getScale());
	setRotation(entity.getRotation());
	setOrigin(entity.getOrigin());
}

Entity& Entity::operator=(const Entity& entity)
{
	m_selected = entity.m_selected;
	action = entity.action;
	actionEvent = entity.actionEvent;
	m_active = entity.m_active;
	m_functionalParent = entity.m_functionalParent;

	setPosition(entity.getPosition());
	setScale(entity.getScale());
	setRotation(entity.getRotation());
	setOrigin(entity.getOrigin());

	return *this;
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
bool gui::Entity::isSelected() const
{
	return m_selected;
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
	m_navigator = -1;

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

	if(Entity::getClassID(entity) == GUI_ID_PAGE){
		m_functionalParents.insert(std::make_pair(entity.getID(), (Functional*)((Page*)(&entity))));
	}
	if(Entity::getClassID(entity) == GUI_ID_DROPDOWN){
		m_functionalParents.insert(std::make_pair(entity.getID(), (Functional*)((Dropdown*)(&entity))));
	}
	
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

void Frame::push_in_navigationOrder(Entity& entity) 
{
	m_navigationOrder.push_back(&entity);
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
				if(m_clicked->getFunctionalParent() == nullptr || m_clicked->getFunctionalParent()->getFunctionalFrame() == FunctionalObject::FRAME)
					m_clicked->callAction();
				
				// if functional parent is page then its bounds are decided by the local bounds visible on the functional parent of its functional parent
				else if(m_clicked->getFunctionalParent()->getFunctionalFrame() == FunctionalObject::PAGE &&
						m_clicked->getFunctionalParent()->getLocalBounds().contains(
							((Page*)(m_clicked->getFunctionalParent()))->getInverseTransform().transformPoint(
								((Page*)(m_clicked->getFunctionalParent()))->getFunctionalParent()->getMousePosition()
							)
						)
					) 
					m_clicked->callAction();

				// if functional parent is dropdown then its bounds are decided by the local bounds visible on the functional parent of its functional parent
				else if (m_clicked->getFunctionalParent()->getFunctionalFrame() == FunctionalObject::DROPDOWN &&
						m_clicked->getFunctionalParent()->getLocalBounds().contains(
							((Dropdown*)(m_clicked->getFunctionalParent()))->getInverseTransform().transformPoint(
								((Dropdown*)(m_clicked->getFunctionalParent()))->getFunctionalParent()->getMousePosition()
							)
						)
					)
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

				//if entity has action event as mouse hover
				if (currentMouseHoveringOn != nullptr && currentMouseHoveringOn->actionEvent == Entity::ActionEvent::MOUSEHOVER && currentMouseHoveringOn->hasAction())
					currentMouseHoveringOn->action();

				// if mouse leaves previously pointed entity
				if (m_mouseHoveringOn != currentMouseHoveringOn)
				{
					if (m_mouseHoveringOn != nullptr) {
						m_mouseHoveringOn->deactivateSelection();
					}
					m_mouseHoveringOn = currentMouseHoveringOn;

					if (m_mouseHoveringOn != nullptr) {
						m_mouseHoveringOn->activateSelection();
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

		if (m_navigator != -1) { // deactivate tab navigation
			m_navigationOrder[m_navigator]->deactivateSelection();
			m_navigator = -1;
		}

		// if textbox then trigger action to turn off cursor
		if (m_clicked != nullptr && Entity::getClassID(*m_clicked) == GUI_ID_TEXTBOX)
			m_clicked->callAction();

		m_clicked = m_mouseHoveringOn;
		
		// trigger entity's action
		if (m_clicked != nullptr && m_clicked->actionEvent == Entity::ActionEvent::PRESS && m_clicked->hasAction())
			m_clicked->callAction();

		return true; // event occurred
	}
	// if an entity is released
	else if (e.type == sf::Event::MouseButtonReleased) {

		if (m_navigator != -1) { // deactivate tab navigation
			m_navigationOrder[m_navigator]->deactivateSelection();
			m_navigator = -1;
		}

		// trigger action
		if (m_clicked != nullptr && m_clicked == m_mouseHoveringOn && e.mouseButton.button == sf::Mouse::Left){
			if (m_clicked->actionEvent == Entity::ActionEvent::RELEASE && m_clicked->hasAction()) {
				m_clicked->callAction();
			
				return true; // event occurred
			}
		}

		// textboxes whose input is enabled dont lose thier click unless something else is clicked
		if (m_clicked != nullptr && (Entity::getClassID(*m_clicked) != GUI_ID_TEXTBOX || !((Textbox*)m_clicked)->isInputEnabled())) {
			m_clicked = nullptr;
				
			return true; // event occurred
		}
	}

	else if (e.type == sf::Event::KeyPressed)
	{
		if(e.key.code == sf::Keyboard::Return){ 
			
			// if textbox was clicked, set clicked to null 
			if (m_clicked != nullptr && Entity::getClassID(*m_clicked) == GUI_ID_TEXTBOX && ((Textbox*)m_clicked)->isInputEnabled()) {
				m_clicked->callAction();
				m_clicked = nullptr;
				
				return true; // event occurred
			}

			// else set clicked to navigated entity
			else if (m_navigator != -1) {

				// if textbox then trigger action to turn off cursor
				if (m_clicked != nullptr && Entity::getClassID(*m_clicked) == GUI_ID_TEXTBOX)
					m_clicked->callAction();

				m_clicked = m_navigationOrder[m_navigator];

				// slider is an exception
				if (Entity::getClassID(*m_clicked) == GUI_ID_SLIDER) 
					m_clicked = nullptr;

				return true; // event occurred
			}
		}
		else if (e.key.code == sf::Keyboard::Left || e.key.code == sf::Keyboard::Right){
			// move slider bar accordingly
			if (m_navigator != -1 && Entity::getClassID(*m_navigationOrder[m_navigator]) == GUI_ID_SLIDER){
				float shift = 2.5;
				if (e.key.code == sf::Keyboard::Left) shift *= -1;
				((Slider*)m_navigationOrder[m_navigator])->shiftOffset(shift);

				return true; // event occurred
			}
		}

		// key navigation 
		else if (e.key.code == sf::Keyboard::Tab || e.key.code == sf::Keyboard::Down || e.key.code == sf::Keyboard::Up) {
			if (m_navigator != -1)
				m_navigationOrder[m_navigator]->deactivateSelection();

			int add = 1;
			if (e.key.code == sf::Keyboard::Up)add = -1;

			m_navigator = (m_navigator + add + m_navigationOrder.size()) % m_navigationOrder.size();

			m_navigationOrder[m_navigator]->activateSelection();

			m_clicked = nullptr;

			return true; // event occurred
		}
	}
	else if (e.type == sf::Event::KeyReleased){
		if (e.key.code == sf::Keyboard::Return){
			// if enter is pressed trigger action on navigated entity
			if (m_clicked != nullptr && m_navigator != -1 && m_clicked->getID() == m_navigationOrder[m_navigator]->getID()) {
				m_clicked->deactivateSelection();
				if(m_clicked->hasAction())m_clicked->callAction();

				// set clicked to null, textbox with input is exception
				if (Entity::getClassID(*m_clicked) != GUI_ID_TEXTBOX || !((Textbox*)m_clicked)->isInputEnabled())
					m_clicked = nullptr;

				return true; // event occurred
			}
		}
	}

	// if text is entered while textbox is selected
	else if (e.type == sf::Event::TextEntered) {
			if (e.text.unicode != 9 && e.text.unicode != 13 // 9 -> Tab, 13 -> Enter 
				&& m_clicked != nullptr && Entity::getClassID(*m_clicked) == GUI_ID_TEXTBOX && ((Textbox*)m_clicked)->isInputEnabled()) {

			Textbox* textbox = ((Textbox*)m_clicked);
			
			char c = e.text.unicode;
			if (c == 8) { // backspace is pressed
				std::string str(textbox->getString());
				if (str.size() > 0) {
					str.erase(str.size() - 1);
					textbox->setString(str);
				}
			}
			else textbox->setString(textbox->getString() + c); // normal characters or numbers

			return true; // event occurred
		}
	}

	else {
		bool wasEventPolled = false;
		// poll events in pages and dropdowns
		for (auto it = m_functionalParents.begin(); it != m_functionalParents.end() && !wasEventPolled; it++) {
			if (it->second->getFunctionalFrame() != 0 && it->second->contains(getMousePosition()))
				wasEventPolled = it->second->pollEvents(e);
		}

		return wasEventPolled; // return event
	}

	return false; // event did not occur
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
