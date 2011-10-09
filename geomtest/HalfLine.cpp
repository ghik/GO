/*
 * HalfLine.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "HalfLine.h"

HalfLine::HalfLine(double _begx, double _begy, double _dirAngle) :
		begx(_begx), begy(_begy), dirAngle(_dirAngle) {
}

HalfLine::~HalfLine() {
}

void HalfLine::draw(cairo_t *cr) const {
	beginDraw(cr, begx, begy, dirAngle);

	applyLineColor(cr);
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, FARAWAY, 0);
	cairo_stroke(cr);
	cairo_move_to(cr, 0, 0);

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

double HalfLine::getBegx() const {
	return begx;
}

double HalfLine::getBegy() const {
	return begy;
}

double HalfLine::getDirAngle() const {
	return dirAngle;
}

void HalfLine::setBegx(double begx) {
	this->begx = begx;
}

void HalfLine::setBegy(double begy) {
	this->begy = begy;
}

void HalfLine::setDirAngle(double dirAngle) {
	this->dirAngle = dirAngle;
}
