/*
 * HalfLine.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "HalfLine.h"
#include <iostream>
using namespace std;

HalfLine::HalfLine() {
}

HalfLine::HalfLine(double _begx, double _begy, double _dirAngle) :
		begx(_begx), begy(_begy), dirAngle(_dirAngle) {
}

HalfLine::~HalfLine() {
}

void HalfLine::draw(cairo_t *cr) const {
	beginDraw(cr, 0, 0, 0);

	double minx = 0, maxx = SCREEN_SIZE, miny = SCREEN_SIZE, maxy = 0;
	screen_to_view(&minx, &miny);
	screen_to_view(&maxx, &maxy);

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
		if(al > 0 && ly > miny && ly < maxy && !lused) {
			p[i][0] = minx;
			p[i][1] = ly;
			lused = true;
		} else if(ar > 0 && ry > miny && ry < maxy && !rused) {
			p[i][0] = maxx;
			p[i][1] = ry;
			rused = true;
		} else if(au > 0 && ux > minx && ux < maxx && !uused) {
			p[i][0] = ux;
			p[i][1] = maxy;
			uused = true;
		} else if(ad > 0 && dx > minx && dx < maxx && !dused) {
			p[i][0] = dx;
			p[i][1] = miny;
			dused = true;
		} else {
			p[i][0] = begx;
			p[i][1] = begy;
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

ostream& HalfLine::serialize(ostream& str) const {
	str << "line " << begx << " " << begy << " " << deg(dirAngle) << ' '
			<< label << endl;
	return str;
}

void HalfLine::registerDraggables(vector<Draggable*> & draggables) {
	draggables.push_back(this);
}

bool HalfLine::drags(double x, double y) {
	return dist(begx, begy, x, y) <= DRAG_RANGE/zoom;
}

void HalfLine::draggedTo(double x, double y) {
	begx = x;
	begy = y;
}

