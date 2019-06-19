//*****************************************************************************
// File: xsiRibHairData.h
// Description: xsiRibHairData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibHairData_H
#define xsiRibHairData_H


#include <xsi_hairprimitive.h>
#include <xsi_renderhairaccessor.h> 
#include "xsiRibCurveData.h"


//*****************************************************************************
//    class: xsiRibHairData xsiRibHairData.h
//*****************************************************************************

class xsiRibHairData : public xsiRibData
{
public:
	xsiRibHairData(X3DObject, float);
	~xsiRibHairData();
	virtual void write();
	virtual ObjectType type() const;

private:
	CString name;
	RtInt ncurves;
	RtInt* nverts;
	RtFloat* cv;
	RtFloat* width;
	RtFloat* id;
	RtFloat* sroot;
	RtFloat* troot;
	RtFloat* tval;
	RtFloat* surf_normal;
	RtFloat flatness;
	bool bevelcurves;
};


#endif