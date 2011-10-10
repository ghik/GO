/*
 * Line.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Line.h"

Line::Line() {
}

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
	cairo_stroke(cr);
	cairo_move_to(cr, 0, 0);

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

ostream& Line::serialize(ostream& str) const {
	str << "line " << begx << " " << begy << " " << deg(dirAngle) << ' '
			<< label << endl;
	return str;
}

void Line::registerDraggables(vector<Draggable*> & draggables) {
	draggables.push_back(this);
}

bool Line::drags(double x, double y) {
	return dist(begx, begy, x, y) <= DRAG_RANGE;
}

void Line::draggedTo(double x, double y) {
	begx = x;
	begy = y;
}

