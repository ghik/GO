/*
 * Dot.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Dot.h"
#include <cairo/cairo.h>

Dot::Dot(double _x, double _y) :
		x(_x), y(_y), size(POINT_SIZE) {
	const double* lineColor = getLineColor();
	setFillColor(lineColor[0], lineColor[1], lineColor[2], lineColor[3]);
}

Dot::~Dot() {
}

void Dot::draw(cairo_t *cr) const {
	beginDraw(cr, x, y, rad(0));

	applyLineColor(cr);
	cairo_move_to(cr, size, 0);
	cairo_arc(cr, 0, 0, POINT_SIZE, 0, rad(360));
	cairo_stroke_preserve(cr);

	applyFillColor(cr);
	cairo_fill(cr);
	cairo_move_to(cr, 0, 0);

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

bool Dot::inside(double x, double y) const {
	return dist(this->x, this->y, x, y) <= size;
}

double Dot::getSize() const {
	return size;
}

double Dot::getX() const {
	return x;
}

double Dot::getY() const {
	return y;
}

void Dot::setSize(double size) {
	this->size = size;
}

void Dot::setX(double x) {
	this->x = x;
}

void Dot::setY(double y) {
	this->y = y;
}

