//*****************************************************************************
// File: xsiRibCameraData.h
// Description: xsiRibCameraData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibCameraData_H
#define xsiRibCameraData_H

#include "xsiRibData.h"


//*****************************************************************************
//    class: xsiRibCameraData xsiRibCameraData.h
//*****************************************************************************


class xsiRibCameraData : public xsiRibData
{
public:
	xsiRibCameraData(X3DObject, float);
	~xsiRibCameraData();
	virtual void write();
	virtual ObjectType type() const;
	cameraStruct get();

private:
	cameraStruct camera;
};

#endif