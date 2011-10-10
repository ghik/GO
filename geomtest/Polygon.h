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
private:
	double* verts;
	int nVerts;

public:
	Polygon(double* verts, int nVerts);
	Polygon(const std::vector<double>& points);
	virtual ~Polygon();

	virtual void draw(cairo_t *cr) const;
	virtual ostream& serialize(ostream& str) const;
};

#endif /* POLYGON_H_ */
