/*
 * Line.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef LINE_H_
#define LINE_H_

#include "Figure.h"

class Line: public Figure {
private:
	double begx;
	double begy;
	double dirAngle;

public:
	Line(double begx, double begy, double dirAngle);
	virtual ~Line();

	virtual void draw(cairo_t* cr) const;
    double getBegx() const;
    double getBegy() const;
    double getDirAngle() const;
    void setBegx(double begx);
    void setBegy(double begy);
    void setDirAngle(double dirAngle);
};

#endif /* LINE_H_ */
