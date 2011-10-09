/*
 * Segment.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "Segment.h"
#include <cmath>

Segment::Segment(double _x1, double _y1, double _x2, double _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2) {
}

Segment::~Segment() {
}

void Segment::draw(cairo_t* cr) const {
	beginDraw(cr, 0, 0, 0);

	applyLineColor(cr);
	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);

	cairo_move_to(cr, x1, y1);
	cairo_rotate(cr, atan((y2 - y1) / (x2 - x1)));

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

double Segment::getX1() const {
	return x1;
}

double Segment::getX2() const {
	return x2;
}

double Segment::getY1() const {
	return y1;
}

double Segment::getY2() const {
	return y2;
}

void Segment::setX1(double x1) {
	this->x1 = x1;
}

void Segment::setX2(double x2) {
	this->x2 = x2;
}

void Segment::setY1(double y1) {
	this->y1 = y1;
}

void Segment::setY2(double y2) {
	this->y2 = y2;
}
