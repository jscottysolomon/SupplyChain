/*
map_element.hpp
*/

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <raylib.h>

//TODO implement global unique id system

class Entity {
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
		virtual void OnTick() {}
	protected:
		Vector2 position_;
};

//Trying to obscure id.
int NextId();

#endif