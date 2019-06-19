//*****************************************************************************
// File: xsiRibLightData.cpp
// Description: xsiRibLightData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibLightData.h"
#include "xsiUtilities.h"
#include <xsi_model.h>
#include <xsi_group.h>
#include <xsi_application.h>
#include <xsi_project.h>

extern multimap<JobType, jobStruct*> jobList;
extern float g_CurrentFrame;
extern CString g_RibPath;
extern CString g_PixPath;
extern CString g_TexPath;
extern CString g_ShdPath;
extern signed short g_FramePadding;
extern float g_ShadowMapBias;
extern bool g_NamedHandles;
extern CString g_HandlePrefix;


xsiRibLightData::xsiRibLightData(X3DObject in_obj, float currentFrame)
{
	Application app;
	Light xsilight(in_obj);
	CString objName = in_obj.GetFullName();

	// get general light info
	light.lightName = objName;	
	light.coneAngle = xsilight.GetParameterValue(CString(L"LightCone"), currentFrame);
	light.coneAngle *= 0.5f*(float)(PI / 180.);
	light.beamDistribution = xsilight.GetParameterValue(CString(L"LightExponent"), currentFrame);
	light.lightType = xsilight.GetParameterValue(CString(L"Type"));

	// if this is area light, get more info
	if (xsilight.GetParameterValue(CString(L"LightArea"))) {
		light.lightType = AreaLight;
		light.areaLightType = xsilight.GetParameterValue(CString(L"LightAreaGeom"));
		float sampx = xsilight.GetParameterValue(CString(L"LightAreaSampU"), currentFrame);
		float sampy = xsilight.GetParameterValue(CString(L"LightAreaSampV"), currentFrame);
		light.areaLightSamples = sampx * sampy;
		light.areaLightSizeX = xsilight.GetParameterValue(CString(L"LightAreaXformSX"), currentFrame);
		light.areaLightSizeY = xsilight.GetParameterValue(CString(L"LightAreaXformSY"), currentFrame);
		light.areaLightSizeZ = xsilight.GetParameterValue(CString(L"LightAreaXformSZ"), currentFrame);
	}

	if (xsilight.GetParameterValue(CString(L"DiffuseContribution"), currentFrame)) light.nondiffuse = 0.;
	else light.nondiffuse = 1.;
	if (xsilight.GetParameterValue(CString(L"SpecularContribution"), currentFrame)) light.nonspecular = 0.;
	else light.nonspecular = 1.;

	if (xsilight.GetParameterValue(CString(L"SelectiveInclusive"))) light.inclusive = true;
	else light.inclusive = false;

	// set some defaults
	light.intensity = 1.0;
	light.coneDeltaAngle = (float)(5.0 * PI / 180.);
	light.color[0] = 1.0;
	light.color[1] = 1.0;
	light.color[2] = 1.0;
	light.falloffExponent = 0.0;
	light.shadowcolor[0] = 0.0;
	light.shadowcolor[1] = 0.0;
	light.shadowcolor[2] = 0.0;
	
	// shadow info
	light.shadow = xsilight.GetParameterValue(CString(L"ShadowMap"));
	light.shadowMapRes = (int)((float)xsilight.GetParameterValue(CString(L"ShadowMapRes"), currentFrame));
	light.shadowMapBias = xsilight.GetParameterValue(CString(L"ShadowMapBias"), currentFrame);
	if (light.shadowMapBias <= 0) light.shadowMapBias = g_ShadowMapBias;
	light.shadowMapSamp = xsilight.GetParameterValue(CString(L"ShadowMapSamp"), currentFrame);
	light.shadowMapBlur = xsilight.GetParameterValue(CString(L"ShadowMapSoft"), currentFrame);

	// check for shadow helper to see if ray trace shadows are turned on
	CRef propRef = in_obj.GetProperties().GetItem(L"XSIManShadows");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter rayTraceShadows = prop.GetParameters().GetItem(L"RayTraceShadows");
		bool traceshad = rayTraceShadows.GetValue();
		if (traceshad) {
			light.shadow = true;
			light.shadowName = L"raytrace";
		}
	}

	MATH::CTransformation lightTransform(in_obj.GetKinematics().GetGlobal().GetTransform(currentFrame));
	MATH::CMatrix4 matrix4(lightTransform.GetMatrix4());
	light.matrix4 = matrix4;

	// get shader arguments
	CRefArray shaderArray = xsilight.GetShaders();
	bool rmanShader = false;
	for (long i=0; i<shaderArray.GetCount(); i++ ) {
		Shader shader = shaderArray[i];
		rmanShader = shader.HasRenderer(L"prman") || shader.HasRenderer(L"3delight") || shader.HasRenderer(L"Air");

		if (rmanShader) {
			CString renderer;
			if (shader.HasRenderer(L"prman")) renderer= L"prman";
			else if (shader.HasRenderer(L"3delight")) renderer= L"3delight";
			else if (shader.HasRenderer(L"Air")) renderer= L"Air";

			CRefArray paramArray = shader.GetParameters();
			long numParam = paramArray.GetCount();
			CValue shaderType = shader.GetRendererOption(renderer, L"Type");
			if (shaderType == L"light") {
				light.lightType = RManLight;
				light.shaderName = shader.GetCodePath(renderer);

				for (long j=0; j<numParam; j++ ) {
					Parameter param = paramArray[j];
					CValue::DataType ptype = param.GetValueType();
					CString paramName = param.GetName();
					CValue paramValue = param.GetValue();

					// determine parameter detail and type
					CValue paramDetailType = shader.GetRendererOption(renderer, paramName);
					wchar_t paramdetail[10];
					wchar_t paramtype[10];
					swscanf(paramDetailType.GetAsText().GetWideString(), L"%s %s", paramdetail, paramtype);
					CString paramType = CString(paramtype);
					CString paramDetail = CString(paramdetail);
					
					char* pname = NULL;
					XSIW2A(&pname, paramName.GetWideString());
					if (ptype == siFloat) {
						xsiRibTokens tokenDataFloat;
						tokenDataFloat.put(pname, FloatType, false, false, false, 0);

						if (paramDetail == L"uniform") tokenDataFloat.putDetailType(UniformDetail);
						else if (paramDetail == L"varying") tokenDataFloat.putDetailType(VaryingDetail);
						else tokenDataFloat.putDetailType(UniformDetail);

						tokenDataFloat.assignFloat(0, (float)paramValue);
						ribTokens.push_back(tokenDataFloat);
					}
					else if (ptype == siUnknown || ptype == siEmpty) {
						CStatus stat; float alphaChan;
						xsiRibTokens tokenDataFloat;

						if (paramDetail == L"uniform") tokenDataFloat.putDetailType(UniformDetail);
						else if (paramDetail == L"varying") tokenDataFloat.putDetailType(VaryingDetail);
						else tokenDataFloat.putDetailType(UniformDetail);

						float floatParam[3] = {0.0, 0.0, 0.0};
						if (paramType == L"color") {
							stat = shader.GetColorParameterValue(paramName, floatParam[0], floatParam[1], 
								floatParam[2], alphaChan, currentFrame);
							tokenDataFloat.put(pname, ColorType, false, false, false, 0);
						}
						else if (paramType == L"point") {
							// this is a compound parameter
							getCompoundParameter(param, floatParam);
							tokenDataFloat.put(pname, PointType, false, false, false, 0);
						}							
						else if (paramType == L"vector") {
							// this is a compound parameter
							getCompoundParameter(param, floatParam);
							tokenDataFloat.put(pname, VectorType, false, false, false, 0);
						}
						else if (paramType == L"normal") {
							// this is a compound parameter
							getCompoundParameter(param, floatParam);
							tokenDataFloat.put(pname, NormalType, false, false, false, 0);
						}
						else {
							tokenDataFloat.put(pname, ColorType, false, false, false, 0);
						}

						tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
						ribTokens.push_back(tokenDataFloat);
					}
					else if (ptype == siString) {
						if (paramName != L"Name") {
							xsiRibTokens tokenDataString;

							tokenDataString.put(pname, StringType, false, false, false, 0);

							if (paramDetail == L"uniform") tokenDataString.putDetailType(UniformDetail);
							else if (paramDetail == L"varying") tokenDataString.putDetailType(VaryingDetail);
							else tokenDataString.putDetailType(UniformDetail);
							
							// we seem to need to replace back slashes
							CString nuStringName = paramValue.GetAsText();

							// substitute global variables
							nuStringName = subGlobalVars(nuStringName);

							// replace object name as well	
							if (stringHasChar(nuStringName, L"$OBJ")) {
								nuStringName = replaceSubString(nuStringName, L"$OBJ", objName);
							}

							// we seem to need to replace back slashes
							nuStringName = replaceSubString(nuStringName, L"\\", L"/");

							// check for autoshadow keyword 
							if (!wcscmp(nuStringName.GetWideString(), L"autoshadow")) {
								// find shadow job that corresponds to this light
								multimap<JobType, jobStruct*>::iterator jter;
								for (jter=jobList.begin(); jter!=jobList.end(); ++jter) {
									if ((*jter).first == ShadowJob) {
										jobStruct* shadowJob = (*jter).second;	
										if (shadowJob->jobType == ShadowJob) {
											if(shadowJob->name == objName) { 
												CString shadowName;
												nuStringName = constructFileName(false, shadowJob);
												nuStringName = replaceSubString(nuStringName, L"\\", L"/");
											}
										}
									}
								}	
							}

							unsigned int strlen = nuStringName.Length();
							char* str = NULL;
							XSIW2A(&str, nuStringName.GetWideString());

							tokenDataString.assignString(str, strlen);
							ribTokens.push_back(tokenDataString);
						}
					}
				}
			}
		}
		else {
			light.intensity = shader.GetParameterValue(CString(L"intensity"), currentFrame);

			light.shadowcolor[0] = shader.GetParameterValue(CString(L"factor"), currentFrame);
			light.shadowcolor[1] = shader.GetParameterValue(CString(L"factor"), currentFrame);
			light.shadowcolor[2] = shader.GetParameterValue(CString(L"factor"), currentFrame);

			if (shader.GetParameterValue(CString(L"atten"))) {
				if (shader.GetParameterValue(CString(L"mode"))) light.falloffExponent = light.beamDistribution;
				else light.falloffExponent = 1.0;
			}

			float red, green, blue, alpha;
			shader.GetColorParameterValue(CString(L"color"), red, green, blue, alpha, currentFrame);
			light.color[0] = red;
			light.color[1] = green;
			light.color[2] = blue;

			// use photon energy instead of intensity if caustic or global illum light
			if (shader.GetParameterValue(CString(L"use_color")) == false && 
				(xsilight.GetParameterValue(CString(L"Caustics")) == true || xsilight.GetParameterValue(CString(L"GlobalIllum")) == true)) {
					light.intensity = xsilight.GetParameterValue(CString(L"LightEnergyIntens"), currentFrame);
					if (light.lightType == PointLight || light.lightType == SpotLight || light.lightType == AreaLight)
						light.falloffExponent = 2.0;
					if (light.lightType == DistantLight)
						light.falloffExponent = 0.0;
					light.color[0] = xsilight.GetParameterValue(CString(L"LightEnergyR"), currentFrame);
					light.color[1] = xsilight.GetParameterValue(CString(L"LightEnergyG"), currentFrame);
					light.color[2] = xsilight.GetParameterValue(CString(L"LightEnergyB"), currentFrame);
			}
			else if (shader.GetParameterValue(CString(L"use_color")) == true && 
				(xsilight.GetParameterValue(CString(L"Caustics")) == true || xsilight.GetParameterValue(CString(L"GlobalIllum")) == true)) {
					if (light.lightType == PointLight || light.lightType == SpotLight || light.lightType == AreaLight)
						light.falloffExponent = 2.0;
					if (light.lightType == DistantLight)
						light.falloffExponent = 0.0;
					light.intensity *= (float)shader.GetParameterValue(CString(L"energy_factor"), currentFrame);
			}

			light.coneDeltaAngle = shader.GetParameterValue(CString(L"spread"), currentFrame);
			light.coneDeltaAngle *= (float)(PI / 180.);
		}
	}

}

