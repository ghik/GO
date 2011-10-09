/*
 * commons.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#define FARAWAY 2000

const double LINE_WIDTH = 1;
const double POINT_SIZE = 3;
const double FONT_SIZE = 14;
const char* const FONT_FACE = "URW Chancery L";
const double LABEL_OFFSET[2] = { 5, -5 };

double rad(double deg);
double dist(double x1, double y1, double x2, double y2);
void setColor(double* color, double r, double g, double b, double a);

#endif /* COMMONS_H_ */
