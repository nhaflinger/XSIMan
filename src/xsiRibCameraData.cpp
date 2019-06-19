//*****************************************************************************
// File: xsiRibCameraData.cpp
// Description: xsiRibCameraData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibCameraData.h"

xsiRibCameraData::xsiRibCameraData(X3DObject in_obj, float cframe)
{
	Camera camData(in_obj);
	CString objName = in_obj.GetFullName();
	camera.name = objName;
	camera.orthogonal = (camData.GetParameterValue(CString(L"proj"), cframe) ? false : true);
	camera.nearClip = camData.GetParameterValue(CString(L"near"), cframe);
	camera.farClip = camData.GetParameterValue(CString(L"far"), cframe);
	camera.orthoWidth = camData.GetParameterValue(CString(L"orthoheight"), cframe);
	camera.orthoHeight = camData.GetParameterValue(CString(L"orthoheight"), cframe);
	camera.horizFOV = camData.GetParameterValue(CString(L"fov"), cframe);
	camera.focalLength = camData.GetParameterValue(CString(L"projplanedist"), cframe);
	MATH::CTransformation cameraTransform(in_obj.GetKinematics().GetGlobal().GetTransform(cframe));
	MATH::CMatrix4 matrix4(cameraTransform.GetMatrix4());
	camera.matrix4 = matrix4;
}

xsiRibCameraData::~xsiRibCameraData()
{
}

void xsiRibCameraData::write()
{
	char* namePtr = NULL;
	XSIW2A(&namePtr, camera.name.GetWideString());

	// cameras in XSI seem to be flipped
	double flipCamera[] = {1.0, 1.0, -1.0, 1.0};

	RtMatrix cameraMatrix;
	for(unsigned i = 0; i < 4 ; i++) {
		for(unsigned j = 0; j < 4 ; j++) {
			cameraMatrix[i][j] = (float)(camera.matrix4.GetValue(i,j) * flipCamera[i]);
		}
	}

	RiTransformBegin();
	RiConcatTransform(cameraMatrix);
	RiCoordinateSystem(namePtr);
	RiTransformEnd();
}

ObjectType xsiRibCameraData::type() const
{
	return CameraObject;
}

cameraStruct xsiRibCameraData::get()
{
	return camera;
}
