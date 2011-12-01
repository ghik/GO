/*
 * RBTree.h
 *
 *  Created on: 22-10-2011
 *      Author: ghik
 */

#ifndef RBTREE_H_
#define RBTREE_H_

#include <iostream>

template<class T>
class DefaultCompare {
public:
	int operator()(const T& val1, const T& val2) {
		if (val1 < val2) {
			return -1;
		} else if (val1 > val2) {
			return 1;
		} else {
			return 0;
		}
	}
};

template<class T, class C = DefaultCompare<T> >
class RBTree {
public:
	class Node;
	typedef Node* PNode;

	class Node {
	public:
		const T& value() {
			return val;
		}
		PNode successor() {
			return tree->successor(this);
		}
		PNode predecessor() {
			return tree->predecessor(this);
		}
		PNode insert() {
			tree->insert(this);
			return this;
		}
		bool valid() {
			return this != NIL;
		}

		/* For internal tree structure analysis */
		PNode getleft() {
			return left;
		}
		PNode getright() {
			return right;
		}
		PNode parent() {
			return p;
		}

	private:
		std::ostream& serialize(std::ostream& str);
		std::istream& deserialize(std::istream& str);

		T val;
		bool red;
		PNode left;
		PNode right;
		PNode p;

		RBTree* tree;

		friend class RBTree;
	};

	RBTree();
	~RBTree();

	PNode first();
	PNode last();
	int size();
	PNode create();
	PNode create(const T& val);
	/* Returns any node with value equal to 'val' or NIL if there is no such node */
	PNode find(const T& val);
	/* Returns first node with value greater or equal to 'val' or NIL if there is no such node */
	PNode lower_bound(const T& val);
	/* Returns last node with value less or equal to 'val' or NIL if there is no such node */
	PNode upper_bound(const T& val);
	PNode successor(PNode n);
	PNode predecessor(PNode n);
	PNode insert(const T& val);
	void insert(PNode n);
	void remove(PNode n, bool swaplinks = true);

	std::ostream& serialize(std::ostream& str);
	std::istream& deserialize(std::istream& str);

	static PNode NIL;

	/* For internal tree structure analysis */
	PNode getroot() {
		return root;
	}

private:
	PNode maxnode(PNode n);
	PNode minnode(PNode n);
	void lrotate(PNode n);
	void rrotate(PNode n);
	void insfixup(PNode n);
	void remfixup(PNode n);
	void swaplinks(PNode n1, PNode n2);
	void destroy(PNode n);

private:
	PNode root;
	int count;
	C comp;

	static Node nnil;
};

template<class T, class C>
typename RBTree<T, C>::Node RBTree<T, C>::nnil;

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::NIL = 0;

template<class T, class C>
RBTree<T, C>::RBTree() {
	if (NIL == 0) {
		nnil.red = false;
		nnil.left = nnil.right = nnil.p = &nnil;
		NIL = &nnil;
	}
	root = NIL;
	count = 0;
}

template<class T, class C>
RBTree<T, C>::~RBTree() {
	destroy(root);
}

