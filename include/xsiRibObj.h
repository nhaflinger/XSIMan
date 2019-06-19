//*****************************************************************************
// File: xsiRibObj.h
// Description: xsiRibData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibObj_H
#define xsiRibObj_H

#include "xsiRibData.h"
#include "xsiRibObj.h"

//*****************************************************************************
//    class: xsiRibObj xsiRibObj.h
//*****************************************************************************

class xsiRibObj
{
public:
	xsiRibObj();
	~xsiRibObj();

private:
	MATH::CMatrix4 matrix4;
	RtObjectHandle handle;
	xsiRibData* data;
};

#endif