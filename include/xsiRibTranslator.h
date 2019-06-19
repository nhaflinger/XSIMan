//*****************************************************************************
// File: xsiRibTranslator.h
// Description: xsiRibTranslator object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibTranslator_H
#define xsiRibTranslator_H

#include <xsi_status.h>
#include <xsi_model.h>
#include <xsi_geometry.h>
#include <xsi_polygonmesh.h>
#include <xsi_nurbscurvelist.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_application.h>
#include <xsi_scene.h>
#include <xsi_material.h>
#include <xsi_projectitem.h>
#include <xsi_parameter.h>
#include <xsi_siobject.h>
#include <xsi_shader.h>
#include <xsi_camera.h>
#include <SemanticLayer.h>
#include <xsiman.h>
#include <ri.h>
#include "xsiRibCameraData.h"
#include "xsiRibLightData.h"
#include "xsiRibMap.h"

using namespace std;


struct ltwstr
{
	bool operator()(const wchar_t* s1, const wchar_t* s2) const
	{
		return wcscmp(s1, s2) < 0;
	}
};


//*****************************************************************************
//    class: xsiRibTranslator xsiRibTranslator.h
//*****************************************************************************

class xsiRibTranslator
{
public:
	xsiRibTranslator();
	~xsiRibTranslator();
	XSI::CStatus ExportFile(commandLineOptions*, bool);

private:
	XSI::CStatus xsiManOptions();
	XSI::CStatus ribPrologue();
	XSI::CStatus ribEpilogue();
	XSI::CStatus instanceGeometry();
	XSI::CStatus framePrologue();
	XSI::CStatus frameBody();
	XSI::CStatus frameEpilogue();
	XSI::CStatus exportLights();
	XSI::CStatus exportCoordinateSystems();
	XSI::CStatus insertFrontplaneGeom();
	XSI::CStatus addCameraShaders();
	XSI::CStatus insertBackplaneGeom();
	XSI::CStatus addPassShaders();
	XSI::CStatus updateSceneTextureMaps(XSI::Scene&);
	XSI::CStatus exportGeometry();
	XSI::CStatus scanScene(XSI::SceneItem&, CString);
	XSI::CStatus appendBeautyJob(XSI::X3DObject&);
	XSI::CStatus appendShadowJob(XSI::X3DObject&);
	XSI::CStatus appendArchiveJob(XSI::X3DObject&);
	XSI::CStatus appendEnvmapJob(XSI::X3DObject&);
	XSI::CStatus appendCausticMapJob(XSI::X3DObject&);
	XSI::CStatus appendGlobalMapJob(XSI::X3DObject&);
	XSI::CStatus appendInstanceJob();
	void getMatrix(X3DObject, RtMatrix, float);
	XSI::CStatus execute(CString);
	XSI::CStatus createProcess(CString);
	XSI::CStatus makedir(CString);
	XSI::CStatus builtInShader(XSI::Material&, CString&);
	XSI::CStatus getPassShader(XSI::Pass&, CString&);
	XSI::CStatus getSurfaceShader(XSI::Material&, CString, CString&);
	XSI::CStatus getDisplacementShader(XSI::Material&, CString, CString&);
	XSI::CStatus getVolumeShader(XSI::Material&, CString, CString&);
	XSI::CStatus getImageClip(XSI::ImageClip2);
	XSI::CStatus updateTextureMaps(XSI::Material&);
	XSI::CStatus getShaderNodes(XSI::Shader);
	XSI::CStatus getShaderParams(Shader);
	void addGlobalIllumParams(vector<xsiRibTokens>&);
	XSI::CString getFileExt(CString);
	XSI::CString alfredJobScript();
	XSI::CString shellJobScript();
	XSI::CString constructFileName(bool, jobStruct*);
	XSI::CString textureCommand(CString, CString);
	XSI::CString cleanupCommand(CString);
	void writePixelFilter(PixelFilterType pixelFilter, float, float);
	XSI::CStatus rmanShaderParams(Shader, RtColor, RtColor, float&);
	void shaderNetworkData();
	void compileShader(CString);
	XSI::CStatus shaderArgIsValid(shaderTemplateItem);
	XSI::CStatus shaderFuncIsValid(CString);
	XSI::CString getParamValue(Parameter);
	bool globalEnvironmentParam(CString);
	CString getArgType(CString);
	bool shaderExists(CString);
	void addShaderParamToTokenList(CString, CString);
	CString subGlobalVars(CString);
	void xsiRibTranslator::addDefaultMaterial(XSI::X3DObject&, nodeStruct);
	void xsiRibTranslator::addClusterMaterials(XSI::X3DObject&, nodeStruct);
	XSI::Application m_app;
	XSI::Model m_sceneRoot;

	vector<CString> prejobList;
	vector<CString> preframeList;
	vector<taskStruct*> renderList;
	vector<CString> cleanupList;
	RtFloat shadingRate;
	RtInt pixelSamples[2];
	RtInt bucketSize[2];
	RtInt gridSize;
	RtInt textureMemory;
	RtInt eyeSplits;
	RtFloat shadowBias;
	RtFloat filterSize[2];
	PixelFilterType pixelFilter;
	RtInt quantValue, quantizeOne, quantizeMin, quantizeMax;
	RtFloat dither;
	RtFloat gainValue;
	RtFloat gammaValue;
	RtFloat cropWindowX1, cropWindowY1, cropWindowX2, cropWindowY2;
	jobStruct* job;
	xsiRibMap nodeData;
	vector<xsiRibTokens> ribTokens;
	vector<xsiRibTokens> ribTokensSurf, ribTokensDisp, ribTokensVol, ribTokensInterior, ribTokensImager;
	vector<shaderTemplateItem> shaderTemplates;
	vector<CString> shaderList;
	vector<shaderParamStruct> shaderParamList;
	map<const wchar_t*, wchar_t*, ltwstr> shaderFuncs;
	map<const wchar_t*, wchar_t*, ltwstr> shaderArgs;
	map<const wchar_t*, wchar_t*, ltwstr> shaderArgDefs;
	map<const wchar_t*, wchar_t*, ltwstr> shaderArgTypes;
	vector<CString> lightSwitch;
};

#endif 
