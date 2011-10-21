#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
using namespace std;

#include "commons.h"
#include "Dot.h"
#include "Line.h"
#include "HalfLine.h"
#include "Segment.h"
#include "Polygon.h"
#include "Polyline.h"
#include "Circle.h"

GtkWidget *window = NULL, *drawingArea = NULL, *posLabel = NULL;
bool draggingScreen = false;
double dragScreenBegx, dragScreenBegy;

vector<Figure*>* figures = NULL;
vector<Draggable*> draggables;
Draggable* dragged = NULL;
double zoom = 1;
double centerx = 0;
double centery = 0;

cairo_matrix_t revViewMatrix;
double minx, miny, maxx, maxy;

void update_view_params(double _zoom, double _cx, double _cy) {
	zoom = _zoom;
	centerx = _cx;
	centery = _cy;
	cairo_matrix_init_identity(&revViewMatrix);
	cairo_matrix_scale(&revViewMatrix, 1 / zoom, -1 / zoom);
	cairo_matrix_translate(&revViewMatrix, -SCREEN_SIZE / 2.0 - centerx,
			-SCREEN_SIZE / 2.0 + centery);
}

vector<Figure*>* parseFigures(const char* filename) {
	vector<Figure*>* res = new vector<Figure*>;
	ifstream fin(filename);
	double lineColor[4] = { 0, 0, 0, 1 };
	double fillColor[4] = { 1, 1, 1, 0 };
	double labelColor[4] = { 0, 0, 0, 1 };
	char buf[1000];
	string cmd, label;
	while (!fin.eof()) {
		fin.getline(buf, 1000);
		istringstream ss(buf);
		ss >> cmd;
		if (cmd == "lineColor") {
			ss >> lineColor[0] >> lineColor[1] >> lineColor[2] >> lineColor[3];
		} else if (cmd == "fillColor") {
			ss >> fillColor[0] >> fillColor[1] >> fillColor[2] >> fillColor[3];
		} else if (cmd == "labelColor") {
			ss >> labelColor[0] >> labelColor[1] >> labelColor[2]
					>> labelColor[3];
		} else if (cmd == "dot") {
			Dot* dot = new Dot;
			ss >> dot->x >> dot->y;
			setColor(dot->lineColor, lineColor);
			setColor(dot->fillColor, fillColor);
			setColor(dot->labelColor, labelColor);
			if (!ss.eof()) {
				ss >> dot->size;
			}
			if (!ss.eof()) {
				ss >> dot->label;
			}
			res->push_back(dot);
			dot->registerDraggables(draggables);
		} else if (cmd == "line") {
			double angle;
			Line* line = new Line;
			ss >> line->begx >> line->begy >> angle;
			line->dirAngle = rad(angle);
			setColor(line->lineColor, lineColor);
			setColor(line->fillColor, fillColor);
			setColor(line->labelColor, labelColor);
			if (!ss.eof()) {
				ss >> line->label;
			}
			res->push_back(line);
			line->registerDraggables(draggables);
		} else if (cmd == "hline") {
			double angle;
			HalfLine* hline = new HalfLine();
			ss >> hline->begx >> hline->begy >> angle;
			hline->dirAngle = rad(angle);
			setColor(hline->lineColor, lineColor);
			setColor(hline->fillColor, fillColor);
			setColor(hline->labelColor, labelColor);
			if (!ss.eof()) {
				ss >> hline->label;
			}
			res->push_back(hline);
			hline->registerDraggables(draggables);
		} else if (cmd == "segment") {
			Segment* seg = new Segment();
			ss >> seg->x1 >> seg->y1 >> seg->x2 >> seg->y2;
			setColor(seg->lineColor, lineColor);
			setColor(seg->fillColor, fillColor);
			setColor(seg->labelColor, labelColor);
			if (!ss.eof()) {
				ss >> seg->label;
			}
			res->push_back(seg);
			seg->registerDraggables(draggables);
		} else if (cmd == "polygon") {
			vector<double> points;
			while (!ss.eof()) {
				double x, y;
				ss >> x >> y;
				points.push_back(x);
				points.push_back(y);
			}
			Polygon* polygon = new Polygon(points);
			setColor(polygon->lineColor, lineColor);
			setColor(polygon->fillColor, fillColor);
			setColor(polygon->labelColor, labelColor);
			res->push_back(polygon);
			polygon->registerDraggables(draggables);
		} else if (cmd == "polyline") {
			vector<double> points;
			while (!ss.eof()) {
				double x, y;
				ss >> x >> y;
				points.push_back(x);
				points.push_back(y);
			}
			Polyline* polyline = new Polyline(points);
			setColor(polyline->lineColor, lineColor);
			setColor(polyline->fillColor, fillColor);
			setColor(polyline->labelColor, labelColor);
			res->push_back(polyline);
			polyline->registerDraggables(draggables);
		} else if (cmd == "circle") {
			Circle* circle = new Circle;
			ss >> circle->cx >> circle->cy >> circle->r;
			setColor(circle->lineColor, lineColor);
			setColor(circle->fillColor, fillColor);
			setColor(circle->labelColor, labelColor);
			if (!ss.eof()) {
				ss >> circle->label;
			}
			res->push_back(circle);
		}
	}

	for (vector<Figure*>::iterator it = res->begin(); it != res->end(); it++) {
		cout << **it;
	}

	return res;
}

