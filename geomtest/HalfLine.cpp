/*
 * HalfLine.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "HalfLine.h"

HalfLine::HalfLine() {
}

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

ostream& HalfLine::serialize(ostream& str) const {
	str << "line " << begx << " " << begy << " " << deg(dirAngle) << ' '
			<< label << endl;
	return str;
}

void HalfLine::registerDraggables(vector<Draggable*> & draggables) {
	draggables.push_back(this);
}

bool HalfLine::drags(double x, double y) {
	return dist(begx, begy, x, y) <= DRAG_RANGE;
}

void HalfLine::draggedTo(double x, double y) {
	begx = x;
	begy = y;
}

