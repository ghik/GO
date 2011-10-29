/*
 * figure.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Figure.h"
#include <gtk/gtk.h>
#include <cairo/cairo.h>

#include <iostream>
using namespace std;

Figure::Figure() :
		begframe(0), endframe(INT_MAX), widget(NULL) {
	setColor(lineColor, 0, 0, 0, 1);
	setColor(labelColor, 0, 0, 0, 1);
	setColor(fillColor, 1, 1, 1, 0);
}

Figure::~Figure() {
}

void Figure::applyLineColor(cairo_t *cr) const {
	cairo_set_source_rgba(cr, lineColor[0], lineColor[1], lineColor[2],
			lineColor[3]);
}

void Figure::applyFillColor(cairo_t *cr) const {
	cairo_set_source_rgba(cr, fillColor[0], fillColor[1], fillColor[2],
			fillColor[3]);
}

void Figure::beginDraw(cairo_t *cr, double x, double y, double angle) const {
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_rotate(cr, angle);
	cairo_move_to(cr, 0, 0);
}

void Figure::putLabel(cairo_t *cr) const {
	if (label.length() > 0) {
		cairo_set_source_rgba(cr, labelColor[0], labelColor[1], labelColor[2],
				labelColor[3]);

		cairo_save(cr);
		cairo_scale(cr, 1/zoom, -1/zoom);
		cairo_rel_move_to(cr, LABEL_OFFSET[0], LABEL_OFFSET[1]);
		cairo_show_text(cr, label.c_str());
		cairo_rel_move_to(cr, -LABEL_OFFSET[0], -LABEL_OFFSET[1]);
		cairo_restore(cr);
	}
}

void Figure::endDraw(cairo_t *cr) const {
	cairo_stroke(cr);
	cairo_restore(cr);
}

void Figure::repaint() const {
	if (widget != NULL) {
		gtk_widget_queue_draw(widget);
	}
}

void Figure::setWidget(GtkWidget *widget) {
	this->widget = widget;
}

ostream& operator<<(ostream& str, const Figure& figure) {
	figure.serialize(str);
	return str;
}
