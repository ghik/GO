/*
 * commons.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include <cmath>

double rad(double deg) {
	return deg * M_PI / 180.0L;
}

double dist(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void setColor(double* color, double r, double g, double b, double a) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}
