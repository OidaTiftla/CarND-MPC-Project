#include <math.h>

#ifndef COORDINATES_H
#define COORDINATES_H

// convert car coordinates (car_x, car_y) into map coordinates (map_x, map_y)
// where the car is at (car_in_map_x, car_in_map_y) in the map
// and is oriented in an angle of car_in_map_psi to the x-axis of the map.
void car2mapCoordinates(double car_in_map_x, double car_in_map_y, double car_in_map_psi, double car_x, double car_y, double &map_x, double &map_y);

// convert map coordinates (map_x, map_y) into car coordinates (car_x, car_y)
// where the car is at (car_in_map_x, car_in_map_y) in the map
// and is oriented in an angle of car_in_map_psi to the x-axis of the map.
void map2carCoordinates(double car_in_map_x, double car_in_map_y, double car_in_map_psi, double map_x, double map_y, double &car_x, double &car_y);

#endif /*COORDINATES_H*/