/*
 * figure.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef FIGURE_H_
#define FIGURE_H_

#include "commons.h"
#include <string>
#include <ostream>
#include <vector>
#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include "Draggable.h"
using namespace std;

class Figure {
public:
	Figure();
	virtual ~Figure();

protected:
	void applyLineColor(cairo_t* cr) const;
	void applyFillColor(cairo_t* cr) const;
	void beginDraw(cairo_t* cr, double x, double y, double angle) const;
	void putLabel(cairo_t* cr) const;
	void endDraw(cairo_t* cr) const;
	void repaint() const;

public:
	virtual void registerDraggables(std::vector<Draggable*>& draggables) = 0;
	virtual void draw(cairo_t* cr) const = 0;
	virtual ostream& serialize(ostream& str) const = 0;
	void setWidget(GtkWidget* widget);

	string label;
	double lineColor[4];
	double fillColor[4];
	double labelColor[4];
	int begframe;
	int endframe;

private:
	GtkWidget* widget;
};

ostream& operator<<(ostream& str, const Figure& figure);

#endif /* FIGURE_H_ */
