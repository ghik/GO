/*
 * Polygon.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "Polygon.h"

Polygon::Polygon(double* _verts, int _nVerts): nVerts(_nVerts) {
	verts = new double[nVerts*2];
	for(int i=0;i<nVerts;i++) {
		verts[2*i] = _verts[2*i];
		verts[2*i+1] = _verts[2*i+1];
	}
}

Polygon::~Polygon() {
	delete [] verts;
}

void Polygon::draw(cairo_t *cr) const {
	beginDraw(cr, 0, 0, 0);

	applyLineColor(cr);
	cairo_move_to(cr, verts[0], verts[1]);
	for(int i=1;i<nVerts;i++) {
		cairo_line_to(cr, verts[2*i], verts[2*i+1]);
	}
	cairo_line_to(cr, verts[0], verts[1]);
	cairo_stroke_preserve(cr);

	applyFillColor(cr);
	cairo_fill(cr);

	endDraw(cr);
}
