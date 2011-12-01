/*
 * Parabola.cpp
 *
 *  Created on: 26-11-2011
 *      Author: ghik
 */

#include "Parabola.h"
#include "commons.h"
#include <cmath>
#include <algorithm>

#include <iostream>
using namespace std;

Parabola::Parabola():
	xc(0), yc(0), a(1), y1(-INFINITY), y2(INFINITY) {
}

Parabola::Parabola(double _xc, double _yc, double _a, double _y1, double _y2):
	xc(_xc), yc(_yc), a(_a), y1(_y1), y2(_y2) {
	norm();
}

Parabola::~Parabola() {
}

void Parabola::norm() {
	if(y1 > y2) {
		swap(y1, y2);
	}
}

void Parabola::draw(cairo_t *cr) const {
	beginDraw(cr, 0, 0, 0);

    double clip[4];
    cairo_clip_extents(cr, &clip[0], &clip[1], &clip[2], &clip[3]);

	vector<double> verts;
	partition(y1, y2, verts, clip);

	applyLineColor(cr);
	if(!verts.empty()) {
		cairo_move_to(cr, verts[0], verts[1]);
		for(unsigned int i=2;i<verts.size();i+=2) {
			cairo_line_to(cr, verts[i], verts[i+1]);
		}
		cairo_stroke(cr);
	}

	endDraw(cr);
}

ostream & Parabola::serialize(ostream & str) const {
	return str << "parabola " << xc << ' ' << yc << ' ' << a << ' ' << y1 << ' ' << y2 << endl;
}

ostream & Parabola::raw_serialize(ostream & str) const {
	return str << xc << ' ' << yc << ' ' << a << ' ' << y1 << ' ' << y2 << endl;
}

void Parabola::registerDraggables(vector<Draggable*> & draggables) {
}

void Parabola::partition(double iy1, double iy2, vector<double>& pts, double clip[]) const {
	double ix1 = a*(iy1-yc)*(iy1-yc)+xc;
	double ix2 = a*(iy2-yc)*(iy2-yc)+xc;
	double d = (iy1-iy2)*(iy1-iy2)+(ix1-ix2)*(ix1-ix2)*zoom*zoom;
	if(d > 3*3) {
		double icy = (iy1+iy2)/2;
		partition(iy1, icy, pts, clip);
		partition(icy, iy2, pts, clip);
	} else if(visible(ix1, iy1, clip)) {
		pts.push_back(ix1);
		pts.push_back(iy1);
		if(iy1 < yc && iy2 > yc) {
			pts.push_back(xc);
			pts.push_back(yc);
		}
	}
	if(iy2 == y2 && visible(ix2, iy2, clip)) {
		pts.push_back(ix2);
		pts.push_back(iy2);
	}
}

bool Parabola::visible(double x, double y, double clip[]) const {
	double d = 15/zoom;
	return x+d >= clip[0] && x-d <= clip[2] &&
			y+d >= clip[1] && y-d <= clip[3];
}





