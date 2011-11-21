/*
 * Polygon.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef POLYGON_H_
#define POLYGON_H_

#include "Figure.h"
#include <vector>

class Polygon: public Figure {
public:
	Polygon();
	Polygon(double* verts, int nVerts);
	Polygon(const std::vector<double>& points);
	virtual ~Polygon();

	virtual void draw(cairo_t *cr) const;
	virtual ostream& serialize(ostream& str) const;
	virtual ostream& raw_serialize(ostream& str) const;
	virtual ostream& mat_serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);
	virtual void addPoint(double x, double y);

	std::vector<double> verts;
	int nVerts;
};

class PolygonVertex: public Draggable {
private:
	PolygonVertex(Polygon* polygon, int vertex);

public:
	virtual ~PolygonVertex();

	virtual bool drags(double x, double y);
	virtual void draggedTo(double x, double y);

private:
	Polygon* polygon;
	int vertex;

	friend class Polygon;
};

#endif /* POLYGON_H_ */
