/*
 * main.cpp
 *
 *  Created on: 20-10-2011
 *      Author: ghik
 */

#include <iostream>
#include <fstream>

#include <algorithm>
#include <vector>
#include <list>
using namespace std;

enum Side {
	NONE, LEFT, RIGHT
};

struct Point {
	Point() :
			x(0), y(0), side(NONE) {
	}
	Point(double _x, double _y) :
			x(_x), y(_y), side(NONE) {
	}
	bool operator==(const Point& rhs) {
		return x == rhs.x && y == rhs.y;
	}
	bool operator<(const Point& rhs) {
		if(y == rhs.y) {
			return x < rhs.x;
		}
		return y < rhs.y;
	}
	bool operator>(const Point& rhs) {
		if(y == rhs.y) {
			return x > rhs.x;
		}
		return y > rhs.y;
	}
	double x;
	double y;
	Side side;
};

ostream& operator<<(ostream& str, const Point& p) {
	return str << '(' << p.x << ',' << p.y << ')';
}

istream& operator>>(istream& str, Point& p) {
	return str >> p.x >> p.y;
}

int n;
vector<Point> polygon;

double product(const Point& p1, const Point& p2, const Point& p3) {
	return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

bool is_y_monotonous(vector<Point>& polygon) {
	bool minfound = false, maxfound = false;
	for(vector<Point>::iterator it=polygon.begin();it!=polygon.end();it++) {
		Point& prev = it != polygon.begin() ? *(it-1) : *(polygon.end()-1);
		Point& next = (it+1) != polygon.end() ? *(it+1) : *polygon.begin();
		if(prev > *it && next > *it) {
			if(minfound) {
				return false;
			}
			minfound = true;
		}
		if(prev < *it && next < *it) {
			if(maxfound) {
				return false;
			}
			maxfound = true;
		}
	}
	return true;
}

vector<Point>::iterator prev(vector<Point>::iterator it) {
	return it != polygon.begin() ? it-1 : polygon.end()-1;
}

vector<Point>::iterator next(vector<Point>::iterator it) {
	return (it+1) != polygon.end() ? it+1 : polygon.begin();
}

bool in_polygon(Point& p1, Point& p2, Point& p3) {
	double prod = product(p1, p2, p3);
	if(p1.side == LEFT) {
		return prod < 0;
	} else {
		return prod > 0;
	}
}

int main(int argc, char** argv) {
	const char* file = "dziub.txt";

	ofstream fout("../geomtest/dziub_res.txt");

	ifstream fin(file);
	fin >> n;
	for (int i = 0; i < n; i++) {
		polygon.push_back(Point());
		fin >> polygon.back();
	}
	fin.close();

	fout << "frames 0 -1" << endl;
	fout << "polygon ";
	for(vector<Point>::iterator it=polygon.begin();it!=polygon.end();it++) {
		fout << it->x << ' ' << it->y << ' ';
	}
	fout << endl;

	if(!is_y_monotonous(polygon)) {
		cout << "Not y-monotonous polygon!" << endl;
		return 0;
	}

	vector<Point>::iterator it, topit, maxit, minit, lit, rit;

	maxit = polygon.begin();
	for(vector<Point>::iterator it=polygon.begin();it!=polygon.end();it++) {
		if(*it > *maxit) {
			maxit = it;
		}
	}

	minit = polygon.begin();
	for(it=polygon.begin();it!=polygon.end();it++) {
		if(*it < *minit) {
			minit = it;
		}
	}

	list<vector<Point>::iterator> stack;
	lit = rit = maxit;

	it = lit;
	while(it != minit) {
		it = next(it);
		it->side = LEFT;
	}

	it = rit;
	while(it != minit) {
		it = prev(it);
		it->side = RIGHT;
	}

	minit->side = maxit->side = NONE;

	stack.push_back(maxit);
	if(*next(maxit) > *prev(maxit)) {
		lit = next(maxit);
		stack.push_back(lit);
	} else {
		rit = prev(maxit);
		stack.push_back(rit);
	}

	int frame = 0;
	list<vector<Point>::iterator>::iterator sit;
	while(true) {
		frame++;
		fout << "frames " << frame << ' ' << frame << endl;
		fout << "lineColor 1 0 0 1" << endl << "fillColor 1 0 0 1" << endl;
		for(sit=stack.begin();sit!=stack.end();sit++) {
			fout << "dot " << (*sit)->x << ' ' << (*sit)->y << " 2" << endl;
		}

		topit = stack.back();
		if(*next(lit) > *prev(rit)) {
			it = lit = next(lit);
		} else {
			it = rit = prev(rit);
		}

		fout << "lineColor 0 1 0 1" << endl << "fillColor 0 1 0 1" << endl;
		fout << "dot " << it->x << ' ' << it->y << " 2" << endl;

		fout << "lineColor 0 0 1 1" << endl;
		if(it == minit) {
			break;
		}
		if(it->side != stack.back()->side) {
			stack.erase(stack.begin());
			for(sit=stack.begin();sit!=stack.end();sit++) {
				fout << "frames " << frame << " -1" << endl;
				fout << "segment " << (*sit)->x << ' ' << (*sit)->y << ' ' << it->x << ' ' << it->y << endl;
			}
			stack.clear();
			stack.push_back(topit);
			stack.push_back(it);
		} else {
			sit = stack.end();
			sit--; sit--;
			while(in_polygon(*it, *topit, **sit)) {
				fout << "frames " << frame << " -1" << endl;
				fout << "segment " << (*sit)->x << ' ' << (*sit)->y << ' ' << it->x << ' ' << it->y << endl;
				stack.pop_back();
				if((*sit)->side != it->side) {
					break;
				}
				topit = stack.back();
				sit--;
			}
			stack.push_back(it);
		}
	}

	fout.close();

	return 0;
}
