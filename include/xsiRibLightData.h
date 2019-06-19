//*****************************************************************************
// File: xsiRibLightData.h
// Description: xsiRibLightData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibLightData_H
#define xsiRibLightData_H

#include "xsiRibData.h"
#include "xsiRibTranslator.h"
#include <map>


//*****************************************************************************
//    class: xsiRibLightData xsiRibLightData.h
//*****************************************************************************

class xsiRibLightData : public xsiRibData
{
public:
	xsiRibLightData(X3DObject, float);
	~xsiRibLightData();
	virtual void write();
	virtual ObjectType type() const;
	RtLightHandle getLightHandle() {return light.handle;}
	CString subGlobalVars(CString);

private:	
	XSI::CString constructFileName(bool, jobStruct*);
	lightStruct light;
	friend class xsiRibTranslator;
};

#endif