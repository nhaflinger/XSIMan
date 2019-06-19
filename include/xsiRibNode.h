//*****************************************************************************
// File: xsiRibNode.h
// Description: xsiRibNode object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibNode_H
#define xsiRibNode_H

#include <xsi_string.h>
#include <xsi_x3dobject.h>
#include <map>
#include "xsiRibData.h"

//*****************************************************************************
//    class: xsiRibNode xsiRibNode.h
//*****************************************************************************

class xsiRibNode
{
public:
	xsiRibNode();
	~xsiRibNode();
	nodeStruct get();
	void put(nodeStruct);

private:
	nodeStruct node;
	vector<xsiRibNode> children;
	friend class xsiRibTranslator;
};

#endif