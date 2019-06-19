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
#include <ri.h>

using namespace XSI;

#define PI 3.1415926535897932384626433832795
#define INF 1.0e20f

struct float3 {
	float x;	
	float y;
	float z;
};

enum rendererType {
	PRMan = 0,
	Delight = 1,
	RenderDotC = 2,
	Air = 3
};

enum PixelFilterType {
	BoxFilter = 0,
	TriangleFilter = 1,
	CatmullRomFilter = 2,
	GaussianFilter = 3,
	SincFilter = 4,
	BlackmanHarrisFilter = 5,
	MitchellFilter = 6,
	SepCatmullRomFilter = 7,
	BesselFilter = 8
};

enum LightType {	
	PointLight = 0,
	DistantLight = 1,
	SpotLight = 2,
	AmbientLight = 3,
	AreaLight = 4,
	RManLight = 5,
	UnknownLight = 6
};

enum ObjectType {
	NurbsObject = 0,
	CurveObject = 1,
	MeshObject = 2,
	ParticlesObject = 3,
	SubdivisionObject = 4,
	HairObject = 5,
	NullObject = 6,
	LightObject = 7,
	CameraObject = 8,
	ModelObject = 9,
	EndModelObject = 10,
	UnknownObject = 11
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

enum JobType {
	BeautyJob = 0,
	ShadowJob = 1,
	ArchiveJob = 2,
	EnvmapJob = 3
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
	VectorShaderArg = 11,
	NormalShaderArg = 12,
    MatrixShaderArg = 13	
};

enum ShaderDetailType {
    UnknownShaderDetail = 0,
    VaryingShaderDetail = 1,
    UniformShaderDetail = 2
};

enum FaceView {
	PX = 0,
	NX = 1,
	PY = 2,
	NY = 3,
	PZ = 4,
	NZ = 5
}; 

enum AreaLightType {
	RectangleAreaLight = 1,
	DiskAreaLight = 2,
	SphereAreaLight = 3,
	CylinderAreaLight = 4
};

enum ShutterTiming {
	OpenOnFrame = 1,
	CloseOnFrame = 2,
	CenterOnFrame = 3
};

enum GroupType {	
	CSGPrimGroup = 0,
	CSGDiffGroup= 1,
	CSGIntGroup = 2,
	CSGUnionGroup = 3,
	LODControl = 4
};

enum TaskType {
	GlobalTask = 0,
	SuperTask = 1,
	SubTask = 2
};

enum ParticleType {
	PointParticle = 0,
	SphereParticle = 1,
	BlobbyParticle = 2,
	SpriteParticle = 3
};

typedef struct {
	MATH::CMatrix4 matrix4;
	RtFloat nearClip;
	RtFloat farClip;
	RtFloat horizFOV;
	bool orthogonal;
	RtFloat orthoWidth;
	RtFloat orthoHeight;
	CString name;
	bool motionBlur;
	RtFloat shutter;
	RtFloat fStop;
	RtFloat focalDistance;
	RtFloat focalLength;
} cameraStruct;

typedef struct {
	float lightType;
	float areaLightType;
	float areaLightSamples;
	float areaLightSizeX;
	float areaLightSizeY;
	float areaLightSizeZ;
	RtFloat color[3];
	RtFloat intensity;
	RtFloat coneAngle;
	RtFloat coneDeltaAngle;
	RtFloat beamDistribution;
	RtFloat falloffExponent;
	RtFloat from, to;
	RtLightHandle handle;
	MATH::CMatrix4 matrix4;
	CString lightName;
	bool shadow;
	CString shadowName;
	int shadowMapRes;
	RtFloat shadowMapBias;
	RtFloat shadowMapSamp;
	CString shaderName;
	RtFloat nondiffuse;
	RtFloat nonspecular;
	CRefArray associatedModels;
} lightStruct;

typedef struct {
	CString fullName;
	SceneItem sceneItem;
	double cframe;
	int sample;
	ObjectType objectType;
	bool renderVisibility;
	bool primaryVisibility;
	bool secondaryVisibility;
	bool shadowVisibility;
	bool instanceGeometry;
	bool generateEnvmap;
	RtObjectHandle handle[5];
	int motionSamples;
} nodeStruct;

typedef struct {
	int width, height;
	RtFloat aspectRatio;
	CString name;
	CString imageMode;
	CString format;
	CString ribFileName;
	CString imageName;
	cameraStruct camera[5];
	bool reuseData;
	CString shadowMapType;
	CString shadowVolInterp;
	int shadowPixelSamples;
	bool isPointLight;
	int cubeMapDir;
	bool isEnvmap;
	CString fileExt;
	CString writeFrequency;
	bool rewriteData;
	JobType jobType;
} jobStruct;

typedef struct {
	TaskType type;
	CString name;
	unsigned frame;
	CString fileName;
	CString imageName;
} taskStruct;

typedef struct {
	CString RibName;
	CString ImageName;
	CString ExportCamera;
	CString DisplayDriver;
	float FrameStart;
	float FrameEnd;
	float FrameStep;
} commandLineOptions;

#endif