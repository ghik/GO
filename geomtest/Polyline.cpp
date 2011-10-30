/*
 * Polyline.cpp
 *
 *  Created on: 20-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Polyline.h"

Polyline::Polyline() {
}

Polyline::Polyline(double* _verts, int _nVerts) :
		nVerts(_nVerts) {
	for (int i = 0; i < nVerts; i++) {
		verts.push_back(_verts[2 * i]);
		verts.push_back(_verts[2 * i + 1]);
	}
}

Polyline::Polyline(const std::vector<double>& points) :
		nVerts(points.size() / 2) {
	for (int i = 0; i < nVerts; i++) {
		verts.push_back(points[2 * i]);
		verts.push_back(points[2 * i + 1]);
	}
}

Polyline::~Polyline() {
}

void Polyline::draw(cairo_t *cr) const {
	if(nVerts < 2) {
		return;
	}
	beginDraw(cr, 0, 0, 0);

	applyLineColor(cr);
	cairo_move_to(cr, verts[0], verts[1]);
	for (int i = 1; i < nVerts; i++) {
		cairo_line_to(cr, verts[2 * i], verts[2 * i + 1]);
	}
	cairo_stroke_preserve(cr);

	endDraw(cr);
}

ostream& Polyline::serialize(ostream& str) const {
	str << "polyline ";
	for (int i = 0; i < nVerts * 2; i++) {
		str << verts[i] << " ";
	}
	str << endl;
	return str;
}

void Polyline::registerDraggables(vector<Draggable*> & draggables) {
	for(int i=0;i<nVerts;i++) {
		draggables.push_back(new PolylineVertex(this, i));
	}
}

PolylineVertex::PolylineVertex(Polyline *_polyline, int _vertex): polyline(_polyline), vertex(_vertex) {
}

PolylineVertex::~PolylineVertex() {
}

bool PolylineVertex::drags(double x, double y) {
	return dist(polyline->verts[2*vertex], polyline->verts[2*vertex+1], x, y) <= DRAG_RANGE/zoom;
}

void PolylineVertex::draggedTo(double x, double y) {
	polyline->verts[2*vertex] = x;
	polyline->verts[2*vertex+1] = y;
}

