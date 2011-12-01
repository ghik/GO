/*
 * main.cpp
 *
 *  Created on: 10-10-2011
 *      Author: rjanusz
 */

#include <fstream>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "../geomtest/RBTree.h"

const double DOTSIZE = 2;

double uniform(double a, double b) {
	return a + (rand() / double(RAND_MAX)) * (b - a);
}

void rand_seg(double x1, double y1, double x2, double y2, double* x, double* y) {
	double f = uniform(0, 1);
	*x = x1 + f * (x2 - x1);
	*y = y1 + f * (y2 - y1);
}

void rand_circle(double cx, double cy, double r, double* x, double* y) {
	double fi = uniform(0, 2*M_PI);
	*x = cx + r*cos(fi);
	*y = cy + r*sin(fi);
}

void rand_area(double x1, double y1, double x2, double y2, double* x, double* y) {
	*x = uniform(x1, x2);
	*y = uniform(y1, y2);
}

int main(int argc, char** argv) {
	srand(time(0));

	ifstream fin("../geomtest/tree.txt");

	char buf[100];
	RBTree<int> tree;
	fin >> buf >> tree;
	for(RBTree<int>::PNode n=tree.first();n!=RBTree<int>::NIL;n=n->successor()) {
		cout << n->value() << ' ';
	}
	cout << endl;

	RBTree<int>::PNode lb;
	lb = tree.lower_bound(100);
	cout << lb->predecessor()->value() << ' ' << lb->value() << ' ' << lb->successor()->value() << endl;

	/*
	fout.open("../geomtest/segments.txt");
	raw.open("../lab3/raw_segments.txt");
	raw << 50 << endl;
	for (int i = 0; i < 50; i++) {
		double x1, y1, x2, y2;
		rand_area(-4000, -4000, 4000, 4000, &x1, &y1);
		rand_area(-4000, -4000, 4000, 4000, &x2, &y2);
		fout << "segment " << x1 << ' ' << y1 << ' ' << x2 << ' ' << y2 << endl;
		raw << x1 << ' ' << y1 << ' ' << x2 << ' ' << y2 << endl;
	}
	*/

	fin.close();

	return 0;
}
