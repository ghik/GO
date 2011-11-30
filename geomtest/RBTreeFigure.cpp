/*
 * RBTreeFigure.cpp
 *
 *  Created on: 30-11-2011
 *      Author: ghik
 */

#include "RBTreeFigure.h"
#include <map>

const double NODEWIDTH = 50;

RBTreeFigure::RBTreeFigure(Tree* _tree):
		tree(_tree), topx(0), topy(0) {
}

RBTreeFigure::RBTreeFigure(Tree* _tree, double _topx, double _topy):
		tree(_tree), topx(_topx), topy(_topy) {
}

RBTreeFigure::~RBTreeFigure() {
	delete tree;
}

void RBTreeFigure::registerDraggables(std::vector<Draggable*> & draggables) {
}

void RBTreeFigure::draw(cairo_t *cr) const {
	if(tree == NULL || tree->size() == 0) {
		return;
	}
	std::map<Tree::PNode, double> widths;
	computeNodeWidths(tree->getroot(), widths);

	beginDraw(cr, topx, topy, 0);

	applyLineColor(cr);

	endDraw(cr);
}

double RBTreeFigure::computeNodeWidths(Tree::PNode pnode, std::map<Tree::PNode, double>& widths) const {
	double leftw = NODEWIDTH/2, rightw = NODEWIDTH/2;
	Tree::PNode left = pnode->getleft(), right = pnode->getright();
	if(left != Tree::NIL) {
		leftw = computeNodeWidths(left, widths);
	}
	if(right != Tree::NIL) {
		rightw = computeNodeWidths(right, widths);
	}
	widths[pnode] = leftw+rightw;
	return leftw+rightw;
}

ostream & RBTreeFigure::serialize(ostream & str) const {
	return str << "rbtree " << *tree << ' ' << topx << ' ' << topy << endl;
}

ostream & RBTreeFigure::raw_serialize(ostream & ostr) const {
	return ostr << *tree << ' ' << topx << ' ' << topy << endl;
}

