/**
 * @file intersection_control.hpp
 * @author your name (you@domain.com)
 * @brief Intersection Mediator for notifying vehicles of intersection changes, and who is in line at the intersection.
 * @version 0.1
 * @date 2026-05-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef INTERSECTION_CONTROL_HPP
#define INTERSECTION_CONTROL_HPP

void RequestIntersectionAccess();
void CanEnterIntersection();
void NotifyIntersectionEntered();
void NotifyIntersectionExited();
void QueueTruckAtIntersection();
void ReleaseWaitingTruck();

#endif