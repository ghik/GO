/*
 * main.cpp
 *
 *  Created on: 20-10-2011
 *      Author: ghik
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cmath>
using namespace std;

#include <algorithm>
#include <vector>
#include <queue>

#include "RBTree.h"

struct Point;
struct Segment;

enum PointType {
	END, BEG, ISECT, FREE
};

double isection_x(const Segment& seg1, const Segment& seg2);
double isection_y(const Segment& seg1, const Segment& seg2);

struct Point {
	struct ComparePtr {
		bool operator()(Point* p1, Point* p2);
	};

	Point() :
			x(0), y(0), seg(NULL), type(FREE) {
	}
	Point(double _x, double _y) :
			x(_x), y(_y), seg(NULL), type(FREE) {
	}
	bool operator==(const Point& rhs) {
		return x == rhs.x && y == rhs.y && seg == rhs.seg && type == rhs.type;
	}
	double x;
	double y;
	Segment* seg;
	PointType type;
};

struct Segment {
	struct ComparePtr {
		int operator()(Segment* s1, Segment* s2);
	};

	Segment() {
		beg.seg = end.seg = this;
		beg.type = BEG;
		end.type = END;
	}
	Segment(const Point& _beg, const Point& _end) :
			beg(_beg), end(_end) {
		beg.seg = end.seg = this;
		norm();
	}
	Segment(const Segment& rhs) {
		beg = rhs.beg;
		end = rhs.end;
		beg.seg = end.seg = this;
	}
	void norm() {
		if (beg.x > end.x) {
			swap(beg, end);
			beg.type = BEG;
			end.type = END;
		}
	}
	double getSweepY() const;
	Point beg;
	Point end;
	RBTree<Segment*, Segment::ComparePtr>::PNode it;

	int treein;
	int treeout;
};

typedef RBTree<Segment*, Segment::ComparePtr> SegmentTree;

struct Intersection {
	Intersection() :
			s1(NULL), s2(NULL) {
	}
	Intersection(Segment* _s1, Segment* _s2) :
			s1(_s1), s2(_s2) {
		isection.x = isection_x(*s1, *s2);
		isection.y = isection_y(*s1, *s2);
	}
	bool operator==(const Intersection& rhs) {
		return (s1 == rhs.s1 && s2 == rhs.s2) || (s1 == rhs.s2 && s2 == rhs.s1);
	}
	bool operator<(const Intersection& rhs) {
		return isection.x < rhs.isection.x;
	}
	Segment* s1;
	Segment* s2;
	Point isection;
};

int n;
vector<Segment> segs;
vector<Intersection> isections;
double sweep = 0;
Segment *isectSeg1 = NULL, *isectSeg2 = NULL;

ofstream fout;
int frame = 0;

bool Point::ComparePtr::operator()(Point* p1, Point* p2) {
	if (p1->x == p2->x) {
		return p1->type < p2->type;
	}
	return p1->x > p2->x;
}

double Segment::getSweepY() const {
	if (sweep == beg.x) {
		return beg.y;
	}
	if (sweep == end.x) {
		return end.y;
	}
	return beg.y + (sweep - beg.x) / (end.x - beg.x) * (end.y - beg.y);
}

double product(const Point& p1, const Point& p2, const Point& p3) {
	return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

double product(const Segment& s1, const Segment& s2) {
	return (s1.end.x - s1.beg.x) * (s2.end.y - s2.beg.y) - (s1.end.y - s1.beg.y) * (s2.end.x - s2.beg.x);
}

bool intersect(const Segment& seg1, const Segment& seg2) {
	if (product(seg1.beg, seg2.beg, seg1.end)
			* product(seg1.beg, seg1.end, seg2.end) < 0) {
		return false;
	}
	if (product(seg2.beg, seg1.beg, seg2.end)
			* product(seg2.beg, seg2.end, seg1.end) < 0) {
		return false;
	}

	return true;
}

double isection_x(const Segment& seg1, const Segment& seg2) {
	return seg1.beg.x
			+ (seg1.end.x - seg1.beg.x)
					* ((seg2.end.x - seg2.beg.x) * (seg1.beg.y - seg2.beg.y)
							- (seg2.end.y - seg2.beg.y)
									* (seg1.beg.x - seg2.beg.x))
					/ ((seg2.end.y - seg2.beg.y) * (seg1.end.x - seg1.beg.x)
							- (seg2.end.x - seg2.beg.x)
									* (seg1.end.y - seg1.beg.y));
}

double isection_y(const Segment& seg1, const Segment& seg2) {
	return seg1.beg.y
			+ (seg1.end.y - seg1.beg.y)
					* ((seg2.end.x - seg2.beg.x) * (seg1.beg.y - seg2.beg.y)
							- (seg2.end.y - seg2.beg.y)
									* (seg1.beg.x - seg2.beg.x))
					/ ((seg2.end.y - seg2.beg.y) * (seg1.end.x - seg1.beg.x)
							- (seg2.end.x - seg2.beg.x)
									* (seg1.end.y - seg1.beg.y));
}

ostream& operator<<(ostream& str, const Segment& s);
int Segment::ComparePtr::operator()(Segment* s1, Segment* s2) {
	if (s1 == s2) {
		return 0;
	}
	if ((isectSeg1 == s1 && isectSeg2 == s2)
			|| (isectSeg1 == s2 && isectSeg2 == s1)) {
		//cout << "Comparing " << *s1 << ' ' << *s2 << endl;
		//cout << "IsecResult " << product(*s1, *s2) << ' ' << (product(*s1, *s2) > 0 ? -1 : 1) << endl;
		return product(*s1, *s2) > 0 ? -1 : 1;
	} else {
		double sy1 = s1->getSweepY();
		double sy2 = s2->getSweepY();
		return sy1 < sy2 ? -1 : 1;
	}
}

ostream& operator<<(ostream& str, const Point& p) {
	return str << '(' << p.x << ',' << p.y << ')';
}

ostream& operator<<(ostream& str, const Segment& s) {
	return str << s.beg << '-' << s.end;
}

ostream& operator<<(ostream& str, const Intersection& i) {
	return str << *i.s1 << ' ' << *i.s2 << ' ' << i.isection;
}

ostream& operator<<(ostream& str, PointType pt) {
	switch (pt) {
	case BEG:
		return str << "BEG";
	case END:
		return str << "END";
	case FREE:
		return str << "FREE";
	case ISECT:
		return str << "ISECT";
	}
	return str;
}

struct SimpleSegmentCompare {
	bool operator()(Segment* s1, Segment* s2) {
		if (s1->beg.y == s2->beg.y) {
			return s1 < s2;
		}
		return s1->beg.y < s2->beg.y;
	}
};

Point* intersection_event(Segment& seg1, Segment& seg2) {
	if (intersect(seg1, seg2)) {
		double isx = isection_x(seg1, seg2);
		if (isx >= sweep) {
			double isy = isection_y(seg1, seg2);
			Point* isection = new Point(isx, isy);
			isection->seg = &seg1;
			isection->type = ISECT;

			fout << "frames " << frame << " -1" << endl;
			fout << "lineColor 0.2 0.6 0.1 1" << endl;
			fout << "fillColor 0.2 0.6 0.1 1" << endl;
			fout << "dot " << isx << ' ' << isy << " 2" << endl;

			return isection;
		}
	}
	return NULL;
}

void dump_scene(const char* filename, SegmentTree& T, int scene) {
	ofstream fout;
	if(scene == 0) {
		fout.open(filename);
	} else {
		fout.open(filename, ios_base::app);
	}

	fout << "frames " << scene << ' ' << scene << endl;
	fout << "lineColor 1 0 0 1" << endl;
	fout << "line " << sweep << ' ' << 0 << ' ' << 90 << endl;
	for (vector<Segment>::iterator it = segs.begin(); it != segs.end(); it++) {
		if (T.find(&*it)->valid()) {
			fout << "lineColor 0 0 1 1" << endl;
		} else {
			fout << "lineColor 0 0 0 1" << endl;
		}
		fout << "segment " << it->beg.x << ' ' << it->beg.y << ' ' << it->end.x
				<< ' ' << it->end.y << endl;
	}
	fout << "lineColor 0 0 0 1" << endl;
	fout.close();
}

int main(int argc, char** argv) {
	const char* file = "raw_segments.txt";

	ifstream fin(file);
	fin >> n;
	for (int i = 0; i < n; i++) {
		segs.push_back(Segment());
		Segment& seg = segs.back();
		fin >> seg.beg.x >> seg.beg.y >> seg.end.x >> seg.end.y;
		seg.norm();
	}
	fin.close();

	fout.open("../geomtest/segments.txt");

	priority_queue<Point*, vector<Point*>, Point::ComparePtr> Q;
	SegmentTree T;

	for (int i = 0; i < n; i++) {
		Q.push(&segs[i].beg);
		Q.push(&segs[i].end);
	}

	sweep = Q.top()->x;
	while (!Q.empty()) {
		Point* event = Q.top();
		Q.pop();

		isectSeg1 = NULL;
		isectSeg2 = NULL;

		SegmentTree::PNode it, pit, nit, nnit;
		switch (event->type) {
		case BEG:
			sweep = event->x;
			event->seg->treein = frame;
			it = event->seg->it = T.create(event->seg)->insert();
			pit = it->predecessor();
			nit = it->successor();

			if (pit->valid()) {
				Point* isection = intersection_event(*pit->value(),
						*it->value());
				if (isection != NULL) {
					Q.push(isection);
				}
			}
			if (nit->valid()) {
				Point* isection = intersection_event(*it->value(),
						*nit->value());
				if (isection != NULL) {
					Q.push(isection);
				}
			}
			break;
		case END:
			event->seg->treeout = frame;
			it = event->seg->it;
			pit = it->predecessor();
			nit = it->successor();
			T.remove(it);

			sweep = event->x;
			if (pit->valid() && nit->valid()) {
				Point* isection = intersection_event(*pit->value(),
						*nit->value());
				if (isection != NULL) {
					Q.push(isection);
				}
			}
			break;
		case ISECT: {
			it = event->seg->it;
			nit = it->successor();
			Segment *seg1 = it->value(), *seg2 = nit->value();

			isections.push_back(Intersection(seg1, seg2));

			T.remove(it);
			T.remove(nit);

			isectSeg1 = seg1;
			isectSeg2 = seg2;
			sweep = event->x;

			nit = seg1->it = T.create(seg1)->insert();
			it = seg2->it = T.create(seg2)->insert();
			pit = it->predecessor();

			if (pit->valid()) {
				Point* isection = intersection_event(*pit->value(),
						*it->value());
				if (isection != NULL) {
					Q.push(isection);
				}
			}
			nnit = nit->successor();
			if (nnit->valid()) {
				Point* isection = intersection_event(*nit->value(),
						*nnit->value());
				if (isection != NULL) {
					Q.push(isection);
				}
			}

			while (*Q.top() == *event) {
				delete Q.top();
				Q.pop();
			}
			delete event;
			break;
		}
		default:
			break;
		}

		fout << "frames " << frame << ' ' << frame << endl;
		fout << "lineColor 1 0 0 1" << endl;
		fout << "line " << sweep << " 0 90" << endl;

		frame++;
	}

	for(vector<Segment>::iterator it=segs.begin();it!=segs.end();it++) {
		fout << "frames 0 " << it->treein-1 << endl;
		fout << "lineColor 0 0 0 1" << endl;
		fout << "segment " << it->beg.x << ' ' << it->beg.y << ' ' << it->end.x << ' ' << it->end.y << endl;

		fout << "frames " << it->treein << ' ' << it->treeout-1 << endl;
		fout << "lineColor 0 0 1 1" << endl;
		fout << "segment " << it->beg.x << ' ' << it->beg.y << ' ' << it->end.x << ' ' << it->end.y << endl;

		fout << "frames " << it->treeout << ' ' << frame << endl;
		fout << "lineColor 0 0 0 1" << endl;
		fout << "segment " << it->beg.x << ' ' << it->beg.y << ' ' << it->end.x << ' ' << it->end.y << endl;
	}
	fout.close();

	return 0;
}
