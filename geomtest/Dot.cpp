/*
 * Dot.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Dot.h"
#include <cairo/cairo.h>

Dot::Dot() {
}

Dot::Dot(double _x, double _y) :
		x(_x), y(_y), size(POINT_SIZE) {
	setColor(fillColor, lineColor);
}

Dot::~Dot() {
}

void Dot::draw(cairo_t *cr) const {
	beginDraw(cr, x, y, rad(0));

	applyLineColor(cr);
	cairo_move_to(cr, size/fabs(zoom), 0);
	cairo_arc(cr, 0, 0, size/fabs(zoom), 0, rad(360));
	cairo_stroke_preserve(cr);

	applyFillColor(cr);
	cairo_fill(cr);
	cairo_move_to(cr, 0, 0);

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

void Dot::registerDraggables(vector<Draggable*>& draggables) {
	draggables.push_back(this);
}

bool Dot::drags(double x, double y) {
	return dist(this->x, this->y, x, y) <= size/zoom;
}

void Dot::draggedTo(double x, double y) {
	this->x = x;
	this->y = y;
}

ostream& Dot::serialize(ostream& str) const {
	str << "dot " << x << " " << y << " " << size << ' ' << label << endl;
	return str;
}

ostream& Dot::raw_serialize(ostream& str) const {
	return str << x << ' ' << y << endl;
}
