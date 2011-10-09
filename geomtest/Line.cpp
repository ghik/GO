/*
 * Line.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Line.h"

Line::Line(double _begx, double _begy, double _dirAngle) :
		begx(_begx), begy(_begy), dirAngle(_dirAngle) {
}

Line::~Line() {
}

void Line::draw(cairo_t *cr) const {
	beginDraw(cr, begx, begy, dirAngle);

	applyLineColor(cr);

	cairo_move_to(cr, -FARAWAY, 0);
	cairo_line_to(cr, FARAWAY, 0);
	cairo_move_to(cr, 0, 0);
	cairo_stroke(cr);

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

double Line::getBegx() const {
	return begx;
}

double Line::getBegy() const {
	return begy;
}

double Line::getDirAngle() const {
	return dirAngle;
}

void Line::setBegx(double begx) {
	this->begx = begx;
}

void Line::setBegy(double begy) {
	this->begy = begy;
}

void Line::setDirAngle(double dirAngle) {
	this->dirAngle = dirAngle;
}

