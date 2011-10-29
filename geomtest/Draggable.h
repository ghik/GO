/*
 * Draggable.h
 *
 *  Created on: 10-10-2011
 *      Author: ghik
 */

#ifndef DRAGGABLE_H_
#define DRAGGABLE_H_

class Draggable {
public:
	Draggable();
	virtual ~Draggable();

	virtual bool drags(double x, double y) = 0;
	virtual void draggedTo(double x, double y) = 0;
};

#endif /* DRAGGABLE_H_ */
