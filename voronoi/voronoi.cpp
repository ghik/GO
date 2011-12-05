#include "voronoi.h"

// Bounding box coordinates.
double X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

ostream* pfout = NULL;
int voronoi_frame = 0;

vector<seg*> output;
arc *root = 0;

priority_queue<point,  vector<point>,  gt> points; // site events
priority_queue<event*, vector<event*>, gt> events; // circle events

void voronoi(vector<point>& pts, ostream& fout)
{
   pfout = &fout;
   *pfout << "frames 0 -1\n";
   *pfout << "fillColor 0 0 0 1\n";
   // Read points from input.
   point p;
   for(unsigned int i=0;i<pts.size();i++) {
      p = pts[i];
      points.push(p);

      *pfout << "dot " << p.x << ' ' << p.y << ' ' << 2 << endl;

      // Keep track of bounding box size.
      if (p.x < X0) X0 = p.x;
      if (p.y < Y0) Y0 = p.y;
      if (p.x > X1) X1 = p.x;
      if (p.y > Y1) Y1 = p.y;
   }
   // Add 20% margins to the bounding box.
   double dx = (X1-X0+1)/5.0; double dy = (Y1-Y0+1)/5.0;
   X0 -= dx; X1 += dx; Y0 -= dy; Y1 += dy;

   // Process the queues; select the top element with smaller x coordinate.
   while (!points.empty())
      if (!events.empty() && events.top()->x <= points.top().x)
         process_event();
      else
         process_point();

   // After all points are processed, do the remaining circle events.
   while (!events.empty())
      process_event();

   finish_edges(); // Clean up dangling edges.
   //print_output(); // Output the voronoi diagram.
}

double clip(double val, double lb, double ub) {
	if(val < lb) {
		return lb;
	}
	if(val > ub) {
		return ub;
	}
	return val;
}

void fout_paraboles(double x) {
	*pfout << "frames " << voronoi_frame << ' ' << voronoi_frame << endl;
	for(arc* a = root; a != NULL; a = a->next) {
		double y0 = Y0, y1 = Y1;
		point p0, p1;
		if(a->prev != NULL) {
			p0 = intersection(a->prev->p, a->p, x);
			if(a->s0 != NULL) {
				*pfout << "lineColor 0 0 0 1\n";
				*pfout << "segment " << a->s0->start.x << ' ' << a->s0->start.y << ' ' << p0.x << ' ' << p0.y << endl;
			}
			y0 = clip(p0.y, Y0, Y1);
		}
		if(a->next != NULL) {
			p1 = intersection(a->p, a->next->p, x);
			y1 = clip(p1.y, Y0, Y1);
		}
		*pfout << "lineColor 0 0 1 1\n";
		double d = x-a->p.x;
		if(d != 0) {
			double pa = -1/(2*d);
			*pfout << "parabola " << a->p.x+d/2 << ' ' << a->p.y << ' ' << pa << ' ' << y0 << ' ' << y1 << endl;
		} else {
			*pfout << "segment " << a->p.x << ' ' << a->p.y << ' ' << p0.x << ' ' << p0.y << endl;
		}
	}
}

void process_point()
{
   // Get the next point from the queue.
   point p = points.top();
   points.pop();

   voronoi_frame++;

   *pfout << "frames " << voronoi_frame << ' ' << voronoi_frame << endl;
   *pfout << "lineColor 1 0 0 1\n";
   *pfout << "line " << p.x << " 0 90\n";

   // Add a new arc to the parabolic front.
   front_insert(p);

   fout_paraboles(p.x);
}

void process_event()
{
   // Get the next event from the queue.
   event *e = events.top();
   events.pop();

   voronoi_frame++;

   *pfout << "frames " << voronoi_frame << ' ' << voronoi_frame << endl;
   *pfout << "lineColor 1 0 0 1\n";
   *pfout << "line " << e->x << " 0 90\n";

   if (e->valid) {
      // Start a new edge.
      seg *s = new seg(e->p);

      // Remove the associated arc from the front.
      arc *a = e->a;
      if (a->prev) {
         a->prev->next = a->next;
         a->prev->s1 = s;
      }
      if (a->next) {
         a->next->prev = a->prev;
         a->next->s0 = s;
      }

      // Finish the edges before and after a.
      if (a->s0) a->s0->finish(e->p);
      if (a->s1) a->s1->finish(e->p);

      // Recheck circle events on either side of p:
      if (a->prev) check_circle_event(a->prev, e->x);
      if (a->next) check_circle_event(a->next, e->x);
   }

   fout_paraboles(e->x);

   delete e;
}

