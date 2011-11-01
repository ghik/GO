/*
 * Polyline.h
 *
 *  Created on: 20-10-2011
 *      Author: ghik
 */

#ifndef POLYLINE_H_
#define POLYLINE_H_

#include "Figure.h"
#include <vector>

class Polyline: public Figure {
public:
	Polyline();
	Polyline(double* verts, int nVerts);
	Polyline(const std::vector<double>& points);
	virtual ~Polyline();

	virtual void draw(cairo_t *cr) const;
	virtual ostream& serialize(ostream& str) const;
	virtual ostream& raw_serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);

	std::vector<double> verts;
	int nVerts;
};

class PolylineVertex: public Draggable {
private:
	PolylineVertex(Polyline* polyline, int vertex);

public:
	virtual ~PolylineVertex();

	virtual bool drags(double x, double y);
	virtual void draggedTo(double x, double y);

private:
	Polyline* polyline;
	int vertex;

	friend class Polyline;
};

#endif /* POLYLINE_H_ */
