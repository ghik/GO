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

Figure::Figure() :
		widget(NULL) {
	setLineColor(0, 0, 0, 1);
	setLabelColor(0, 0, 0, 1);
	setFillColor(1, 1, 1, 0);
}

Figure::~Figure() {
}

void Figure::applyLineColor(cairo_t *cr) const {
	cairo_set_source_rgba(cr, lineColor[0], lineColor[1], lineColor[2], lineColor[3]);
}

void Figure::applyFillColor(cairo_t *cr) const {
	cairo_set_source_rgba(cr, fillColor[0], fillColor[1], fillColor[2], fillColor[3]);
}

void Figure::beginDraw(cairo_t *cr, double x, double y, double angle) const {
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_rotate(cr, angle);
	cairo_move_to(cr, 0, 0);
}

void Figure::putLabel(cairo_t *cr) const {
	if (label.length() > 0) {
		cairo_set_source_rgba(cr, labelColor[0], labelColor[1], labelColor[2], labelColor[3]);
		cairo_rel_move_to(cr, LABEL_OFFSET[0], LABEL_OFFSET[1]);
		cairo_show_text(cr, label.c_str());
		cairo_rel_move_to(cr, -LABEL_OFFSET[0], -LABEL_OFFSET[1]);
	}
}

void Figure::endDraw(cairo_t *cr) const {
	cairo_stroke(cr);
	cairo_restore(cr);
}

const string& Figure::getLabel() const {
	return label;
}

void Figure::setLabel(const string& label) {
	this->label = label;
	repaint();
}

const double* Figure::getLineColor() const {
	return lineColor;
}

void Figure::setLineColor(double r, double g, double b, double a) {
	setColor(lineColor, r, g, b, a);
}

const double* Figure::getFillColor() const {
	return fillColor;
}

void Figure::setFillColor(double r, double g, double b, double a) {
	setColor(fillColor, r, g, b, a);
}

const double* Figure::getLabelColor() const {
	return labelColor;
}

void Figure::setLabelColor(double r, double g, double b, double a) {
	setColor(labelColor, r, g, b, a);
}

void Figure::repaint() const {
	if (widget != NULL) {
		gtk_widget_queue_draw(widget);
	}
}

void Figure::setWidget(GtkWidget *widget) {
	this->widget = widget;
}

