//*****************************************************************************
// File: xsiRibCurveData.h
// Description: xsiRibCurveData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibCurveData_H
#define xsiRibCurveData_H

#include "xsiRibData.h"

//*****************************************************************************
//    class: xsiRibCurveData xsiRibCurveData.h
//*****************************************************************************

class xsiRibCurveData : public xsiRibData
{
public:
	xsiRibCurveData(X3DObject, float, long);
	~xsiRibCurveData();
	virtual void write();
	virtual ObjectType type() const;
	virtual bool isUClosed() { return uClosed; }

private:
	CString name;
	RtInt ncurves;
	RtInt* nverts;
	RtFloat* cv;
	RtFloat* width;
	bool uClosed;
	RtFloat flatness;
	bool bevelcurves;
};

#endif