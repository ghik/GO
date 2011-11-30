/*
 * RBTreeFigure.h
 *
 *  Created on: 30-11-2011
 *      Author: ghik
 */

#ifndef RBTREEFIGURE_H_
#define RBTREEFIGURE_H_

#include "Figure.h"
#include "RBTree.h"
#include <map>

class RBTreeFigure: public Figure {
public:
	typedef RBTree<int> Tree;

	RBTreeFigure(Tree* tree);
	RBTreeFigure(Tree* tree, double topx, double topy);
	virtual ~RBTreeFigure();

	virtual void registerDraggables(std::vector<Draggable*>& draggables);
	virtual void draw(cairo_t* cr) const;
	virtual ostream& serialize(ostream& str) const;
	virtual ostream& raw_serialize(ostream& ostr) const;

	Tree* tree;
	double topx, topy;

private:
	double computeNodeWidths(Tree::PNode pnode, std::map<Tree::PNode, double>& widths) const;
};

#endif /* RBTREEFIGURE_H_ */
