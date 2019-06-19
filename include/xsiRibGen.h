//*****************************************************************************
// File: xsiRibGen.h
// Description: xsiRibGen object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibGen_H
#define xsiRibGen_H

#include "ri.h"
#include "xsiRibContext.h"

class xsiRibGen;

// here are the two entrypoints RIB Generators must implement. 
extern "C" xsiRibGen *xsiRibGenCreate();
extern "C" void xsiRibGenDestroy(xsiRibGen*);

class xsiRibGen
{
public:
	xsiRibGen(xsiRibContext*);

    ~xsiRibGen();

	virtual void setArgs(xsiRibContext*, int n, RtToken tokens[], RtPointer values[]);

	virtual void bound(xsiRibContext*, RtBound b);
};

#endif