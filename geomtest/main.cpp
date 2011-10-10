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
vector<Draggable*> draggables;
Draggable* dragged = NULL;

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
	for (vector<Draggable*>::iterator it = draggables.begin(); it != draggables.end(); it++) {
		Draggable* draggable = *it;
		if (draggable->drags(event->button.x, event->button.y)) {
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
	dragged->draggedTo(event->button.x, event->button.y);
	gtk_widget_queue_draw(widget);
	dragged = NULL;
	return true;
}

bool motion_notify(GtkWidget* widget, GdkEvent* event, gpointer data) {
	static long lastRepaintTime = 0;
	GtkLabel* label = (GtkLabel*) data;
	ostringstream ss;
	ss << "(" << event->motion.x << ", " << event->motion.y << ")";
	gtk_label_set_text(label, ss.str().c_str());

	long nowTime = now();
	if(dragged != NULL && nowTime-lastRepaintTime > REPAINT_INTERVAL) {
		lastRepaintTime = nowTime;
		dragged->draggedTo(event->motion.x, event->motion.y);
		gtk_widget_queue_draw(widget);
	}
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
	g_signal_connect(area, "motion-notify-event", G_CALLBACK(motion_notify),
			(gpointer)label);

	gtk_widget_show_all(window);

	gdk_window_set_events(gtk_widget_get_window(area), GDK_ALL_EVENTS_MASK);

	figures = parseFigures("figures.txt");

	gtk_main();

	return 0;
}

