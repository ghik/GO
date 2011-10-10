/*
 * Dot.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef DOT_H_
#define DOT_H_

#include "Figure.h"

class Dot: public Figure {
public:
	Dot(double x, double y);
	virtual ~Dot();

private:
	double x;
	double y;
	double size;

public:
	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;

	bool inside(double x, double y) const;

    double getSize() const;
    double getX() const;
    double getY() const;
    void setSize(double size);
    void setX(double x);
    void setY(double y);
};

#endif /* DOT_H_ */
