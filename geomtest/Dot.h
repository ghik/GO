/*
 * Dot.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef DOT_H_
#define DOT_H_

#include "Figure.h"
#include "Draggable.h"

class Dot: public Figure, public Draggable {
public:
	Dot();
	Dot(double x, double y);
	virtual ~Dot();

	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;
	virtual ostream& raw_serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);
	virtual bool drags(double x, double y);
	virtual void draggedTo(double x, double y);

	double x;
	double y;
	double size;
};

#endif /* DOT_H_ */
