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
#include "Circle.h"

vector<Figure*>* figures = NULL;
vector<Dot*> dots;
Dot* dragged = NULL;

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
			double x, y, size;
			ss >> x >> y;
			Dot* dot = new Dot(x, y);
			dot->setLineColor(lineColor);
			dot->setFillColor(fillColor);
			dot->setLabelColor(labelColor);
			if (!ss.eof()) {
				ss >> size;
				cout << size << endl;
				dot->setSize(size);
			}
			if (!ss.eof()) {
				ss >> label;
				dot->setLabel(label);
			}
			res->push_back(dot);
			dots.push_back(dot);
		} else if (cmd == "line") {
			double x, y, angle;
			ss >> x >> y >> angle;
			Line* line = new Line(x, y, rad(angle));
			line->setLineColor(lineColor);
			line->setFillColor(fillColor);
			line->setLabelColor(labelColor);
			if (!ss.eof()) {
				ss >> label;
				line->setLabel(label);
			}
			res->push_back(line);
		} else if (cmd == "hline") {
			double x, y, angle;
			ss >> x >> y >> angle;
			HalfLine* hline = new HalfLine(x, y, rad(angle));
			hline->setLineColor(lineColor);
			hline->setFillColor(fillColor);
			hline->setLabelColor(labelColor);
			if (!ss.eof()) {
				ss >> label;
				hline->setLabel(label);
			}
			res->push_back(hline);
		} else if (cmd == "segment") {
			double x1, y1, x2, y2;
			ss >> x1 >> y1 >> x2 >> y2;
			Segment* seg = new Segment(x1, y1, x2, y2);
			seg->setLineColor(lineColor);
			seg->setFillColor(fillColor);
			seg->setLabelColor(labelColor);
			if (!ss.eof()) {
				ss >> label;
				seg->setLabel(label);
			}
			res->push_back(seg);
		} else if (cmd == "polygon") {
			vector<double> points;
			while (!ss.eof()) {
				double x, y;
				ss >> x >> y;
				points.push_back(x);
				points.push_back(y);
			}
			Polygon* polygon = new Polygon(points);
			polygon->setLineColor(lineColor);
			polygon->setFillColor(fillColor);
			polygon->setLabelColor(labelColor);
			res->push_back(polygon);
		} else if (cmd == "circle") {
			double cx, cy, r;
			ss >> cx >> cy >> r;
			Circle* circle = new Circle(cx, cy, r);
			circle->setLineColor(lineColor);
			circle->setFillColor(fillColor);
			circle->setLabelColor(labelColor);
			if (!ss.eof()) {
				ss >> label;
				circle->setLabel(label);
			}
			res->push_back(circle);
		}
	}

	for (vector<Figure*>::iterator it = res->begin(); it != res->end(); it++) {
		cout << **it;
	}

	return res;
}

void paint(GtkWidget* widget) {
	cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(widget));

	draw_grid(widget, cr, 100);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, LINE_WIDTH);
	cairo_select_font_face(cr, FONT_FACE, CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, FONT_SIZE);

	/*
	 Line line(100, 200, rad(30));
	 line.setLabel("jakas linia");
	 line.draw(cr);

	 HalfLine hline(300, 400, rad(-70));
	 hline.setLabel("jakas półlinia");
	 hline.draw(cr);

	 Segment seg(50, 100, 400, 200);
	 seg.setLabel("odcineq");
	 seg.draw(cr);


	 double pol[] = { 50, 50, 200, 100, 500, 30, 700, 500, 100, 530 };
	 Polygon polygon((double*) pol, 5);
	 polygon.draw(cr);

	 draw_line(cr, 100, 200, rad(30), "jakas linia");
	 draw_halfline(cr, 300, 400, rad(-80), "do góry");
	 draw_segment(cr, 50, 100, 400, 200, "odcineq");
	 draw_circle(cr, 500, 300, 30, "kół");

	 double polygon[][2] = { { 50, 50 }, { 200, 100 }, { 500, 30 }, { 700, 500 },
	 { 100, 530 } };
	 draw_polygon(cr, polygon, 5);
	 */

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
	if (event->button.button != 1) {
		return true;
	}
	for (vector<Dot*>::iterator it = dots.begin(); it != dots.end(); it++) {
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

bool motion_notify(GtkWidget* widget, GdkEvent* event, gpointer data) {
	GtkLabel* label = (GtkLabel*)data;
	ostringstream ss;
	ss << "(" << event->motion.x << ", " << event->motion.y << ")";
	gtk_label_set_text(label, ss.str().c_str());
	return true;
}

int main(int argc, char *argv[]) {
	GtkWidget *window, *vbox, *area, *label;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_move(GTK_WINDOW(window), 100, 50);

	vbox = gtk_vbox_new(false, 5);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(vbox), area);

	gtk_drawing_area_size(GTK_DRAWING_AREA(area), 800, 600);

	label = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(vbox), label);

	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(area, "expose-event", G_CALLBACK(expose), NULL);
	g_signal_connect(area, "button-press-event", G_CALLBACK(button_press),
			NULL);
	g_signal_connect(area, "button-release-event", G_CALLBACK(button_release),
			NULL);
	g_signal_connect(area, "motion-notify-event", G_CALLBACK(motion_notify), (gpointer)label);

	gtk_widget_show_all(window);

	gdk_window_set_events(gtk_widget_get_window(area), GDK_ALL_EVENTS_MASK);

	figures = parseFigures("figures.txt");

	/*
	 srand(time(0));
	 for (int i = 0; i < 100; i++) {
	 Dot* const dot = new Dot(rand() % 800, rand() % 600);
	 ostringstream ss;
	 ss << i;
	 dot->setLabel(ss.str());
	 dot->setFillColor(0, 0, 1, 1);
	 dots.insert(dot);
	 }*/

	gtk_main();

	return 0;
}

