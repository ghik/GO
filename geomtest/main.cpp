#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <set>
using namespace std;

#define FARAWAY 2000

const double LINE_WIDTH = 1;
const double POINT_SIZE = 3;
const double FONT_SIZE = 14;
const char* FONT_FACE = "URW Chancery L";
const double LABEL_OFFSET[2] = { 5, -5 };

double rad(double deg) {
	return deg * M_PI / 180.0L;
}

double dist(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void start_draw(cairo_t* cr, double x, double y, double angle) {
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_rotate(cr, angle);
	cairo_move_to(cr, 0, 0);
}

void end_draw(cairo_t* cr) {
	cairo_stroke(cr);
	cairo_restore(cr);
}

void put_label(cairo_t* cr, const char* label) {
	if (label != NULL) {
		cairo_rel_move_to(cr, LABEL_OFFSET[0], LABEL_OFFSET[1]);
		cairo_show_text(cr, label);
		cairo_rel_move_to(cr, -LABEL_OFFSET[0], -LABEL_OFFSET[1]);
	}
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

void draw_point(cairo_t* cr, double x, double y, const char* label = NULL) {
	start_draw(cr, x, y, 0);

	cairo_arc(cr, 0, 0, POINT_SIZE, 0, rad(360));
	cairo_fill(cr);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_line(cairo_t* cr, double x, double y, double angle,
		const char* label = NULL) {
	start_draw(cr, x, y, angle);

	cairo_move_to(cr, -FARAWAY, 0);
	cairo_line_to(cr, FARAWAY, 0);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_halfline(cairo_t* cr, double x, double y, double angle,
		const char* label = NULL) {
	start_draw(cr, x, y, angle);

	cairo_line_to(cr, FARAWAY, 0);
	cairo_move_to(cr, 0, 0);

	put_label(cr, label);
	end_draw(cr);
}

void draw_segment(cairo_t* cr, double x1, double y1, double x2, double y2,
		const char* label = NULL) {
	start_draw(cr, 0, 0, 0);

	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);

	cairo_move_to(cr, x1, y1);

	cairo_rotate(cr, atan((y2 - y1) / (x2 - x1)));
	put_label(cr, label);
	end_draw(cr);
}

void draw_circle(cairo_t* cr, double cx, double cy, double r,
		const char* label = NULL) {
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

class Figure {
public:
	Figure();
	virtual ~Figure() {
	}

protected:
	double lineColor[4];
	double fillColor[4];
	string label;

public:
	virtual void draw(cairo_t* cr) const = 0;
    const string& getLabel() const;

    void setLabel(const string&);
};

Figure::Figure() {
}

const string& Figure::getLabel() const
{
    return label;
}

void Figure::setLabel(const string& label) {
	this->label = label;
}

class Dot : public Figure {
private:
	double x;
	double y;
	double size;
public:
	Dot(double _x, double _y) :
			x(_x), y(_y), size(POINT_SIZE) {
	}

	virtual ~Dot() {
	}

	virtual void draw(cairo_t* cr) const;
	bool inside(double x, double y) const;

	double getX() const;
	void setX(double x);
	double getY() const;
	void setY(double y);
};

void Dot::draw(cairo_t* cr) const {
	draw_point(cr, x, y, label.c_str());
}

bool Dot::inside(double x, double y) const {
	return dist(this->x, this->y, x, y) <= size;
}

double Dot::getX() const {
	return x;
}

void Dot::setX(double x) {
	this->x = x;
}

double Dot::getY() const {
	return y;
}

void Dot::setY(double y) {
	this->y = y;
}

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
	for (set<Dot*>::iterator it = dots.begin(); it != dots.end(); it++) {
		if ((*it)->inside(event->button.x, event->button.y)) {
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
	g_signal_connect(area, "button-press-event", G_CALLBACK(button_press), NULL);
	g_signal_connect(area, "button-release-event", G_CALLBACK(button_release), NULL);

	gtk_widget_show(area);
	gtk_widget_show(window);

	gdk_window_set_events(gtk_widget_get_window(area), GDK_ALL_EVENTS_MASK);

	srand(time(0));
	for(int i=0;i<100;i++) {
		Dot* dot = new Dot(rand()%800, rand()%600);
		ostringstream ss;
		ss << i;
		dot->setLabel(ss.str());
		dots.insert(dot);
	}

	gtk_main();

	return 0;
}


