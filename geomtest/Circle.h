/*
 * Circle.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef CIRCLE_H_
#define CIRCLE_H_

#include "Figure.h"

class Circle: public Figure {
private:
	double cx;
	double cy;
	double r;

public:
	Circle(double cx, double cy, double r);
	virtual ~Circle();

	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;

    double getCx() const;
    double getCy() const;
    double getR() const;
    void setCx(double cx);
    void setCy(double cy);
    void setR(double r);
};

#endif /* CIRCLE_H_ */
