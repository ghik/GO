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
	beginDraw(cr, 0, 0, 0);

    double minx, maxx, miny, maxy;
    cairo_clip_extents(cr, &minx, &miny, &maxx, &maxy);

	double vx = cos(dirAngle), vy = sin(dirAngle);
	double al, ar, au, ad, ly, ry, ux, dx;
	bool lused = false, rused = false, uused = false, dused = false;
	al = (minx-begx)/vx;
	ly = begy+al*vy;
	ar = (maxx-begx)/vx;
	ry = begy+ar*vy;
	au = (maxy-begy)/vy;
	ux = begx+au*vx;
	ad = (miny-begy)/vy;
	dx = begx+ad*vx;

	double p[2][2];
	for(int i=0;i<2;i++) {
		if(ly > miny && ly < maxy && !lused) {
			p[i][0] = minx;
			p[i][1] = ly;
			lused = true;
		} else if(ry > miny && ry < maxy && !rused) {
			p[i][0] = maxx;
			p[i][1] = ry;
			rused = true;
		} else if(ux > minx && ux < maxx && !uused) {
			p[i][0] = ux;
			p[i][1] = maxy;
			uused = true;
		} else if(dx > minx && dx < maxx && !dused) {
			p[i][0] = dx;
			p[i][1] = miny;
			dused = true;
		}
	}

	applyLineColor(cr);

	cairo_move_to(cr, p[0][0], p[0][1]);
	cairo_line_to(cr, p[1][0], p[1][1]);
	cairo_stroke(cr);
	cairo_move_to(cr, begx, begy);

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

ostream& Line::serialize(ostream& str) const {
	str << "line " << begx << " " << begy << " " << deg(dirAngle) << ' '
			<< label << endl;
	return str;
}

ostream& Line::raw_serialize(ostream& str) const {
	return str << begx << " " << begy << " " << deg(dirAngle) << endl;
}

void Line::registerDraggables(vector<Draggable*> & draggables) {
	draggables.push_back(this);
}

bool Line::drags(double x, double y) {
	return dist(begx, begy, x, y) <= DRAG_RANGE/zoom;
}

void Line::draggedTo(double x, double y) {
	begx = x;
	begy = y;
}

