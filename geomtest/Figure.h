/*
 * figure.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef FIGURE_H_
#define FIGURE_H_

#include <string>
#include <gtk/gtk.h>
using std::string;

class Figure {
private:
	string label;
	double lineColor[4];
	double fillColor[4];
	double labelColor[4];

	GtkWidget* widget;

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
	virtual void draw(cairo_t* cr) const = 0;

	void setWidget(GtkWidget* widget);
    const string& getLabel() const;
    void setLabel(const string&);
    const double* getLineColor() const;
    void setLineColor(double r, double g, double b, double a = 1.0);
    const double* getFillColor() const;
    void setFillColor(double r, double g, double b, double a = 1.0);
    const double* getLabelColor() const;
    void setLabelColor(double r, double g, double b, double a = 1.0);
};

#endif /* FIGURE_H_ */
