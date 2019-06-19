//*****************************************************************************
// File: xsiRibMap.cpp
// Description: xsiRibMap object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibMap.h"

extern bool g_InstanceGeometry;
extern signed g_MaxMotionSamples;


xsiRibMap::xsiRibMap()
{
	// this might speed things up
	nodeMap.reserve(1000);
}

xsiRibMap::~xsiRibMap()
{
	nodeMap.clear();
}

void xsiRibMap::insert(SceneItem sceneItem, double cframe, int sample, ObjectType objectType, CString clusterName)
{
	CString objName = sceneItem.GetFullName();
	xsiRibNode node;
	nodeStruct nodeData;
	nodeData.objectType = objectType;
	nodeData.sceneItem = sceneItem;
	if (clusterName != L"") {
		nodeData.fullName = clusterName;
		nodeData.clusterName = clusterName;
	}
	else {
		nodeData.fullName = objName;
		nodeData.clusterName = L"";
	}
	nodeData.cframe = cframe;
	nodeData.sample = sample;
	nodeData.instanceGeometry = false;
	nodeData.generateEnvmap = false;
	for (long i=0; i<g_MaxMotionSamples; i++) 
		nodeData.handle[i] = NULL;
	nodeData.motionSamples = 1;

	if (g_InstanceGeometry == true) {
		nodeData.instanceGeometry = true;
		char* handle = NULL;
		XSIW2A(&handle, objName.GetWideString());
		for (long i=0; i<g_MaxMotionSamples; i++) 
			nodeData.handle[i] = handle;
	}

	CRefArray propRefArray = sceneItem.GetProperties();
	for (long i = 0; i < propRefArray.GetCount(); i++ ) {
		if (SIObject(propRefArray[i]).GetName() == L"Visibility") {
			Property prop = propRefArray[i];
			Parameter rendvis = prop.GetParameters().GetItem(L"rendvis");
			nodeData.renderVisibility = rendvis.GetValue();
			Parameter primray = prop.GetParameters().GetItem(L"primray");
			nodeData.primaryVisibility = primray.GetValue();
			Parameter scndray = prop.GetParameters().GetItem(L"scndray");
			nodeData.secondaryVisibility = scndray.GetValue();
			Parameter shdwcast = prop.GetParameters().GetItem(L"shdwcast");
			nodeData.shadowVisibility = shdwcast.GetValue();
		}
		else if (SIObject(propRefArray[i]).GetName() == L"XSIManCSGPrimitive") {
			Property prop = propRefArray[i];
			Parameter csgPrim = prop.GetParameters().GetItem(L"csgPrim");
			nodeData.instanceGeometry = csgPrim.GetValue();
		}
		else if (SIObject(propRefArray[i]).GetName() == L"XSIManSamples") {
			Property prop = propRefArray[i];
			Parameter samples = prop.GetParameters().GetItem(L"samples");
			nodeData.motionSamples = (long)samples.GetValue();
		}
		else if (SIObject(propRefArray[i]).GetName() == L"XSIManEnvmap") {
			Property prop = propRefArray[i];
			Parameter envmap = prop.GetParameters().GetItem(L"envmap");
			nodeData.generateEnvmap = envmap.GetValue();
		}
	}

	node.put(nodeData);

	nodeMap.push_back(node);
}

xsiRibNode* xsiRibMap::find(SceneItem sceneItem)
{
	CString objName = sceneItem.GetFullName();
	xsiRibNode* result;

	vector<xsiRibNode>::iterator iter;

	for(iter=nodeMap.begin(); iter!=nodeMap.end(); ++iter) {
		xsiRibNode ribNode = (*iter);
		
		nodeStruct node = ribNode.get();

		if (node.fullName == objName)  {
			result = &ribNode;
		}
	}

	return result;
}