//*****************************************************************************
// File: xsiRibMeshData.h
// Description: xsiRibMeshData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibMeshData_H
#define xsiRibMeshData_H

#include <xsi_point.h>
#include <xsi_polygonnode.h>
#include <xsi_facet.h>
#include <xsi_cluster.h>
#include "xsiRibData.h"

//*****************************************************************************
//    class: xsiRibMeshData xsiRibMeshData.h
//*****************************************************************************

class xsiRibMeshData : public xsiRibData
{
public:
	xsiRibMeshData(X3DObject, float, CString);
	~xsiRibMeshData();
	virtual void write();
	virtual ObjectType type() const;

private:
	CString name;
	RtInt numFaces;
	RtInt* nverts;
	RtInt* verts;
};

#endif