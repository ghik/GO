/*
 * Polygon.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef POLYGON_H_
#define POLYGON_H_

#include "Figure.h"

class Polygon: public Figure {
private:
	double* verts;
	int nVerts;

public:
	Polygon(double* verts, int nVerts);
	virtual ~Polygon();

	virtual void draw(cairo_t *cr) const;
};

#endif /* POLYGON_H_ */
