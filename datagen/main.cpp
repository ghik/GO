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

	ofstream fout("../geomtest/tree.txt");

	RBTree<int> tree;
	for(int i=0;i<100;i++) {
		tree.insert(tree.create(rand()%100));
	}
	fout << "rbtree " << tree << endl;

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

	fout.close();

	return 0;
}
