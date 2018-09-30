#include <math.h>

#ifndef COORDINATES_HPP
#define COORDINATES_HPP

// convert map coordinates (map_x, map_y) into car coordinates (car_x, car_y)
// where the car is at (car_in_map_x, car_in_map_y) in the map
// and is oriented in an angle of car_in_map_psi to the x-axis of the map.
void map2carCoordinates(double car_in_map_x, double car_in_map_y, double car_in_map_psi, double map_x, double map_y, double &car_x, double &car_y) {
  car_x = cos(car_in_map_psi) * (map_x - car_in_map_x) + sin(car_in_map_psi) * (map_y - car_in_map_y);
  car_y = -sin(car_in_map_psi) * (map_x - car_in_map_x) + cos(car_in_map_psi) * (map_y - car_in_map_y);
}

#endif /*COORDINATES_HPP*/