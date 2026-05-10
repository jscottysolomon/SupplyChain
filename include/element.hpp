/*
map_element.hpp
*/

#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <raylib.h>

class Element {
	protected:
		Vector2 position_;
	public:
		void SetPosition(Vector2 position) {
			position_ = position;
		}
		Vector2 GetPosition() {
			return position_;
		}
		void Tick();
		virtual void Draw() {
			DrawRectangle(position_.x,position_.y,15,15,BLUE);
		}
};

#endif