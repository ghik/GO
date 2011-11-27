#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
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
#include "Parabola.h"

GtkWidget *window = NULL, *drawingArea = NULL, *posLabel = NULL;

GStaticMutex mutex = G_STATIC_MUTEX_INIT;

vector<Figure*>* volatile figures = NULL;
vector<Draggable*> draggables;

Draggable* dragged = NULL;
Polygon* paintedPolygon = NULL;
bool draggingScreen = false;
double dragScreenBegx, dragScreenBegy;

double zoom = 1;
double centerx = 0;
double centery = 0;
cairo_matrix_t revViewMatrix;
double minx, miny, maxx, maxy;

int frame = 0;
int maxframe = 0;
long lastRepaintTime = 0;

void update_view_params(double _zoom, double _cx, double _cy) {
	zoom = _zoom;
	centerx = _cx;
	centery = _cy;
	cairo_matrix_init_identity(&revViewMatrix);
	cairo_matrix_scale(&revViewMatrix, 1 / zoom, -1 / zoom);
	cairo_matrix_translate(&revViewMatrix, -SCREEN_SIZE / 2.0 - centerx,
			-SCREEN_SIZE / 2.0 + centery);
}

Figure* parseFigure(const string& fig, istringstream& str) {
	if (fig == "dot") {
		Dot* dot = new Dot;
		str >> dot->x >> dot->y;
		if (!str.eof()) {
			str >> dot->size;
		}
		if (!str.eof()) {
			str >> dot->label;
		}
		return dot;
	} else if (fig == "line") {
		double angle;
		Line* line = new Line;
		str >> line->begx >> line->begy >> angle;
		line->dirAngle = rad(angle);
		if (!str.eof()) {
			str >> line->label;
		}
		return line;
	} else if (fig == "hline") {
		double angle;
		HalfLine* hline = new HalfLine();
		str >> hline->begx >> hline->begy >> angle;
		hline->dirAngle = rad(angle);
		if (!str.eof()) {
			str >> hline->label;
		}
		return hline;
	} else if (fig == "segment") {
		Segment* seg = new Segment();
		str >> seg->x1 >> seg->y1 >> seg->x2 >> seg->y2;
		if (!str.eof()) {
			str >> seg->label;
		}
		return seg;
	} else if (fig == "polygon") {
		vector<double> points;
		while (!str.eof()) {
			double x, y;
			str >> x >> y;
			points.push_back(x);
			points.push_back(y);
		}
		Polygon* polygon = new Polygon(points);
		return polygon;
	} else if (fig == "polyline") {
		vector<double> points;
		while (!str.eof()) {
			double x, y;
			str >> x >> y;
			points.push_back(x);
			points.push_back(y);
		}
		Polyline* polyline = new Polyline(points);
		return polyline;
	} else if (fig == "circle") {
		Circle* circle = new Circle;
		str >> circle->cx >> circle->cy >> circle->r;
		if (!str.eof()) {
			str >> circle->label;
		}
		return circle;
	} else if (fig == "parabola") {
		Parabola* parabola = new Parabola;
		str >> parabola->xc >> parabola->yc >> parabola->a >> parabola->y1 >> parabola->y2;
		if(!str.eof()) {
			str >> parabola->label;
		}
		return parabola;
	}
	return NULL;
}

