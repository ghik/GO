/*
 * main.c
 *
 *  Created on: 10-10-2011
 *      Author: rjanusz
 */

#include "predicates.c"
#include <stdio.h>

int main(int argc, char** argv) {
	int i;
	double pts[100000][2];

	FILE *f, *f2;

	f = fopen("raw3.txt", "r");
	for (i = 0; i < 10000; i++) {
		fscanf(f, "%lf %lf", &pts[i][0], &pts[i][1]);
	}
	fclose(f);

	double v[4];
	v[0] = -1.0;
	v[1] = 0.0;
	v[2] = 1.0;
	v[3] = 0.1;

	f2 = fopen("../../GO/geomtest/fast3.txt", "w");
	for (i = 0; i < 10000; i++) {
		double q = orient2dfast(v, v + 2, &pts[i]);
		if (q < 0) {
			fprintf(f2, "lineColor 1 0 0 1\n");
		} else if (q > 0) {
			fprintf(f2, "lineColor 0 1 0 1\n");
		} else {
			fprintf(f2, "lineColor 0 0 1 1\n");
		}
		fprintf(f2, "dot %lf %lf 0.02\n", pts[i][0], pts[i][1]);
	}
	fclose(f2);

	f2 = fopen("../../GO/geomtest/exact3.txt", "w");
	for (i = 0; i < 10000; i++) {
		double q = orient2dexact(v, v + 2, &pts[i]);
		if (q < 0) {
			fprintf(f2, "lineColor 1 0 0 1\n");
		} else if (q > 0) {
			fprintf(f2, "lineColor 0 1 0 1\n");
		} else {
			fprintf(f2, "lineColor 0 0 1 1\n");
		}
		fprintf(f2, "dot %lf %lf 0.02\n", pts[i][0], pts[i][1]);
	}
	fclose(f2);

	f2 = fopen("../../GO/geomtest/slow3.txt", "w");
	for (i = 0; i < 10000; i++) {
		double q = orient2dslow(v, v + 2, &pts[i]);
		if (q < 0) {
			fprintf(f2, "lineColor 1 0 0 1\n");
		} else if (q > 0) {
			fprintf(f2, "lineColor 0 1 0 1\n");
		} else {
			fprintf(f2, "lineColor 0 0 1 1\n");
		}
		fprintf(f2, "dot %lf %lf 0.02\n", pts[i][0], pts[i][1]);
	}
	fclose(f2);

	return 0;
}