void update_position_label(double x, double y) {
	ostringstream ss;
	ss << "(" << x << ", " << y << ")";
	gtk_label_set_text(GTK_LABEL(posLabel), ss.str().c_str());
}

void current_position(double* x, double* y) {
	gint ix, iy;
	gdk_window_get_pointer(gtk_widget_get_window(drawingArea), &ix, &iy, NULL);
	*x = ix;
	*y = iy;
}

void paint(GtkWidget* widget) {
	cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(widget));

	cairo_translate(cr, SCREEN_SIZE / 2.0 + centerx,
			SCREEN_SIZE / 2.0 - centery);
	cairo_scale(cr, zoom, -zoom);

	draw_grid(cr);

	cairo_font_face_t* fontFace = cairo_toy_font_face_create(FONT_FACE,
			CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

	cairo_matrix_t fontMatrix, identMatrix;
	cairo_font_options_t* fontOptions = cairo_font_options_create();
	cairo_matrix_init_scale(&fontMatrix, 3, 3);
	cairo_matrix_init_identity(&identMatrix);
	cairo_scaled_font_t* scaledFont = cairo_scaled_font_create(fontFace,
			&fontMatrix, &identMatrix, fontOptions);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, LINE_WIDTH / fabs(zoom));
	cairo_set_scaled_font(cr, scaledFont);
	cairo_set_font_size(cr, FONT_SIZE);

	for (vector<Figure*>::iterator it = figures->begin(); it != figures->end();
			it++) {
		(*it)->draw(cr);
	}
}

bool expose(GtkWidget* widget, GdkEvent* event, gpointer data) {
	paint(widget);
	return true;
}

bool button_press(GtkWidget* widget, GdkEvent* event, gpointer data) {
	double x = event->button.x, y = event->button.y;
	screen_to_view(&x, &y);

	double sx = event->button.x, sy = event->button.y;
	screen_to_std(&sx, &sy);

	switch (event->button.button) {
	case 1:
		draggingScreen = true;
		dragScreenBegx = sx;
		dragScreenBegy = sy;
		break;
	case 3:
		for (vector<Draggable*>::iterator it = draggables.begin();
				it != draggables.end(); it++) {
			Draggable* draggable = *it;
			if (draggable->drags(x, y)) {
				dragged = *it;
				return true;
			}
		}
		dragged = NULL;
		break;
	default:
		break;
	}
	return true;
}

bool button_release(GtkWidget* widget, GdkEvent* event, gpointer data) {
	double x = event->button.x, y = event->button.y;
	screen_to_view(&x, &y);

	double sx = event->button.x, sy = event->button.y;
	screen_to_std(&sx, &sy);

	switch (event->button.button) {
	case 1:
		draggingScreen = false;
		update_view_params(zoom, centerx + (sx - dragScreenBegx),
				centery + (sy - dragScreenBegy));
		gtk_widget_queue_draw(drawingArea);
		break;
	case 3:
		if (dragged == NULL) {
			break;
		}
		dragged->draggedTo(x, y);
		gtk_widget_queue_draw(drawingArea);
		dragged = NULL;
		break;
	default:
		break;
	}
	return true;
}

