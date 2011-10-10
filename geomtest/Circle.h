/*
 * Circle.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef CIRCLE_H_
#define CIRCLE_H_

#include <vector>
#include "Figure.h"

class Circle: public Figure {
public:
	Circle();
	Circle(double cx, double cy, double r);
	virtual ~Circle();

	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;

	virtual void registerDraggables(std::vector<Draggable*>& draggables);

	double cx;
	double cy;
	double r;
};

#endif /* CIRCLE_H_ */
