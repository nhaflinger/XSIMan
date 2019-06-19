//*****************************************************************************
// File: xsiRibMap.h
// Description: xsiRibMap object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibMap_H
#define xsiRibMap_H

#include <map>
#include <xsi_x3dobject.h>
#include "xsiRibNode.h"

using namespace std;

//*****************************************************************************
//    class: xsiRibMap xsiRibMap.h
//*****************************************************************************

class xsiRibMap
{
public:
	xsiRibMap();
	~xsiRibMap();
	void insert(XSI::SceneItem, double, int, ObjectType, CString);
	xsiRibNode* find(XSI::SceneItem);

private:
	vector<xsiRibNode> nodeMap;
	friend class xsiRibTranslator;
};

#endif