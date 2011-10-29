/*
 * HalfLine.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef HALFLINE_H_
#define HALFLINE_H_

#include "Figure.h"
#include "Draggable.h"

class HalfLine: public Figure, public Draggable {
public:
	HalfLine();
	HalfLine(double begx, double begy, double dirAngle);
	virtual ~HalfLine();

	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);
	virtual bool drags(double x, double y);
	virtual void draggedTo(double x, double y);

	double begx;
	double begy;
	double dirAngle;
};

#endif /* HALFLINE_H_ */
