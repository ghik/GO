#ifndef VORONOI_H_
#define VORONOI_H_

#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <math.h>

using namespace std;

extern ostream* pfout;
extern int voronoi_frame;

// Notation for working with points

struct point {
	double x, y;
	point(): x(0), y(0) {}
	point(double _x, double _y): x(_x), y(_y) {}
};

// Arc, event, and segment datatypes
struct arc;
struct seg;

struct event {
   double x;
   point p;
   arc *a;
   bool valid;

   event(double xx, point pp, arc *aa)
      : x(xx), p(pp), a(aa), valid(true) {}
};

struct arc {
   point p;
   arc *prev, *next;
   event *e;

   seg *s0, *s1;

   arc(point pp, arc *a=0, arc *b=0)
    : p(pp), prev(a), next(b), e(0), s0(0), s1(0) {}
};

extern vector<seg*> output;  // Array of output segments.

struct seg {
   point start, end;
   bool done;

   seg(point p)
      : start(p), end(0,0), done(false)
   { output.push_back(this); }

   // Set the end point and mark as "done."
   void finish(point p) {
	   if (done)
		   return;
	   end = p;
	   *pfout << "frames " << voronoi_frame << " -1\n";
	   *pfout << "lineColor 0 0 0 1\n";
	   *pfout << "segment " << start.x << ' ' << start.y << ' ' << end.x << ' ' << end.y << endl;
	   done = true;
   }
};

extern arc *root; // First item in the parabolic front linked list.

// Function declarations
void process_point();
void process_event();
void front_insert(point  p);

bool circle(point a, point b, point c, double *x, point *o);
void check_circle_event(arc *i, double x0);

bool intersect(point p, arc *i, point *result = 0);
point intersection(point p0, point p1, double l);

void finish_edges();
void print_output();

// "Greater than" comparison, for reverse sorting in priority queue.
struct gt {
   bool operator()(point a, point b) {return a.x==b.x ? a.y>b.y : a.x>b.x;}
   bool operator()(event *a, event *b) {return a->x>b->x;}
};

void voronoi(vector<point>& pts, ostream& fout);
void fout_paraboles(double x);

#endif
