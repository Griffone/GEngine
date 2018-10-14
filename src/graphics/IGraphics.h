#pragma once

/*
	Graphics context interface
*/

#include "../Window.h"

/// Interface for graphical context
class IGraphics {
public:
	/// Any graphics context is locked to a window for the purposes of the current project
	IGraphics(Window &);
	virtual ~IGraphics() = default;

	// Will likely change in the future, as actual in-project pipeline for drawing is constructed
	virtual void load() = 0;
	virtual void draw() = 0;

protected:
	Window &window;
};