/*
 * Polygon.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Polygon.h"

Polygon::Polygon(): nVerts(0) {
}

Polygon::Polygon(double* _verts, int _nVerts) :
		nVerts(_nVerts) {
	for (int i = 0; i < nVerts; i++) {
		verts.push_back(_verts[2 * i]);
		verts.push_back(_verts[2 * i + 1]);
	}
}

Polygon::Polygon(const std::vector<double>& points) :
		nVerts(points.size() / 2) {
	for (int i = 0; i < nVerts; i++) {
		verts.push_back(points[2 * i]);
		verts.push_back(points[2 * i + 1]);
	}
}

Polygon::~Polygon() {
}

void Polygon::draw(cairo_t *cr) const {
	if(nVerts < 3) {
		return;
	}
	beginDraw(cr, 0, 0, 0);

	applyLineColor(cr);
	cairo_move_to(cr, verts[0], verts[1]);
	for (int i = 1; i < nVerts; i++) {
		cairo_line_to(cr, verts[2 * i], verts[2 * i + 1]);
	}
	cairo_line_to(cr, verts[0], verts[1]);
	cairo_stroke_preserve(cr);

	applyFillColor(cr);
	cairo_fill(cr);

	endDraw(cr);
}

ostream& Polygon::serialize(ostream& str) const {
	str << "polygon ";
	for (int i = 0; i < nVerts * 2; i++) {
		str << verts[i] << " ";
	}
	str << endl;
	return str;
}

void Polygon::registerDraggables(vector<Draggable*> & draggables) {
	for(int i=0;i<nVerts;i++) {
		draggables.push_back(new PolygonVertex(this, i));
	}
}

void Polygon::addPoint(double x, double y) {
	verts.push_back(x);
	verts.push_back(y);
	nVerts++;
}

PolygonVertex::PolygonVertex(Polygon *_polygon, int _vertex): polygon(_polygon), vertex(_vertex) {
}

PolygonVertex::~PolygonVertex() {
}

bool PolygonVertex::drags(double x, double y) {
	return dist(polygon->verts[2*vertex], polygon->verts[2*vertex+1], x, y) <= DRAG_RANGE/zoom;
}

void PolygonVertex::draggedTo(double x, double y) {
	polygon->verts[2*vertex] = x;
	polygon->verts[2*vertex+1] = y;
}

