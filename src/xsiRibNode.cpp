//*****************************************************************************
// File: xsiRibNode.cpp
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

#include "xsiRibNode.h"

xsiRibNode::xsiRibNode()
{
}

xsiRibNode::~xsiRibNode()
{
}

nodeStruct xsiRibNode::get()
{
	return node;
}

void xsiRibNode::put(nodeStruct obj)
{
	node = obj;
}