bool motion_notify(GtkWidget* widget, GdkEvent* event, gpointer data) {
	static long lastRepaintTime = 0;

	double x = event->button.x, y = event->button.y;
	screen_to_view(&x, &y);

	double sx = event->button.x, sy = event->button.y;
	screen_to_std(&sx, &sy);

	update_position_label(x, y);

	long nowTime = now();
	if (dragged != NULL && nowTime - lastRepaintTime > REPAINT_INTERVAL) {
		lastRepaintTime = nowTime;
		dragged->draggedTo(x, y);
		gtk_widget_queue_draw(drawingArea);
	}
	if (draggingScreen && nowTime - lastRepaintTime > REPAINT_INTERVAL) {
		lastRepaintTime = nowTime;
		update_view_params(zoom, centerx + (sx - dragScreenBegx),
				centery + (sy - dragScreenBegy));
		dragScreenBegx = sx;
		dragScreenBegy = sy;
		gtk_widget_queue_draw(drawingArea);
	}
	return true;
}

bool scroll(GtkWidget* widget, GdkEvent* event, gpointer data) {
	double factor = 1;
	switch (event->scroll.direction) {
	case GDK_SCROLL_UP:
		factor = BASE_ZOOM_FACTOR;
		break;
	case GDK_SCROLL_DOWN:
		factor = 1 / BASE_ZOOM_FACTOR;
		break;
	default:
		break;
	}
	double x = event->scroll.x, y = event->scroll.y;
	screen_to_std(&x, &y);
	update_view_params(zoom * factor, x + factor * (centerx - x),
			y + factor * (centery - y));
	gtk_widget_queue_draw(drawingArea);

	return true;
}

bool key_press(GtkWidget* widget, GdkEvent* event, gpointer data) {
	switch (event->key.keyval) {
	case KEY_ARROW_LEFT:
		update_view_params(zoom, centerx - SCREEN_MOVE_AMOUNT, centery);
		break;
	case KEY_ARROW_UP:
		update_view_params(zoom, centerx, centery + SCREEN_MOVE_AMOUNT);
		break;
	case KEY_ARROW_RIGHT:
		update_view_params(zoom, centerx + SCREEN_MOVE_AMOUNT, centery);
		break;
	case KEY_ARROW_DOWN:
		update_view_params(zoom, centerx, centery - SCREEN_MOVE_AMOUNT);
		break;
	default:
		break;
	}

	double x, y;
	current_position(&x, &y);
	screen_to_view(&x, &y);
	update_position_label(x, y);

	gtk_widget_queue_draw(drawingArea);
	return true;
}

int main(int argc, char *argv[]) {
	GtkWidget *vbox;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_move(GTK_WINDOW(window), 100, 50);

	vbox = gtk_vbox_new(false, 5);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	drawingArea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(drawingArea));

	gtk_drawing_area_size(GTK_DRAWING_AREA(drawingArea), SCREEN_SIZE,
			SCREEN_SIZE);

	posLabel = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(posLabel));

	g_signal_connect(GTK_WIDGET(window), "delete-event",
			G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(drawingArea, "expose-event", G_CALLBACK(expose), NULL);
	g_signal_connect(drawingArea, "button-press-event",
			G_CALLBACK(button_press), NULL);
	g_signal_connect(drawingArea, "button-release-event",
			G_CALLBACK(button_release), NULL);
	g_signal_connect(drawingArea, "motion-notify-event",
			G_CALLBACK(motion_notify), NULL);
	g_signal_connect(drawingArea, "scroll-event", G_CALLBACK(scroll), NULL);
	g_signal_connect(window, "key-press-event", G_CALLBACK(key_press), NULL);

	gtk_widget_show_all(GTK_WIDGET(window));

	gdk_window_set_events(gtk_widget_get_window(drawingArea),
			GDK_ALL_EVENTS_MASK);

	figures = parseFigures("figures.txt");
	update_view_params(zoom, centerx, centery);

	gtk_main();

	return 0;
}

