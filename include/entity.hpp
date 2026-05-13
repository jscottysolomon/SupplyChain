/*
map_element.hpp
*/

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <raylib.h>

//TODO implement global unique id system

class Entity {
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
		virtual void OnTick() {}
};

//Trying to obscure id.
int NextId();

#endif