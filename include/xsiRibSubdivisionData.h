//*****************************************************************************
// File: xsiRibSubdivisionData.h
// Description: xsiRibSubdivisionData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibSubdivisionData_H
#define xsiRibSubdivisionData_H

#include <xsi_point.h>
#include <xsi_polygonnode.h>
#include <xsi_facet.h>
#include <xsi_cluster.h>
#include "xsiRibData.h"

//*****************************************************************************
//    class: xsiRibSubdivisionData xsiRibSubdivisionData.h
//*****************************************************************************

class xsiRibSubdivisionData : public xsiRibData
{
public:
	xsiRibSubdivisionData(X3DObject, float);
	~xsiRibSubdivisionData();
	void addFaceEditTag(X3DObject, float);
	virtual void write();
	virtual ObjectType type() const;

private:
	CString name;
	RtInt numFaces;
	RtInt* nverts;
	RtInt* verts;
	CString scheme;
	bool interpolateBoundary;
	vector<float> creases;
	vector<long> edgeverts;
	vector<float> corners;
	vector<long> pointverts;
	long numFaceSets;
	CLongArray faces;
	CLongArray sortOrder;
	vector<CString> faceSets;
	CStringArray rsrcHandles;
};

#endif