void front_insert(point p)
{
   if (!root) {
      root = new arc(p);
      return;
   }

   // Find the current arc(s) at height p.y (if there are any).
   for (arc *i = root; i; i = i->next) {
      point z, zz;
      if (intersect(p,i,&z)) {
         // New parabola intersects arc i.  If necessary, duplicate i.
         if (i->next && !intersect(p,i->next, &zz)) {
            i->next->prev = new arc(i->p,i,i->next);
            i->next = i->next->prev;
         }
         else i->next = new arc(i->p,i);
         i->next->s1 = i->s1;

         // Add p between i and i->next.
         i->next->prev = new arc(p,i,i->next);
         i->next = i->next->prev;

         i = i->next; // Now i points to the new arc.

         // Add new half-edges connected to i's endpoints.
         i->prev->s1 = i->s0 = new seg(z);
         i->next->s0 = i->s1 = new seg(z);

         // Check for new circle events around the new arc:
         check_circle_event(i, p.x);
         check_circle_event(i->prev, p.x);
         check_circle_event(i->next, p.x);

         return;
      }
   }

   // Special case: If p never intersects an arc, append it to the list.
   arc *i;
   for (i = root; i->next; i=i->next) ; // Find the last node.

   i->next = new arc(p,i);  
   // Insert segment between p and i
   point start;
   start.x = X0;
   start.y = (i->next->p.y + i->p.y) / 2;
   i->s1 = i->next->s0 = new seg(start);
}

// Look for a new circle event for arc i.
void check_circle_event(arc *i, double x0)
{
   // Invalidate any old event.
   if (i->e && i->e->x != x0)
      i->e->valid = false;
   i->e = NULL;

   if (!i->prev || !i->next)
      return;

   double x;
   point o;

   if (circle(i->prev->p, i->p, i->next->p, &x,&o) && x > x0) {
      // Create new event.
      i->e = new event(x, o, i);
      events.push(i->e);
      *pfout << "frames " << voronoi_frame << " -1\n";
      *pfout << "lineColor 1 0 0 1" << endl << "fillColor 1 0 0 1" << endl;
      *pfout << "dot " << x << ' ' << o.y << ' ' << 1 << endl;
      *pfout << "frames " << voronoi_frame << ' ' << voronoi_frame << endl;
      *pfout << "lineColor 0 1 0 1" << endl << "fillColor 1 1 1 0" << endl;
      *pfout << "circle " << o.x << ' ' << o.y << ' ' << (x-o.x) << endl;
   }
}

// Find the rightmost point on the circle through a,b,c.
bool circle(point a, point b, point c, double *x, point *o)
{
   // Check that bc is a "right turn" from ab.
   if ((b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) > 0)
      return false;

   // Algorithm from O'Rourke 2ed p. 189.
   double A = b.x - a.x,  B = b.y - a.y,
          C = c.x - a.x,  D = c.y - a.y,
          E = A*(a.x+b.x) + B*(a.y+b.y),
          F = C*(a.x+c.x) + D*(a.y+c.y),
          G = 2*(A*(c.y-b.y) - B*(c.x-b.x));

   if (G == 0) return false;  // Points are co-linear.

   // Point o is the center of the circle.
   o->x = (D*E-B*F)/G;
   o->y = (A*F-C*E)/G;

   // o.x plus radius equals max x coordinate.
   *x = o->x + sqrt( pow(a.x - o->x, 2) + pow(a.y - o->y, 2) );
   return true;
}

// Will a new parabola at point p intersect with arc i?
bool intersect(point p, arc *i, point *result)
{
   if (i->p.x == p.x) return false;

   double a,b;
   if (i->prev) // Get the intersection of i->prev, i.
      a = intersection(i->prev->p, i->p, p.x).y;
   if (i->next) // Get the intersection of i->next, i.
      b = intersection(i->p, i->next->p, p.x).y;

   if ((!i->prev || a <= p.y) && (!i->next || p.y <= b)) {
      result->y = p.y;

      result->x = (i->p.x*i->p.x + (i->p.y-result->y)*(i->p.y-result->y) - p.x*p.x)
                / (2*i->p.x - 2*p.x);

      return true;
   }
   return false;
}

// Where do two parabolas intersect?
point intersection(point p0, point p1, double l)
{
   point res, p = p0;

   double z0 = 2*(p0.x - l);
   double z1 = 2*(p1.x - l);

   if (p0.x == p1.x)
      res.y = (p0.y + p1.y) / 2;
   else if (p1.x == l)
      res.y = p1.y;
   else if (p0.x == l) {
      res.y = p0.y;
      p = p1;
   } else {
      // Use the quadratic formula.
      double a = 1/z0 - 1/z1;
      double b = -2*(p0.y/z0 - p1.y/z1);
      double c = (p0.y*p0.y + p0.x*p0.x - l*l)/z0
               - (p1.y*p1.y + p1.x*p1.x - l*l)/z1;

      res.y = ( -b - sqrt(b*b - 4*a*c) ) / (2*a);
   }
   // Plug back into one of the parabola equations.
   res.x = (p.x*p.x + (p.y-res.y)*(p.y-res.y) - l*l)/(2*p.x-2*l);
   return res;
}

void finish_edges()
{
   // Advance the sweep line so no parabolas can cross the bounding box.
   double l = X1 + (X1-X0) + (Y1-Y0);

   // Extend each remaining segment to the new parabola intersections.
   for (arc *i = root; i->next; i = i->next)
      if (i->s1)
         i->s1->finish(intersection(i->p, i->next->p, l*2));
}

void print_output()
{
   // Bounding box coordinates.
   cout << X0 << " "<< X1 << " " << Y0 << " " << Y1 << endl;

   // Each output segment in four-column format.
   vector<seg*>::iterator i;
   for (i = output.begin(); i != output.end(); i++) {
      point p0 = (*i)->start;
      point p1 = (*i)->end;
      cout << p0.x << " " << p0.y << " " << p1.x << " " << p1.y << endl;
   }
}