template<class T, class C>
int RBTree<T, C>::size() {
	return count;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::first() {
	return minnode(root);
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::last() {
	return maxnode(root);
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::find(const T& val) {
	PNode n = root;
	while (n != NIL) {
		int comp_res = comp(val, n->val);
		if (comp_res < 0)
			n = n->left;
		else if (comp_res > 0)
			n = n->right;
		else
			return n;
	}
	return NIL;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::lower_bound(const T& val) {
	PNode n = root, pn;
	while (n != NIL) {
		pn = n;
		int comp_res = comp(val, n->val);
		if (comp_res <= 0)
			n = n->left;
		else
			n = n->right;
	}
	if(pn == NIL) {
		return NIL;
	}
	if(comp(val, pn->val) > 0) {
		pn = pn->successor();
	}
	return pn;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::upper_bound(const T& val) {
	PNode n = root, pn;
	while (n != NIL) {
		pn = n;
		int comp_res = comp(val, n->val);
		if (comp_res >= 0)
			n = n->right;
		else
			n = n->left;
	}
	if(pn == NIL) {
		return NIL;
	}
	if(comp(val, pn->val) < 0) {
		pn = pn->predecessor();
	}
	return pn;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::maxnode(PNode n) {
	if (n == NIL)
		return NIL;
	while (n->right != NIL)
		n = n->right;
	return n;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::minnode(PNode n) {
	if (n == NIL)
		return NIL;
	while (n->left != NIL)
		n = n->left;
	return n;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::successor(PNode n) {
	if (n->right != NIL) {
		n = n->right;
		while (n->left != NIL)
			n = n->left;
		return n;
	}
	PNode t = n->p;
	while (t != NIL && n == t->right) {
		n = t;
		t = t->p;
	}
	return t;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::predecessor(PNode n) {
	if (n->left != NIL) {
		n = n->left;
		while (n->right != NIL)
			n = n->right;
		return n;
	}
	PNode t = n->p;
	while (t != NIL && n == t->left) {
		n = t;
		t = t->p;
	}
	return t;
}

template<class T, class C>
void RBTree<T, C>::lrotate(PNode n) {
	if (n == NIL || n->right == NIL)
		return;
	if (n == root)
		root = n->right;
	n->right->p = n->p;
	if (n->p != NIL) {
		if (n == n->p->left)
			n->p->left = n->right;
		else
			n->p->right = n->right;
	}
	n->p = n->right;
	n->right = n->right->left;
	if (n->right != NIL)
		n->right->p = n;
	n->p->left = n;
}

template<class T, class C>
void RBTree<T, C>::rrotate(PNode n) {
	if (n == NIL || n->left == NIL)
		return;
	if (n == root)
		root = n->left;
	n->left->p = n->p;
	if (n->p != NIL) {
		if (n == n->p->left)
			n->p->left = n->left;
		else
			n->p->right = n->left;
	}
	n->p = n->left;
	n->left = n->left->right;
	if (n->left != NIL)
		n->left->p = n;
	n->p->right = n;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::insert(const T& value) {
	PNode res = create(value);
	insert(res);
	return res;
}

template<class T, class C>
void RBTree<T, C>::insert(PNode n) {
	count++;
	n->red = true;
	n->left = n->right = NIL;
	const T& val = n->val;
	PNode t = root, pt = NIL;
	while (t != NIL) {
		pt = t;
		if (comp(val, t->val) < 0)
			t = t->left;
		else
			t = t->right;
	}
	if (pt != NIL) {
		if (comp(val, pt->val) < 0)
			pt->left = n;
		else
			pt->right = n;
	}
	n->p = pt;
	if (root == NIL)
		root = n;
	insfixup(n);
}

template<class T, class C>
void RBTree<T, C>::insfixup(PNode n) {
	while (n->p->red)
		if (n->p == n->p->p->left) {
			if (n->p->p->right->red) {
				n->p->p->red = true;
				n->p->red = false;
				n->p->p->right->red = false;
				n = n->p->p;
			} else {
				if (n == n->p->right) {
					n = n->p;
					lrotate(n);
				}
				n->p->p->red = true;
				n->p->red = false;
				rrotate(n->p->p);
			}
		} else {
			if (n->p->p->left->red) {
				n->p->p->red = true;
				n->p->red = false;
				n->p->p->left->red = false;
				n = n->p->p;
			} else {
				if (n == n->p->left) {
					n = n->p;
					rrotate(n);
				}
				n->p->p->red = true;
				n->p->red = false;
				lrotate(n->p->p);
			}
		}
	root->red = false;
}

template<class T, class C>
void RBTree<T, C>::swaplinks(PNode n1, PNode n2) {
	if (n1 == n2 || n1 == NIL || n2 == NIL)
		return;
	if (n1 == root)
		root = n2;
	else if (n2 == root)
		root = n1;
	PNode tn;
	bool n1leftn2 = (n1 == n2->left), n1rightn2 = (n1 == n2->right);
	if (n2->p == n1) {
		tn = n1;
		n1 = n2;
		n2 = tn;
	}
	if (n1->p == n2->p) {
		tn = n1->p->left;
		n1->p->left = n1->p->right;
		n1->p->right = tn;
	} else {
		if (n1->p != NIL && !n1leftn2 && !n1rightn2) {
			if (n1 == n1->p->right)
				n1->p->right = n2;
			else
				n1->p->left = n2;
		}
		if (n2->p != NIL) {
			if (n2 == n2->p->right)
				n2->p->right = n1;
			else
				n2->p->left = n1;
		}
		tn = n1->p;
		n1->p = n2->p;
		if (!n1leftn2 && !n1rightn2)
			n2->p = tn;
		else
			n2->p = n1;
	}
	if (n1->left != NIL)
		n1->left->p = n2;
	if (n1->right != NIL)
		n1->right->p = n2;
	if (n2->left != NIL && !n1leftn2)
		n2->left->p = n1;
	if (n2->right != NIL && !n1rightn2)
		n2->right->p = n1;
	tn = n2->left;
	n2->left = n1->left;
	if (!n1leftn2)
		n1->left = tn;
	else
		n1->left = n2;
	tn = n2->right;
	n2->right = n1->right;
	if (!n1rightn2)
		n1->right = tn;
	else
		n1->right = n2;
	bool tc;
	tc = n1->red;
	n1->red = n2->red;
	n2->red = tc;
}

template<class T, class C>
void RBTree<T, C>::remove(PNode n, bool swln) {
	if (n == NIL)
		return;
	count--;
	PNode tn;
	if (n->left != NIL && n->right != NIL) {
		tn = successor(n);
		if (swln)
			swaplinks(n, tn);
		else {
			const T& tv = n->val;
			n->val = tn->val;
			tn->val = tv;
			n = tn;
		}
	}
	if (n->right != NIL)
		tn = n->right;
	else
		tn = n->left;
	if (n != root) {
		if (n == n->p->right)
			n->p->right = tn;
		else
			n->p->left = tn;
	} else
		root = tn;
	tn->p = n->p;
	if (!n->red)
		remfixup(tn);
	if (tn == NIL)
		tn->p = NIL;
	delete n;
}

template<class T, class C>
void RBTree<T, C>::remfixup(PNode tn) {
	PNode w;
	while (tn != root && !tn->red)
		if (tn == tn->p->left) {
			w = tn->p->right;
			if (w->red) {
				tn->p->red = true;
				w->red = false;
				lrotate(tn->p);
				w = tn->p->right;
			}
			if (!w->right->red && !w->left->red) {
				w->red = true;
				tn = tn->p;
			} else {
				if (!w->right->red) {
					w->red = true;
					w->left->red = false;
					rrotate(w);
					w = w->p;
				}
				w->red = tn->p->red;
				tn->p->red = false;
				w->right->red = false;
				lrotate(tn->p);
				tn = root;
			}
		} else {
			w = tn->p->left;
			if (w->red) {
				tn->p->red = true;
				w->red = false;
				rrotate(tn->p);
				w = tn->p->left;
			}
			if (!w->right->red && !w->left->red) {
				w->red = true;
				tn = tn->p;
			} else {
				if (!w->left->red) {
					w->red = true;
					w->right->red = false;
					lrotate(w);
					w = w->p;
				}
				w->red = tn->p->red;
				tn->p->red = false;
				w->left->red = false;
				rrotate(tn->p);
				tn = root;
			}
		}
	tn->red = false;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::create() {
	PNode n = new Node;
	n->tree = this;
	return n;
}

template<class T, class C>
typename RBTree<T, C>::PNode RBTree<T, C>::create(const T& val) {
	PNode n = new Node;
	n->val = val;
	n->tree = this;
	return n;
}

template<class T, class C>
void RBTree<T, C>::destroy(PNode n) {
	if (n == NIL)
		return;
	destroy(n->right);
	destroy(n->left);
	delete n;
	n = NIL;
}

template<class T, class C>
std::ostream& RBTree<T, C>::Node::serialize(std::ostream& str) {
	str << val << ' ' << red << ' ' << (left != NIL) << ' ' << (right != NIL) << ' ';
	if(left != NIL) {
		left->serialize(str);
	}
	if(right != NIL) {
		right->serialize(str);
	}
	return str;
}

template<class T, class C>
std::ostream& RBTree<T, C>::serialize(std::ostream& str) {
	str << count << ' ';
	if(root != NIL) {
		root->serialize(str);
	}
	return str;
}

template<class T, class C>
std::istream& RBTree<T, C>::Node::deserialize(std::istream& str) {
	bool isleft, isright;
	str >> val >> red >> isleft >> isright;
	if(isleft) {
		left = tree->create();
		left->p = this;
		left->deserialize(str);
	} else {
		left = NIL;
	}
	if(isright) {
		right = tree->create();
		right->p = this;
		right->deserialize(str);
	} else {
		right = NIL;
	}
	return str;
}

template<class T, class C>
std::istream& RBTree<T, C>::deserialize(std::istream& str) {
	if(root != NIL) {
		destroy(root);
	}
	str >> count;
	if(count > 0) {
		root = create();
		root->p = NIL;
		root->deserialize(str);
	} else {
		root = NIL;
	}
	return str;
}

template<class T, class C>
std::ostream& operator<<(std::ostream& str, RBTree<T, C>& tree) {
	return tree.serialize(str);
}

template<class T, class C>
std::istream& operator>>(std::istream& str, RBTree<T, C>& tree) {
	return tree.deserialize(str);
}

#endif /* RBTREE_H_ */