vector<Figure*>* parseFigures(const char* filename) {
	vector<Figure*>* res = new vector<Figure*>;
	ifstream fin(filename);
	if(!fin.is_open()) {
		return NULL;
	}
	double lineColor[4] = { 0, 0, 0, 1 };
	double fillColor[4] = { 1, 1, 1, 0 };
	double labelColor[4] = { 0, 0, 0, 1 };
	char buf[1000];
	string cmd, label;
	int begframe = 0, endframe = INT_MAX;
	while (!fin.eof()) {
		fin.getline(buf, 1000);

		if(strlen(buf) == 0) {
			continue;
		}

		istringstream ss(buf);
		ss >> cmd;
		if (cmd == "lineColor") {
			ss >> lineColor[0] >> lineColor[1] >> lineColor[2] >> lineColor[3];
		} else if (cmd == "fillColor") {
			ss >> fillColor[0] >> fillColor[1] >> fillColor[2] >> fillColor[3];
		} else if (cmd == "labelColor") {
			ss >> labelColor[0] >> labelColor[1] >> labelColor[2]
					>> labelColor[3];
		} else if (cmd == "frames") {
			ss >> begframe >> endframe;
			if (endframe > maxframe) {
				maxframe = endframe;
			}
			if (begframe > maxframe) {
				maxframe = begframe;
			}
		} else {
			Figure* fig = parseFigure(cmd, ss);
			if(fig != NULL) {
				setColor(fig->lineColor, lineColor);
				setColor(fig->fillColor, fillColor);
				setColor(fig->labelColor, labelColor);
				fig->begframe = begframe;
				fig->endframe = endframe;
				res->push_back(fig);
				fig->registerDraggables(draggables);
			}
		}
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

void paint(cairo_t* cr) {
	g_static_mutex_lock(&mutex);

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
		if (frame >= (*it)->begframe
				&& ((*it)->endframe < 0 || frame <= (*it)->endframe)) {
			(*it)->draw(cr);
		}
	}

	g_static_mutex_unlock(&mutex);
}

bool expose(GtkWidget* widget, GdkEvent* event, gpointer data) {
	cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(widget));
	paint(cr);
	cairo_destroy(cr);
	return true;
}

