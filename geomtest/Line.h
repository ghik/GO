/*
 * Line.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef LINE_H_
#define LINE_H_

#include "Figure.h"
#include "Draggable.h"

class Line: public Figure, public Draggable {
public:
	Line();
	Line(double begx, double begy, double dirAngle);
	virtual ~Line();

	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);
	virtual bool drags(double x, double y);
	virtual void draggedTo(double x, double y);

	double begx;
	double begy;
	double dirAngle;
};

#endif /* LINE_H_ */