xsiRibLightData::~xsiRibLightData()
{
}

void xsiRibLightData::write()
{
	Application app;
	char* ambientlight = "ambientlight";
	char* distantlight = "distantlight";
	char* shadowdistant = "shadowdistant";
	char* pointlight = "pointlight";
	char* shadowpoint = "shadowpoint";
	char* spotlight = "spotlight";
	char* shadowspot = "shadowspot";

	char* namePtr = NULL;
	XSIW2A(&namePtr, light.lightName.GetWideString());
	RiAttribute("identifier", "name", &namePtr, RI_NULL);
	
	char* lightHandle = NULL;
	CString handleName = g_HandlePrefix + light.lightName;
	XSIW2A(&lightHandle, handleName.GetWideString());
	light.handle = strdup(lightHandle);

	RiTransformBegin();

	// lights in XSI seem to be flipped
	double flipLight[] = {1.0, 1.0, -1.0, 1.0};

	RtMatrix transformMatrix;
	for (unsigned i = 0; i < 4 ; i++) {
		for (unsigned j = 0; j < 4 ; j++) {
			transformMatrix[i][j] = (float)(light.matrix4.GetValue(i,j) * flipLight[i]);
		}
	}
	RiConcatTransform(transformMatrix);

	// see if XSIMan directory exists
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	if (CValue(xsimanhome).GetAsText() != L"") {
		ambientlight = "xsimanAmbientLight";
		distantlight = "xsimanDistantLight";
		shadowdistant = "xsimanDistantLight";
		pointlight = "xsimanPointLight";
		shadowpoint = "xsimanPointLight";
		spotlight = "xsimanSpotLight";
		shadowspot = "xsimanSpotLight";
	}
	else app.LogMessage(L"Warning: XSIMAN directory not set. Using default shaders.");

	switch ((int)light.lightType) {
		case AmbientLight:
			RiDeclare("intensity", "uniform float");
			RiDeclare("lightcolor", "uniform color");

			if (g_NamedHandles) {
				RiLightSource(ambientlight, "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color, RI_NULL);
			}
			else {
				light.handle = RiLightSource(ambientlight, "intensity", &light.intensity, "lightcolor", light.color, RI_NULL);
			}
			break;
		case DistantLight:
			RiDeclare("intensity", "uniform float");
			RiDeclare("lightcolor", "uniform color");
			RiDeclare("__nondiffuse", "uniform float");
			RiDeclare("__nonspecular", "uniform float");

			if (!light.shadow) {
				if (g_NamedHandles) {
					RiLightSource(distantlight, "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color, 
						"__nondiffuse", &light.nondiffuse, "__nonspecular", &light.nonspecular, RI_NULL);
				}
				else {
					light.handle = RiLightSource(distantlight, "intensity", &light.intensity, "lightcolor", light.color, 
						"__nondiffuse", &light.nondiffuse, "__nonspecular", &light.nonspecular, RI_NULL);
				}
			}
			else {
				RiDeclare("shadowcolor", "uniform color");
				RiDeclare("shadowsamples", "uniform float");
				RiDeclare("shadowbias", "uniform float");
				RiDeclare("shadowblur", "uniform float");
				// get shadow name
				char* shadPtr = NULL;
				XSIW2A(&shadPtr, light.shadowName.GetWideString());
				RiDeclare("shadowname", "uniform string");
				RtString shadowname = const_cast<char*>(shadPtr);
				RiDeclare("__category", "uniform string");
				char* shadowatten = "__shadowatten";
				
				if (g_NamedHandles) {
					RiLightSource(shadowdistant, "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", 
						light.color, "shadowcolor", light.shadowcolor, "shadowname", &shadowname, "__nondiffuse", &light.nondiffuse, "__nonspecular", 
						&light.nonspecular, "shadowsamples", &light.shadowMapSamp, "shadowbias", &light.shadowMapBias, 
						"shadowblur", &light.shadowMapBlur, "__category", &shadowatten, RI_NULL);
				}
				else {
					light.handle = RiLightSource(shadowdistant, "intensity", &light.intensity, "lightcolor", 
						light.color, "shadowcolor", light.shadowcolor, "shadowname", &shadowname, "__nondiffuse", &light.nondiffuse, "__nonspecular", 
						&light.nonspecular, "shadowsamples", &light.shadowMapSamp, "shadowbias", &light.shadowMapBias, 
						"shadowblur", &light.shadowMapBlur, "__category", &shadowatten, RI_NULL);
				}
			}
			break;
		case PointLight: 
			RiDeclare("intensity", "uniform float");
			RiDeclare("lightcolor", "uniform color");
			RiDeclare("falloffexponent", "uniform float");
			RiDeclare("__nondiffuse", "uniform float");
			RiDeclare("__nonspecular", "uniform float");

			if (!light.shadow) {
				if (g_NamedHandles) {
					RiLightSource(pointlight, "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color, 
						"falloffexponent", &light.falloffExponent, "__nondiffuse", &light.nondiffuse, "__nonspecular", 
						&light.nonspecular, RI_NULL);
				}
				else {
					light.handle = RiLightSource(pointlight, "intensity", &light.intensity, "lightcolor", light.color, 
						"falloffexponent", &light.falloffExponent, "__nondiffuse", &light.nondiffuse, "__nonspecular", 
						&light.nonspecular, RI_NULL);
				}
			}
			else {
				RiDeclare("shadowcolor", "uniform color");
				RiDeclare("shadowsamples", "uniform float");
				RiDeclare("shadowbias", "uniform float");
				RiDeclare("shadowblur", "uniform float");
				// get shadow name
				RiDeclare("sfpx", "uniform string");
				RiDeclare("sfnx", "uniform string");
				RiDeclare("sfpy", "uniform string");
				RiDeclare("sfny", "uniform string");
				RiDeclare("sfpz", "uniform string");
				RiDeclare("sfnz", "uniform string");

				CString nextDir;
				char* shadPtrPX = NULL;
				XSIW2A(&shadPtrPX, light.shadowName.GetWideString());
				RtString sfpx = const_cast<char*>(shadPtrPX);

				nextDir = replaceSubString(light.shadowName, L"PX", L"NX");
				char* shadPtrNX = NULL;
				XSIW2A(&shadPtrNX, nextDir.GetWideString());
				RtString sfnx = const_cast<char*>(shadPtrNX);

				nextDir = replaceSubString(light.shadowName, L"PX", L"PY");
				char* shadPtrPY = NULL;
				XSIW2A(&shadPtrPY, nextDir.GetWideString());
				RtString sfpy = const_cast<char*>(shadPtrPY);
				
				nextDir = replaceSubString(light.shadowName, L"PX", L"NY");
				char* shadPtrNY = NULL;
				XSIW2A(&shadPtrNY, nextDir.GetWideString());
				RtString sfny = const_cast<char*>(shadPtrNY);
				
				nextDir = replaceSubString(light.shadowName, L"PX", L"PZ");
				char* shadPtrPZ = NULL;
				XSIW2A(&shadPtrPZ, nextDir.GetWideString());
				RtString sfpz = const_cast<char*>(shadPtrPZ);
				
				nextDir = replaceSubString(light.shadowName, L"PX", L"NZ");
				char* shadPtrNZ = NULL;
				XSIW2A(&shadPtrNZ, nextDir.GetWideString());
				RtString sfnz = const_cast<char*>(shadPtrNZ);

				RiDeclare("__category", "uniform string");
				char* shadowatten = "__shadowatten";

				if (g_NamedHandles) { 
					RiLightSource(shadowpoint, "__handleid", &light.handle, "intensity", &light.intensity,  "lightcolor", light.color,
						"shadowcolor", light.shadowcolor, "falloffexponent", &light.falloffExponent, "__nondiffuse", &light.nondiffuse, "__nonspecular", 
						&light.nonspecular, "sfpx", &sfpx, "sfnx", &sfnx, "sfpy", &sfpy, "sfny", &sfny, "sfpz", 
						&sfpz, "sfnz", &sfnz,  "shadowsamples", &light.shadowMapSamp, "shadowbias", &light.shadowMapBias, 
						"shadowblur", &light.shadowMapBlur, "__category", &shadowatten, RI_NULL);
				}
				else {
					light.handle = RiLightSource(shadowpoint, "intensity", &light.intensity,  "lightcolor", light.color,
						"shadowcolor", light.shadowcolor, "falloffexponent", &light.falloffExponent, "__nondiffuse", &light.nondiffuse, "__nonspecular", 
						&light.nonspecular, "sfpx", &sfpx, "sfnx", &sfnx, "sfpy", &sfpy, "sfny", &sfny, "sfpz", 
						&sfpz, "sfnz", &sfnz,  "shadowsamples", &light.shadowMapSamp, "shadowbias", &light.shadowMapBias, 
						"shadowblur", &light.shadowMapBlur, "__category", &shadowatten, RI_NULL);
				}
			}
			break;
		case SpotLight: 
			RiDeclare("intensity", "uniform float");
			RiDeclare("lightcolor", "uniform color");
			RiDeclare("coneangle", "uniform float");
			RiDeclare("conedeltaangle", "uniform float");
			RiDeclare("beamdistribution", "uniform float");
			RiDeclare("falloffexponent", "uniform float");
			RiDeclare("__nondiffuse", "uniform float");
			RiDeclare("__nonspecular", "uniform float");

			if (!light.shadow) {
				if (g_NamedHandles) {
					RiLightSource(spotlight, "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", 
						light.color, "coneangle", &light.coneAngle, "conedeltaangle", &light.coneDeltaAngle, 
						"beamdistribution", &light.beamDistribution, "falloffexponent", &light.falloffExponent, 
						"__nondiffuse", &light.nondiffuse, "__nonspecular", &light.nonspecular, RI_NULL);
				}
				else {
					light.handle = RiLightSource(spotlight, "intensity", &light.intensity, "lightcolor", 
						light.color, "coneangle", &light.coneAngle, "conedeltaangle", &light.coneDeltaAngle, 
						"beamdistribution", &light.beamDistribution, "falloffexponent", &light.falloffExponent, 
						"__nondiffuse", &light.nondiffuse, "__nonspecular", &light.nonspecular, RI_NULL);
				}
			}
			else {
				RiDeclare("shadowcolor", "uniform color");
				RiDeclare("shadowsamples", "uniform float");
				RiDeclare("shadowbias", "uniform float");
				RiDeclare("shadowblur", "uniform float");
				// get shadow name
				char* shadPtr = NULL;
				XSIW2A(&shadPtr, light.shadowName.GetWideString());
				RiDeclare("shadowname", "uniform string");
				RtString shadowname = const_cast<char*>(shadPtr);
				RiDeclare("__category", "uniform string");
				char* shadowatten = "__shadowatten";

				if (g_NamedHandles) {
					RiLightSource(shadowspot, "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", 
						light.color, "shadowcolor", light.shadowcolor, "coneangle", &light.coneAngle, "conedeltaangle", &light.coneDeltaAngle, 
						"beamdistribution", &light.beamDistribution, "falloffexponent", &light.falloffExponent, 
						"shadowname", &shadowname, "__nondiffuse", &light.nondiffuse, "__nonspecular", &light.nonspecular, 
						"shadowsamples", &light.shadowMapSamp, "shadowbias", &light.shadowMapBias,  
						"shadowblur", &light.shadowMapBlur, "__category", &shadowatten, RI_NULL);
				}
				else {
					light.handle = RiLightSource(shadowspot, "intensity", &light.intensity, "lightcolor", 
						light.color, "shadowcolor", light.shadowcolor, "coneangle", &light.coneAngle, "conedeltaangle", &light.coneDeltaAngle, 
						"beamdistribution", &light.beamDistribution, "falloffexponent", &light.falloffExponent, 
						"shadowname", &shadowname, "__nondiffuse", &light.nondiffuse, "__nonspecular", &light.nonspecular, 
						"shadowsamples", &light.shadowMapSamp, "shadowbias", &light.shadowMapBias,  
						"shadowblur", &light.shadowMapBlur, "__category", &shadowatten, RI_NULL);
				}
			}
			break;
		case AreaLight:
			switch((int)light.areaLightType) {	
				float radius, zmin, zmax;

				case RectangleAreaLight:		
					RiDeclare("intensity", "uniform float");
					RiDeclare("lightcolor", "uniform color");
					RiDeclare("falloff", "uniform float");
					RiDeclare("samples", "uniform float"); 
					RiDeclare("__nonspecular", "uniform float");
					RiDeclare("xsize", "uniform float"); 
					RiDeclare("ysize", "uniform float"); 

					if (g_NamedHandles) {
						RiLightSource("rectanglelight", "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples, "__nonspecular", 
							&light.nonspecular, "xsize", &light.areaLightSizeX, "ysize", &light.areaLightSizeY,  RI_NULL);
					}
					else {
						light.handle = RiLightSource("rectanglelight", "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples, "__nonspecular", 
							&light.nonspecular, "xsize", &light.areaLightSizeX, "ysize", &light.areaLightSizeY,  RI_NULL);
					}
					break;
				case DiskAreaLight:		
					RiDeclare("intensity", "uniform float");
					RiDeclare("lightcolor", "uniform color");
					RiDeclare("falloff", "uniform float");
					RiDeclare("samples", "uniform float"); 
					RiDeclare("__nonspecular", "uniform float");
					RiDeclare("radius", "uniform float"); 

					radius = sqrt(light.areaLightSizeX*light.areaLightSizeX + light.areaLightSizeY*light.areaLightSizeY +
						light.areaLightSizeZ*light.areaLightSizeZ);
					
					if (g_NamedHandles) {
						RiLightSource("disklight", "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples, "radius", &radius, 
							"__nonspecular", &light.nonspecular, RI_NULL);
					}
					else {
						light.handle = RiLightSource("disklight", "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples, "radius", &radius, 
							"__nonspecular", &light.nonspecular, RI_NULL);
					}
					break;
				case SphereAreaLight: 		
					RiDeclare("intensity", "uniform float");
					RiDeclare("lightcolor", "uniform color");
					RiDeclare("falloff", "uniform float");
					RiDeclare("samples", "uniform float"); 
					RiDeclare("__nonspecular", "uniform float");
					RiDeclare("radius", "uniform float"); 	

					radius = sqrt(light.areaLightSizeX*light.areaLightSizeX + light.areaLightSizeY*light.areaLightSizeY +
						light.areaLightSizeZ*light.areaLightSizeZ);
					
					if (g_NamedHandles) {
						RiLightSource("spherelight", "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples,  "radius", &radius,
							"__nonspecular", &light.nonspecular, RI_NULL);
					}
					else {
						light.handle = RiLightSource("spherelight", "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples,  "radius", &radius,
							"__nonspecular", &light.nonspecular, RI_NULL);
					}
					break;
				case CylinderAreaLight: 		
					RiDeclare("intensity", "uniform float");
					RiDeclare("lightcolor", "uniform color");
					RiDeclare("falloff", "uniform float");
					RiDeclare("samples", "uniform float"); 
					RiDeclare("__nonspecular", "uniform float");
					RiDeclare("radius", "uniform float"); 
					RiDeclare("zmin", "uniform float"); 
					RiDeclare("zmax", "uniform float");
					zmin = -light.areaLightSizeZ;
					zmax = light.areaLightSizeZ;

					if (g_NamedHandles) {
						RiLightSource("cylinderlight", "__handleid", &light.handle, "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples, "__nonspecular", 
							&light.nonspecular, "zmin", &zmin, "zmax", &zmax, RI_NULL);
					}
					else {
						light.handle = RiLightSource("cylinderlight", "intensity", &light.intensity, "lightcolor", light.color,
							"falloff", &light.falloffExponent, "samples", &light.areaLightSamples, "__nonspecular", 
							&light.nonspecular, "zmin", &zmin, "zmax", &zmax, RI_NULL);
					}
					break;
			}
			break;
		case RManLight: {
			if (g_NamedHandles) {
				xsiRibTokens tokenDataString;
				tokenDataString.put("__handleid", StringType, false, false, false, 0);
				tokenDataString.putDetailType(UniformDetail);
				int strlen = handleName.Length();
				tokenDataString.assignString((const char*)light.handle, strlen);
				ribTokens.push_back(tokenDataString);
			}

			unsigned numTokens = ribTokens.size();
			RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
			RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
			assignRibTokensV(&ribTokens, tokenArray, pointerArray);
			char* shadername = NULL;
			XSIW2A(&shadername, light.shaderName.GetWideString());

			if (g_NamedHandles) {
				RiLightSourceV(shadername, numTokens, tokenArray, pointerArray);
			}
			else {
				light.handle = RiLightSourceV(shadername, numTokens, tokenArray, pointerArray);
			}
			break;
		}
		case UnknownLight:
			break;
	}

	RiTransformEnd();
}

ObjectType xsiRibLightData::type() const
{
	return LightObject;
}

CString xsiRibLightData::constructFileName(bool ribFile, jobStruct* myjob)
{
	// frame padding
	wchar_t padframe[10];
    swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);
	if (g_CurrentFrame < 0) {
		long currentFrame = abs((long)g_CurrentFrame);
		swprintf(padframe, L"-%0*ld", g_FramePadding, currentFrame);
	}

	CString fileExt, filePath, fileNameStem, jobName;
	if (ribFile) {
		fileExt = L".rib";
		filePath = g_RibPath;
		fileNameStem = myjob->ribFileName;
		if (myjob->jobType == BeautyJob || myjob->jobType == ShadowJob) jobName = myjob->camera[0].name;
		else if (myjob->jobType == ArchiveJob || myjob->jobType == EnvmapJob) jobName = myjob->name;
	}
	else {
		fileExt = myjob->fileExt;
		if (myjob->jobType == ShadowJob) filePath = g_ShdPath;
		else if (myjob->jobType == BeautyJob) filePath = g_PixPath;
		else if (myjob->jobType == EnvmapJob) filePath = g_TexPath;
		fileNameStem = myjob->imageName;
		if (myjob->jobType == BeautyJob || myjob->jobType == ShadowJob) jobName = myjob->camera[0].name;
		else if (myjob->jobType == EnvmapJob) jobName = myjob->name;
	}

	CString jobSuffix = L"";
	if (myjob->jobType == ShadowJob) jobSuffix = L".SHD";
	if (myjob->jobType == EnvmapJob) jobSuffix = L".ENV";

	// construct output image file name 
	CString fileName;
	if (myjob->jobType == ShadowJob || myjob->jobType == EnvmapJob) {
		if (myjob->isPointLight || myjob->isEnvmap) {
			if (myjob->cubeMapDir == PX) {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".PX" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".PX" + jobSuffix + fileExt;
			}
			else if (myjob->cubeMapDir == NX) {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".NX" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".NX" + jobSuffix + fileExt;
			}
			else if (myjob->cubeMapDir == PY) {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".PY" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".PY" + jobSuffix + fileExt;
			}
			else if (myjob->cubeMapDir == NY) {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".NY" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".NY" + jobSuffix + fileExt;
			}
			else if (myjob->cubeMapDir == PZ) {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".PZ" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".PZ" + jobSuffix + fileExt;
			}
			else if (myjob->cubeMapDir == NZ) {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".NZ" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".NZ" + jobSuffix + fileExt;
			}
			else {
				if (myjob->writeFrequency == L"Every Frame")
					fileName = filePath + fileNameStem + L"." + jobName + L".PX" + jobSuffix + L"." + padframe + fileExt;
				else if (myjob->writeFrequency == L"Once Per Job")
					fileName = filePath + fileNameStem + L"." + jobName + L".PX" + jobSuffix + fileExt;
			}
		}
		else {
			if (myjob->writeFrequency == L"Every Frame") 
				fileName = filePath + fileNameStem + L"." + jobName + L"" + jobSuffix + L"." + padframe + fileExt;
			else if (myjob->writeFrequency == L"Once Per Job") 
				fileName = filePath + fileNameStem + L"." + jobName + L"" + jobSuffix + fileExt;
		}
	}
	else {
		if (myjob->writeFrequency == L"Every Frame") 
			fileName = filePath + fileNameStem + L"." + jobName + L"." + padframe + fileExt;
		else if (myjob->writeFrequency == L"Once Per Job") 
			fileName = filePath + fileNameStem + L"." + jobName + fileExt;
	}

	return fileName;
}

