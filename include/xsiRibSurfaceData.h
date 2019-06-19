//*****************************************************************************
// File: xsiRibSurfaceData.h
// Description: xsiRibSurfaceData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibSurfaceData_H
#define xsiRibSurfaceData_H

#include "xsiRibData.h"

//*****************************************************************************
//    class: xsiRibSurfaceData xsiRibSurfaceData.h
//*****************************************************************************

class xsiRibSurfaceData : public xsiRibData
{
public:
	xsiRibSurfaceData(X3DObject, float);
	~xsiRibSurfaceData();
	virtual void write();
	virtual ObjectType type() const;
	bool hasTrimCurves() const;
	void writeTrims() const;

private:
	CString name;
	RtInt nu, nv;
	RtInt uorder, vorder;
	RtFloat* uknot;
	RtFloat* vknot;
	RtFloat umin, umax, vmin, vmax;
	RtFloat* cv;
	bool uClosed, vClosed;
	bool hasTrims;
	RtInt* tcurves;
	RtInt* torder;
	RtFloat* tknot;
	RtFloat* tmin;
	RtFloat* tmax;
	RtInt* tn;
	RtFloat* tu;
	RtFloat* tv;
	RtFloat* tw;
	RtInt tloops;
};

#endif