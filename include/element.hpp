/*
map_element.hpp
*/

#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <raylib.h>

class Element {
	private:
		Vector2 position_;
	public:
		void SetPosition(Vector2 position) {
			position_ = position;
		}
		Vector2 GetPosition() {
			return position_;
		}
		void Tick();
		void Draw() {
			DrawRectangle(position_.x,position_.y,10,10,BLUE);
		}
};

#endif