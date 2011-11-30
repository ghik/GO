/*
 * commons.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include <cstring>
#include <sys/time.h>
#include <iostream>
#include <sstream>
using namespace std;

double rad(double deg) {
	return deg * M_PI / 180.0L;
}

double deg(double rad) {
	return rad * 180.0L / M_PI;
}

double dist(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double faraway(double x, double y) {
	int w, h;
	get_drawingarea_size(&w, &h);
	return fabs(x+centerx)+fabs(x+centery)+2*max(w,h)/zoom;
}

long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000L + tv.tv_usec / 1000;
}

void screen_to_std(double* x, double* y) {
	int width, height;
	get_drawingarea_size(&width, &height);
	*x = *x - width / 2.0;
	*y = height / 2.0 - *y;
}

void screen_to_view(double* x, double* y) {
	cairo_matrix_transform_point(&revViewMatrix, x, y);
}

void get_text_size(cairo_t* cr, const char* text, double* width, double* height) {
	cairo_text_extents_t textExtents;
	cairo_text_extents(cr, text, &textExtents);
	*width = textExtents.width;
	*height = textExtents.height;
}

void setColor(double* color, double r, double g, double b, double a) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

void setColor(double* target, const double* color) {
	memcpy(target, color, 4 * sizeof(double));
}

void draw_grid(cairo_t* cr) {
	cairo_save(cr);
	cairo_set_line_width(cr, 1 / zoom);
	cairo_select_font_face(cr, GRID_FONT_FACE, CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, GRID_FONT_SIZE);

    double minx, maxx, miny, maxy;
    cairo_clip_extents(cr, &minx, &miny, &maxx, &maxy);

	double width = maxx - minx, interval = 1;

	if (width / interval > GRID_SEGMENTS) {
		while (width / interval > GRID_SEGMENTS) {
			interval *= 10;
		}
	} else {
		while (10 * width / interval < GRID_SEGMENTS) {
			interval /= 10;
		}
	}

	double pos;

	pos = ((int) (minx / interval)) * interval;
	while (pos < maxx) {
		if (pos == 0) {
			cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
		} else {
			cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
		}
		cairo_move_to(cr, pos, maxy);
		cairo_line_to(cr, pos, miny);
		cairo_stroke(cr);

		cairo_save(cr);
		cairo_move_to(cr, pos, miny);
		cairo_scale(cr, 1 / zoom, -1 / zoom);
		cairo_rel_move_to(cr, GRID_LABEL_OFFSET[0], GRID_LABEL_OFFSET[1]);
		cairo_set_source_rgb(cr, 0, 0, 0);

		stringstream ss;
		ss << pos;
		cairo_show_text(cr, ss.str().c_str());

		cairo_rel_move_to(cr, -GRID_LABEL_OFFSET[0], -GRID_LABEL_OFFSET[1]);
		cairo_restore(cr);

		pos += interval;
	}

	pos = ((int) (miny / interval)) * interval;
	while (pos < maxy) {
		if (pos == 0) {
			cairo_set_source_rgb(cr, 0, 0, 0);
		} else {
			cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
		}
		cairo_move_to(cr, maxx, pos);
		cairo_line_to(cr, minx, pos);
		cairo_stroke(cr);

		cairo_save(cr);
		cairo_move_to(cr, minx, pos);
		cairo_scale(cr, 1 / zoom, -1 / zoom);
		cairo_rel_move_to(cr, GRID_LABEL_OFFSET[0], GRID_LABEL_OFFSET[1]);
		cairo_set_source_rgb(cr, 0, 0, 0);

		stringstream ss;
		ss << pos;
		cairo_show_text(cr, ss.str().c_str());

		cairo_rel_move_to(cr, -GRID_LABEL_OFFSET[0], -GRID_LABEL_OFFSET[1]);
		cairo_stroke(cr);
		cairo_restore(cr);

		pos += interval;
	}

	cairo_restore(cr);
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

	cairo_move_to(cr, -faraway(x, y), 0);
	cairo_line_to(cr, faraway(x, y), 0);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_halfline(cairo_t* cr, double x, double y, double angle,
		const char* label) {
	start_draw(cr, x, y, angle);

	cairo_line_to(cr, faraway(x, y), 0);
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
