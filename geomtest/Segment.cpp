/*
 * Segment.cpp
 *
 *  Created on: 09-10-2011
 *      Author: ghik
 */

#include "commons.h"
#include "Segment.h"
#include <cmath>

Segment::Segment() {
}

Segment::Segment(double _x1, double _y1, double _x2, double _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2) {
}

Segment::~Segment() {
}

void Segment::draw(cairo_t* cr) const {
	beginDraw(cr, 0, 0, 0);

	applyLineColor(cr);
	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);

	cairo_move_to(cr, x1, y1);
	cairo_rotate(cr, atan((y2 - y1) / (x2 - x1)));

	putLabel(cr);
	cairo_stroke(cr);

	endDraw(cr);
}

ostream& Segment::serialize(ostream& str) const {
	str << "segment " << x1 << " " << y1 << " " << x2 << " " << y2 << ' '
			<< label << endl;
	return str;
}

void Segment::registerDraggables(vector<Draggable*> & draggables) {
	draggables.push_back(new SegmentEnd(this, true));
	draggables.push_back(new SegmentEnd(this, false));
}

SegmentEnd::SegmentEnd(Segment *_seg, bool _beg) :
		seg(_seg), beg(_beg) {
}

SegmentEnd::~SegmentEnd() {
}

bool SegmentEnd::drags(double x, double y) {
	double myx = beg ? seg->x1 : seg->x2;
	double myy = beg ? seg->y1 : seg->y2;
	return dist(myx, myy, x, y) <= DRAG_RANGE;
}

void SegmentEnd::draggedTo(double x, double y) {
	if(beg) {
		seg->x1 = x;
		seg->y1 = y;
	} else {
		seg->x2 = x;
		seg->y2 = y;
	}
}

