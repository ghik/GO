/*
 * commons.h
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#include <cstdlib>
#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <ctime>
#include <cmath>

const int SCREEN_SIZE = 800;

const double LINE_WIDTH = 1;
const double POINT_SIZE = 3;
const double DRAG_RANGE = 5;

const double FONT_SIZE = 14;
const char* const FONT_FACE = "Arial";
const double LABEL_OFFSET[2] = { 5, -5 };

const double GRID_FONT_SIZE = 10;
const char* const GRID_FONT_FACE = "Arial";
const double GRID_LABEL_OFFSET[2] = { 2, -2 };

const time_t REPAINT_INTERVAL = 100;
const double BASE_ZOOM_FACTOR = 1.2;
const double SCREEN_MOVE_AMOUNT = 25;
const int GRID_SEGMENTS = 20;

const int KEY_ARROW_LEFT = 65361;
const int KEY_ARROW_UP = 65362;
const int KEY_ARROW_RIGHT = 65363;
const int KEY_ARROW_DOWN = 65364;

extern double zoom, centerx, centery;
extern cairo_matrix_t revViewMatrix;
extern double minx, miny, maxx, maxy;

double rad(double deg);
double deg(double rad);
double dist(double x1, double y1, double x2, double y2);
double faraway(double x, double y);

long now();

void screen_to_std(double* x, double* y);
void screen_to_view(double* x, double* y);

void setColor(double* color, double r, double g, double b, double a);
void setColor(double* target, const double* color);
void draw_grid(cairo_t* cr);
void start_draw(cairo_t* cr, double x, double y, double angle);
void end_draw(cairo_t *cr);
void put_label(cairo_t *cr, const char* label);
void draw_point(cairo_t* cr, double x, double y, const char* label = NULL);
void draw_line(cairo_t* cr, double x, double y, double angle,
		const char* label = NULL);
void draw_halfline(cairo_t* cr, double x, double y, double angle,
		const char* label = NULL);
void draw_segment(cairo_t* cr, double x1, double y1, double x2, double y2,
		const char* label = NULL);
void draw_circle(cairo_t* cr, double cx, double cy, double r,
		const char* label = NULL);
void draw_polyline(cairo_t* cr, double points[][2], int num_points);
void draw_polygon(cairo_t* cr, double points[][2], int num_points);

#endif /* COMMONS_H_ */
