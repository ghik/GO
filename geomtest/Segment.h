/*
 * Segment.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include "Figure.h"

class Segment: public Figure {
public:
	Segment();
	Segment(double x1, double y1, double x2, double y2);
	virtual ~Segment();

	virtual void draw(cairo_t *cr) const;
	virtual ostream& serialize(ostream& str) const;
	virtual ostream& raw_serialize(ostream& str) const;

	virtual void registerDraggables(vector<Draggable*>& draggables);

	double x1;
	double y1;
	double x2;
	double y2;
};

class SegmentEnd: public Draggable {
private:
	SegmentEnd(Segment* seg, bool beg);

public:
	virtual ~SegmentEnd();

	virtual bool drags(double x, double y);
	virtual void draggedTo(double x, double y);

private:
	Segment* seg;
	bool beg;

	friend class Segment;
};

#endif /* SEGMENT_H_ */