CString xsiRibLightData::subGlobalVars(CString oldString)
{
	CString newString = oldString;

	// we also replace # & $F4 with padded numbers and @ & $F with unpadded numbers
	wchar_t padframe[10];
	if (stringHasChar(newString, L"#")) {
		swprintf(padframe, L"%0*ld", 4, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"#", CString(padframe));
	}
	else if (stringHasChar(newString, L"@")) {
		swprintf(padframe, L"%0*ld", 1, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"@", CString(padframe));
	}
	if (stringHasChar(newString, L"$F4")) {
		swprintf(padframe, L"%0*ld", 4, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"$F4", CString(padframe));
	}
	else if (stringHasChar(newString, L"$F")) {
		swprintf(padframe, L"%0*ld", 1, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"$F", CString(padframe));
	}							
						
	// check for additional keywords ($PROJ, $TEX, $SHD, $PIX, $RIB)
	if (stringHasChar(newString, L"$PROJ")) {
		Application app;
		CString projPath = app.GetActiveProject().GetFullName(); 
		newString = replaceSubString(newString, L"$PROJ", projPath);
	}
		
	if (stringHasChar(newString, L"$TEX")) {
		newString = replaceSubString(newString, L"$TEX", g_TexPath);
	}
														
	if (stringHasChar(newString, L"$SHD")) {
		newString = replaceSubString(newString, L"$SHD", g_ShdPath);
	}
														
	if (stringHasChar(newString, L"$PIX")) {
		newString = replaceSubString(newString, L"$PIX", g_PixPath);
	}
														
	if (stringHasChar(newString, L"$RIB")) {
		newString = replaceSubString(newString, L"$RIB", g_RibPath);
	}

	return newString;
}
