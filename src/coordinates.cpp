#include "coordinates.h"

void car2mapCoordinates(double car_in_map_x, double car_in_map_y, double car_in_map_psi, double car_x, double car_y, double &map_x, double &map_y) {
  map_x = cos(car_in_map_psi) * car_x - sin(car_in_map_psi) * car_y + car_in_map_x;
  map_y = sin(car_in_map_psi) * car_x + cos(car_in_map_psi) * car_y + car_in_map_y;
}

void map2carCoordinates(double car_in_map_x, double car_in_map_y, double car_in_map_psi, double map_x, double map_y, double &car_x, double &car_y) {
  car_x = cos(car_in_map_psi) * (map_x - car_in_map_x) + sin(car_in_map_psi) * (map_y - car_in_map_y);
  car_y = -sin(car_in_map_psi) * (map_x - car_in_map_x) + cos(car_in_map_psi) * (map_y - car_in_map_y);
}
