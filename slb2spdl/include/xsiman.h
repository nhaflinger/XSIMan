//*****************************************************************************
//  File: xsiman.h
//  Description: xsiman data.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiman_H
#define xsiman_H

#include "stdafx.h"
#include <xsi_matrix4.h>
#include <xsi_x3dobject.h>
#include <xsi_camera.h>
#include <xsi_light.h>
#include <xsi_shader.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_geometry.h>
#include <xsi_primitive.h>
#include <xsi_polygonmesh.h>
#include <xsi_nurbscurvelist.h>
#include <xsi_nurbssurfacemesh.h>
#include <vector>

using namespace XSI;

#define PI 3.1415926535897932384626433832795
#define INF 1.0e20f

struct float3 {
	float x;	
	float y;
	float z;
};


enum ParameterType {
	FloatType = 0,
	PointType = 1,
	VectorType = 2,
	NormalType = 3,
	ColorType = 4,
	StringType = 5,
	HpointType = 6
};

enum DetailType {
	ConstantDetail = 0,
	UniformDetail = 1,
	VaryingDetail = 2,
	VertexDetail = 3,
	FaceVaryingDetail = 4
};

enum ShaderArgType {
    UnknownShaderArg = 0,
    PointShaderArg = 1,
	ColorShaderArg = 2,
    ScalarShaderArg = 3,
	StringShaderArg = 4,
	SurfaceShader = 5,
	LightShader = 6,
	DisplacementShader = 7,
	VolumeShader = 8,
	TransformationShader = 9,
	ImagerShader = 10,
	ProcedureShader = 11,
	VectorShaderArg = 12,
	NormalShaderArg = 13,
    MatrixShaderArg = 14	
};

enum ShaderDetailType {
    UnknownShaderDetail = 0,
    VaryingShaderDetail = 1,
    UniformShaderDetail = 2
};



#endif