/*
 * Parabola.h
 *
 *  Created on: 26-11-2011
 *      Author: ghik
 */

#ifndef PARABOLA_H_
#define PARABOLA_H_

#include "Figure.h"

class Parabola: public Figure {
public:
	Parabola();
	Parabola(double _xc, double _yc, double _a, double _y1, double _y2);
	virtual ~Parabola();
	void norm();

	virtual void draw(cairo_t *cr) const;
	virtual ostream& serialize(ostream& str) const;
	virtual ostream& raw_serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);

	double xc, yc;
	double a;
	double y1, y2;

private:
	void partition(double iy1, double iy2, vector<double>& pts, double clip[]) const;
	bool visible(double x, double y, double clip[]) const;
};

#endif /* PARABOLA_H_ */