bool button_press(GtkWidget* widget, GdkEvent* event, gpointer data) {
	double x = event->button.x, y = event->button.y;
	screen_to_view(&x, &y);

	double sx = event->button.x, sy = event->button.y;
	screen_to_std(&sx, &sy);

	switch (event->button.button) {
	case 1:
		if (paintedPolygon == NULL) {
			draggingScreen = true;
			dragScreenBegx = sx;
			dragScreenBegy = sy;
		}
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
		if (paintedPolygon != NULL) {
			paintedPolygon->addPoint(x, y);
		} else {
			draggingScreen = false;
			update_view_params(zoom, centerx + (sx - dragScreenBegx),
					centery + (sy - dragScreenBegy));
		}
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
	double x = event->button.x, y = event->button.y;
	screen_to_view(&x, &y);

	double sx = event->button.x, sy = event->button.y;
	screen_to_std(&sx, &sy);

	update_position_label(x, y);

	long nowTime = now();
	if (dragged != NULL) {
		dragged->draggedTo(x, y);
		if (nowTime - lastRepaintTime > REPAINT_INTERVAL) {
			gtk_widget_queue_draw(drawingArea);
			lastRepaintTime = nowTime;
		}
	}
	if (draggingScreen) {
		update_view_params(zoom, centerx + (sx - dragScreenBegx),
				centery + (sy - dragScreenBegy));
		dragScreenBegx = sx;
		dragScreenBegy = sy;
		if (nowTime - lastRepaintTime > REPAINT_INTERVAL) {
			gtk_widget_queue_draw(drawingArea);
			lastRepaintTime = nowTime;
		}
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
	bool updatePositionLabel = false;

	switch (event->key.keyval) {
	case KEY_ARROW_LEFT:
		update_view_params(zoom, centerx - SCREEN_MOVE_AMOUNT, centery);
		updatePositionLabel = true;
		break;
	case KEY_ARROW_UP:
		update_view_params(zoom, centerx, centery + SCREEN_MOVE_AMOUNT);
		updatePositionLabel = true;
		break;
	case KEY_ARROW_RIGHT:
		update_view_params(zoom, centerx + SCREEN_MOVE_AMOUNT, centery);
		updatePositionLabel = true;
		break;
	case KEY_ARROW_DOWN:
		update_view_params(zoom, centerx, centery - SCREEN_MOVE_AMOUNT);
		updatePositionLabel = true;
		break;
	case KEY_ESC:
	    gtk_main_quit();
	    break;
	case ' ':
		frame = (frame + 1) % (maxframe + 1);
		break;
	case KEY_BACKSPACE:
		frame = (maxframe + frame) % (maxframe + 1);
		break;
	case 'p':
	case 'P':
		if (paintedPolygon == NULL) {
			paintedPolygon = new Polygon();
			figures->push_back(paintedPolygon);
		} else {
			paintedPolygon->registerDraggables(draggables);
			paintedPolygon = NULL;
		}
		break;
	default:
		cout << "Key " << event->key.keyval << endl;
		break;
	}

	if (updatePositionLabel) {
		double x, y;
		current_position(&x, &y);
		screen_to_view(&x, &y);
		update_position_label(x, y);
	}

	gtk_widget_queue_draw(drawingArea);
	return true;
}

void free_figures() {
	for (vector<Figure*>::iterator it = figures->begin(); it != figures->end();
			it++) {
		delete *it;
	}
	delete figures;
}

gpointer shell(gpointer data) {
	char buf[10000];
	while(!cin.eof()) {
		cout.put('>');
		cin.getline(buf, 10000);

		if(strlen(buf) == 0) {
			continue;
		}

		string cmd;
		istringstream ss(buf);
		ss >> cmd;

		if(cmd == "save") {
			string filename;
			ss >> filename;

			ofstream fout(filename.c_str());
			for(vector<Figure*>::iterator it=figures->begin();it!=figures->end();it++) {
				(*it)->serialize(fout);
			}
			fout.close();
			cout << "Saved to " << filename << endl;
		} else if(cmd == "saveraw") {
			string filename;
			ss >> filename;

			ofstream fout(filename.c_str());
			for(vector<Figure*>::iterator it=figures->begin();it!=figures->end();it++) {
				(*it)->raw_serialize(fout);
			}
			fout.close();
			cout << "Saved to " << filename << endl;
		} else if(cmd == "savemat") {
			string filename;
			ss >> filename;

			ofstream fout(filename.c_str());
			for(vector<Figure*>::iterator it=figures->begin();it!=figures->end();it++) {
				(*it)->mat_serialize(fout);
			}
			fout.close();
			cout << "Saved to " << filename << endl;
		} else if(cmd == "saveimg") {
			string filename;
			ss >> filename;

			int width, height;
			gdk_drawable_get_size(gtk_widget_get_window(drawingArea), &width, &height);

			cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

			cairo_t* cr = cairo_create(surface);

			cairo_set_source_rgba(cr, 1, 1, 1, 1);
			cairo_move_to(cr, 0, 0);
			cairo_line_to(cr, width, 0);
			cairo_line_to(cr, width, height);
			cairo_line_to(cr, 0, height);
			cairo_line_to(cr, 0, 0);
			cairo_fill(cr);

			paint(cr);
			cairo_destroy(cr);

			cairo_surface_write_to_png(surface, filename.c_str());
			cairo_surface_destroy(surface);

			cout << "Saved to " << filename << endl;
		} else if(cmd == "load") {
			string filename;
			ss >> filename;

			g_static_mutex_lock(&mutex);
			draggables.clear();
			free_figures();
			figures = parseFigures(filename.c_str());
			g_static_mutex_unlock(&mutex);
			gtk_widget_queue_draw(drawingArea);
			cout << "Loaded figures from " << filename << endl;
		} else if(cmd == "clear") {
			g_static_mutex_lock(&mutex);
			draggables.clear();
			free_figures();
			figures = new vector<Figure*>;
			g_static_mutex_unlock(&mutex);
			gtk_widget_queue_draw(drawingArea);
		} else if(cmd == "exit") {
			gtk_main_quit();
		} else {
			cout << "Unknown command: " << cmd << endl;
		}
	}
	return NULL;
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

	if(argc > 1) {
		figures = parseFigures(argv[1]);
	}
	if(figures == NULL) {
		figures = new vector<Figure*>;
	}
	update_view_params(zoom, centerx, centery);

	g_thread_create(shell, NULL, false, NULL);

	gtk_main();

	free_figures();

	return 0;
}

