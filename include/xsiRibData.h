//*****************************************************************************
// File: xsiRibData.h
// Description: xsiRibData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibData_H
#define xsiRibData_H

#include "xsiman.h"
#include "xsiRibTokens.h"
#include <ri.h>

using namespace std;

//*****************************************************************************
//    class: xsiRibData xsiRibData.h
//*****************************************************************************

class xsiRibData
{
public:
	xsiRibData();
	~xsiRibData();
	virtual void write();
	virtual ObjectType type() const = 0;
	virtual bool isUClosed() { return uClosed; }
	virtual void addPrimvar(ParameterType, UserDataMap);
	vector<xsiRibTokens> ribTokens;

private:
	CString name;
	bool uClosed;
};

#endif