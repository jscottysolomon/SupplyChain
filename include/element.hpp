/*
map_element.hpp
*/

#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <raylib.h>

class Element {
	private:
		Vector2 position;
	public:
		void SetPosition(Vector2);
		Vector2 GetPosition();
};

#endif