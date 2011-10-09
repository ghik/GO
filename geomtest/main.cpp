#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <set>

#include "commons.h"
#include "Dot.h"
#include "Line.h"
#include "HalfLine.h"
#include "Segment.h"
#include "Polygon.h"
using namespace std;

set<Dot*> dots;
Dot* dragged = NULL;

void paint(GtkWidget* widget) {
	cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(widget));

	draw_grid(widget, cr, 100);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, LINE_WIDTH);
	cairo_select_font_face(cr, FONT_FACE, CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, FONT_SIZE);

	Line line(100, 200, rad(30));
	line.setLabel("jakas linia");
	line.draw(cr);

	HalfLine hline(300, 400, rad(-70));
	hline.setLabel("jakas półlinia");
	hline.draw(cr);

	Segment seg(50, 100, 400, 200);
	seg.setLabel("odcineq");
	seg.draw(cr);

	double pol[] = { 50, 50, 200, 100, 500, 30, 700, 500,
		 100, 530 };
	Polygon polygon((double*)pol, 5);
	polygon.draw(cr);
	/*
	 draw_line(cr, 100, 200, rad(30), "jakas linia");
	 draw_halfline(cr, 300, 400, rad(-80), "do góry");
	 draw_segment(cr, 50, 100, 400, 200, "odcineq");
	 draw_circle(cr, 500, 300, 30, "kół");

	 double polygon[][2] = { { 50, 50 }, { 200, 100 }, { 500, 30 }, { 700, 500 },
	 { 100, 530 } };
	 draw_polygon(cr, polygon, 5);
	 */

	for (set<Dot*>::iterator it = dots.begin(); it != dots.end(); it++) {
		Dot* dot = *it;
		dot->draw(cr);
	}
}

bool expose(GtkWidget* widget, GdkEvent* event, gpointer data) {
	paint(widget);
	return true;
}

bool button_press(GtkWidget* widget, GdkEvent* event, gpointer data) {
	if (event->button.button != 1) {
		return true;
	}
	for (set<Dot*>::iterator it = dots.begin(); it != dots.end(); it++) {
		Dot* dot = *it;
		if (dot->inside(event->button.x, event->button.y)) {
			dragged = *it;
			return true;
		}
	}
	dragged = NULL;
	return true;
}

bool button_release(GtkWidget* widget, GdkEvent* event, gpointer data) {
	if (event->button.button != 1 || dragged == NULL) {
		return true;
	}
	dragged->setX(event->button.x);
	dragged->setY(event->button.y);
	gtk_widget_queue_draw(widget);
	dragged = NULL;
	return true;
}

int main(int argc, char *argv[]) {
	GtkWidget *window, *area;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_move(GTK_WINDOW(window), 100, 50);

	area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), area);

	gtk_drawing_area_size(GTK_DRAWING_AREA(area), 800, 600);

	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(area, "expose-event", G_CALLBACK(expose), NULL);
	g_signal_connect(area, "button-press-event", G_CALLBACK(button_press),
			NULL);
	g_signal_connect(area, "button-release-event", G_CALLBACK(button_release),
			NULL);

	gtk_widget_show(area);
	gtk_widget_show(window);

	gdk_window_set_events(gtk_widget_get_window(area), GDK_ALL_EVENTS_MASK);

	srand(time(0));
	for (int i = 0; i < 100; i++) {
		Dot* const dot = new Dot(rand() % 800, rand() % 600);
		ostringstream ss;
		ss << i;
		dot->setLabel(ss.str());
		dot->setFillColor(0, 0, 1, 1);
		dots.insert(dot);
	}

	gtk_main();

	return 0;
}

