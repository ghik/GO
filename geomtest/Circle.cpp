/*
 * Circle.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Circle.h"

Circle::Circle() {
}

Circle::Circle(double _cx, double _cy, double _r) :
		cx(_cx), cy(_cy), r(_r) {
}

Circle::~Circle() {
}

void Circle::draw(cairo_t *cr) const {
	beginDraw(cr, cx, cy, 0);
	applyLineColor(cr);

	cairo_move_to(cr, r, 0);
	cairo_arc(cr, 0, 0, r, 0, rad(360));
	cairo_stroke_preserve(cr);

	applyFillColor(cr);
	cairo_fill(cr);

	cairo_move_to(cr, 0, -r);
	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

ostream & Circle::serialize(ostream & str) const {
	str << "circle " << cx << ' ' << cy << ' ' << r << ' ' << label << endl;
	return str;
}

void Circle::registerDraggables(std::vector<Draggable*> & draggables) {
}

