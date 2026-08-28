/**
 * @file util.hpp
 * @author J. Scotty Solomon
 * @brief 
 * @date 2025-11-14
 * 
 */

#ifndef UTIL_HPP
#define UTIL_HPP

#include <algorithm>
#include <time.h>

#include <raylib.h>

double GetGlobalTime();

void PauseGlobalTime();
void ResumeGlobalTime();

bool IsGlobalTimePaused();

struct Point 
{ 
	float x; 
	float y; 
}; 

struct Line2D {
	Vector2 start;
	Vector2 end;
};

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool OnSegment(Point p, Point q, Point r);

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int Orientation(Point p, Point q, Point r) ;

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
//taken from https://gist.github.com/SuryaPratapK/4b632447abbc0e95f6e81da321b855fb
bool DoIntersect(Point p1, Point q1, Point p2, Point q2) ;

#endif