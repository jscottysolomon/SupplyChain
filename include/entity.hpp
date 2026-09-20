/*
map_element.hpp
*/

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <raylib.h>

//TODO implement global unique id system
int NextId();

class MapObject {
	public:
		MapObject(Vector2 pos) {
			position_ = pos;
			id_ = NextId();
		}
		MapObject() {
		}

		virtual ~MapObject() = default;
		void SetPosition(Vector2 position) {
			position_ = position;
			if(id_ < 0) {
				id_ = NextId();
			}
		}
		Vector2 GetPosition() const {
			return position_;
		}
		int GetId() const {return id_;}
		void Tick();
		virtual void Draw() {
			DrawRectangle(position_.x,position_.y,15,15,BLUE);
		}
		virtual void OnTick() = 0;
	protected:
		Vector2 position_;
		int id_;
};

//Trying to obscure id.


#endif