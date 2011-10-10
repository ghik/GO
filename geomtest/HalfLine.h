/*
 * HalfLine.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef HALFLINE_H_
#define HALFLINE_H_

#include "Figure.h"

class HalfLine: public Figure {
private:
	double begx;
	double begy;
	double dirAngle;

public:
	HalfLine(double begx, double begy, double dirAngle);
	virtual ~HalfLine();

	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;

    double getBegx() const;
    double getBegy() const;
    double getDirAngle() const;
    void setBegx(double begx);
    void setBegy(double begy);
    void setDirAngle(double dirAngle);
};

#endif /* HALFLINE_H_ */
