#include "util.hpp"

#include <algorithm>
#include <time.h>

clock_t current_time;

void SetGlobalTime() {
    current_time = clock();
}

/**
 * @brief returns global tick time
 * @return global tick time
 */
clock_t GetGlobalTime() {
    return current_time;
}

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool OnSegment(Point p, Point q, Point r) 
{ 
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && 
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y)) 
	return true; 

	return false; 
} 

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int Orientation(Point p, Point q, Point r) 
{ 
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
	// for details of below formula. 
	int val = (q.y - p.y) * (r.x - q.x) - 
			(q.x - p.x) * (r.y - q.y); 

	if (val == 0) return 0; // colinear 

	return (val > 0)? 1: 2; // clock or counterclock wise 
} 

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
//taken from https://gist.github.com/SuryaPratapK/4b632447abbc0e95f6e81da321b855fb
bool DoIntersect(Point p1, Point q1, Point p2, Point q2) 
{ 
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = Orientation(p1, q1, p2); 
	int o2 = Orientation(p1, q1, q2); 
	int o3 = Orientation(p2, q2, p1); 
	int o4 = Orientation(p2, q2, q1); 

	// General case 
	if (o1 != o2 && o3 != o4) 
		return true; 

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && OnSegment(p1, p2, q1)) return true; 

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true; 

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true; 

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true; 

	return false; // Doesn't fall in any of the above cases 
} 