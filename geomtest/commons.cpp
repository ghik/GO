/*
 * commons.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include <cmath>

double rad(double deg) {
	return deg * M_PI / 180.0L;
}

double dist(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void setColor(double* color, double r, double g, double b, double a) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

void draw_grid(GtkWidget* widget, cairo_t* cr, int prec) {
	GtkAllocation allocation;
	gtk_widget_get_allocation(widget, &allocation);

	cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
	cairo_set_line_width(cr, 0.5);

	for (gint h = 0; h < allocation.height; h += prec) {
		cairo_move_to(cr, 0, h);
		cairo_line_to(cr, allocation.width - 1, h);
	}

	for (gint w = 0; w < allocation.width; w += prec) {
		cairo_move_to(cr, w, 0);
		cairo_line_to(cr, w, allocation.height - 1);
	}

	cairo_stroke(cr);
}

void start_draw(cairo_t* cr, double x, double y, double angle) {
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_rotate(cr, angle);
	cairo_move_to(cr, 0, 0);
}

void end_draw(cairo_t *cr) {
	cairo_stroke(cr);
	cairo_restore(cr);
}

void put_label(cairo_t *cr, const char* label) {
	if (label != NULL) {
		cairo_rel_move_to(cr, LABEL_OFFSET[0], LABEL_OFFSET[1]);
		cairo_show_text(cr, label);
		cairo_rel_move_to(cr, -LABEL_OFFSET[0], -LABEL_OFFSET[1]);
	}
}

void draw_point(cairo_t* cr, double x, double y, const char* label) {
	start_draw(cr, x, y, 0);

	cairo_arc(cr, 0, 0, POINT_SIZE, 0, rad(360));
	cairo_fill(cr);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_line(cairo_t* cr, double x, double y, double angle,
		const char* label) {
	start_draw(cr, x, y, angle);

	cairo_move_to(cr, -FARAWAY, 0);
	cairo_line_to(cr, FARAWAY, 0);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_halfline(cairo_t* cr, double x, double y, double angle,
		const char* label) {
	start_draw(cr, x, y, angle);

	cairo_line_to(cr, FARAWAY, 0);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_segment(cairo_t* cr, double x1, double y1, double x2, double y2,
		const char* label) {
	start_draw(cr, 0, 0, 0);

	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);

	cairo_move_to(cr, x1, y1);

	cairo_rotate(cr, atan((y2 - y1) / (x2 - x1)));
	put_label(cr, label);
	end_draw(cr);
}

void draw_circle(cairo_t* cr, double cx, double cy, double r,
		const char* label) {
	start_draw(cr, cx, cy, 0);

	cairo_move_to(cr, r, 0);
	cairo_arc(cr, 0, 0, r, rad(0), rad(360));
	cairo_move_to(cr, 0, -r);

	put_label(cr, label);
	end_draw(cr);
}

void draw_polyline(cairo_t* cr, double points[][2], int num_points) {
	start_draw(cr, 0, 0, 0);

	cairo_move_to(cr, points[0][0], points[0][1]);
	for (int i = 1; i < num_points; i++) {
		cairo_line_to(cr, points[i][0], points[i][1]);
	}

	end_draw(cr);
}

void draw_polygon(cairo_t* cr, double points[][2], int num_points) {
	start_draw(cr, 0, 0, 0);

	cairo_move_to(cr, points[0][0], points[0][1]);
	for (int i = 1; i < num_points; i++) {
		cairo_line_to(cr, points[i][0], points[i][1]);
	}
	cairo_line_to(cr, points[0][0], points[0][1]);

	end_draw(cr);
}
