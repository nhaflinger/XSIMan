//*****************************************************************************
// File: xsiRibNullData.h
// Description: xsiRibNullData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibNullData_H
#define xsiRibNullData_H

#include "xsiRibData.h"

//*****************************************************************************
//    class: xsiRibNullData xsiRibNullData.h
//*****************************************************************************

class xsiRibNullData : public xsiRibData
{
public:
	xsiRibNullData(X3DObject, float);
	~xsiRibNullData();
	virtual void write();
	virtual ObjectType type() const;

private:
	cameraStruct camera;
};

#endif