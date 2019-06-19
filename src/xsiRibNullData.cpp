//*****************************************************************************
// File: xsiRibNullData.cpp
// Description: xsiRibNullData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibNullData.h"

xsiRibNullData::xsiRibNullData(X3DObject in_obj, float cframe)
{
	CString objName = in_obj.GetFullName();
	camera.name = objName;
	MATH::CTransformation nullTransform(in_obj.GetKinematics().GetGlobal().GetTransform(cframe));
	MATH::CMatrix4 matrix4(nullTransform.GetMatrix4());
	camera.matrix4 = matrix4;
}

xsiRibNullData::~xsiRibNullData()
{
}

void xsiRibNullData::write()
{
	char* namePtr = NULL;
	XSIW2A(&namePtr, camera.name.GetWideString());

	RtMatrix nullMatrix;
	for(unsigned i = 0; i < 4 ; i++) {
		for(unsigned j = 0; j < 4 ; j++) {
			nullMatrix[i][j] = (float)camera.matrix4.GetValue(i,j);
		}
	}

	RiTransformBegin();
	RiConcatTransform(nullMatrix);
	RiCoordinateSystem(namePtr);
	RiTransformEnd();
}

ObjectType xsiRibNullData::type() const
{
	return NullObject;
}

