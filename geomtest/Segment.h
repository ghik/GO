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
private:
	double x1;
	double y1;
	double x2;
	double y2;

public:
	Segment(double x1, double y1, double x2, double y2);
	virtual ~Segment();

	virtual void draw(cairo_t *cr) const;
	virtual ostream& serialize(ostream& str) const;

    double getX1() const;
    double getX2() const;
    double getY1() const;
    double getY2() const;
    void setX1(double x1);
    void setX2(double x2);
    void setY1(double y1);
    void setY2(double y2);
};

#endif /* SEGMENT_H_ */
