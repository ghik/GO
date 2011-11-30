/*
 * RBTreeFigure.cpp
 *
 *  Created on: 30-11-2011
 *      Author: ghik
 */

#include "RBTreeFigure.h"
#include <sstream>
#include <map>

const double NODE_WIDTH = 30;
const double LEVEL_HEIGHT = 50;

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

	beginDraw(cr, topx, topy, 0);

	std::map<Tree::PNode, double> widths;
	widths[Tree::NIL] = NODE_WIDTH/2;
	computeNodeWidths(tree->getroot(), widths);
	drawSubtree(cr, tree->getroot(), widths);

	applyLineColor(cr);

	endDraw(cr);
}

void RBTreeFigure::drawSubtree(cairo_t* cr, Tree::PNode pnode, std::map<Tree::PNode, double>& widths) const {
	ostringstream ss;
	ss << pnode->value();

	string strval = ss.str();

	double curx, cury;
	cairo_get_current_point(cr, &curx, &cury);

	double textw, texth;
	get_text_size(cr, strval.c_str(), &textw, &texth);

	cairo_move_to(cr, curx+texth, cury-texth);
	cairo_arc(cr, curx, cury-texth, texth, 0, rad(360));

	cairo_move_to(cr, curx-textw/2, cury-3*texth/2);

	cairo_save(cr);
	cairo_scale(cr, 1, -1);
	cairo_show_text(cr, strval.c_str());
	cairo_restore(cr);

	if(pnode->getleft() != Tree::NIL) {
		cairo_move_to(cr, curx, cury-2*texth);
		cairo_line_to(cr, curx-widths[pnode->getleft()->getright()], cury-2*texth-LEVEL_HEIGHT);
		drawSubtree(cr, pnode->getleft(), widths);
	}

	if(pnode->getright() != Tree::NIL) {
		cairo_move_to(cr, curx, cury-2*texth);
		cairo_line_to(cr, curx+widths[pnode->getright()->getleft()], cury-2*texth-LEVEL_HEIGHT);
		drawSubtree(cr, pnode->getright(), widths);
	}
}

double RBTreeFigure::computeNodeWidths(Tree::PNode pnode, std::map<Tree::PNode, double>& widths) const {
	double leftw = NODE_WIDTH/2, rightw = NODE_WIDTH/2;
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

