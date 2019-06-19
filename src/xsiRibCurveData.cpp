//*****************************************************************************
// File: xsiRibCurveData.cpp
// Description: xsiRibCurveData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibCurveData.h"
#include "xsiUtilities.h"

extern CString g_Renderer;


xsiRibCurveData::xsiRibCurveData(X3DObject in_obj, float cframe, long curvenum)
{
	Geometry geom(in_obj.GetActivePrimitive().GetGeometry(cframe));
	NurbsCurveList nurbscurvelist(geom);

	CNurbsCurveDataArray nurbsCurveDataArray;
	nurbscurvelist.Get(siIGESNurbs, nurbsCurveDataArray);

	CRef propRef = in_obj.GetProperties().GetItem(L"XSIManAirOptions");
	flatness = 0.5;
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter flatnessp = prop.GetParameters().GetItem(L"flatness");
		flatness = flatnessp.GetValue();
		Parameter bevelcurvesp = prop.GetParameters().GetItem(L"bevelcurves");
		bevelcurves = bevelcurvesp.GetValue();
	}

	//ncurves = nurbsCurveDataArray.GetCount();
	ncurves = 1;
	nverts = new RtInt;
		
	long degree = nurbsCurveDataArray[curvenum].m_lDegree;
	uClosed = nurbsCurveDataArray[curvenum].m_bClosed == true? SI_TRUE : SI_FALSE;

	long closedpad;

	// need to double points on end if not closed
	if (!uClosed) closedpad = 4;
	else closedpad = 0;

	nverts[0] = nurbsCurveDataArray[curvenum].m_aControlPoints.GetCount() + closedpad;
	cv = new RtFloat[(nverts[0] * 3)];
	
	unsigned k;
	if (!uClosed) k = 6;
	else k = 0;
	for (long j = 0; j < nverts[0]-closedpad; j++) {
		cv[k] = (RtFloat)nurbsCurveDataArray[curvenum].m_aControlPoints[j].GetX();
		cv[k+1] = (RtFloat)nurbsCurveDataArray[curvenum].m_aControlPoints[j].GetY();
		cv[k+2] = (RtFloat)nurbsCurveDataArray[curvenum].m_aControlPoints[j].GetZ();
		k += 3;
	}

	// need to double up points on ends
	if (!uClosed) {
		cv[0] = cv[6];
		cv[1] = cv[7];
		cv[2] = cv[8];
		cv[3] = cv[6];
		cv[4] = cv[7];
		cv[5] = cv[8];
		cv[k] = cv[k-3];
		cv[k+1] = cv[k-2];
		cv[k+2] = cv[k-1];
		cv[k+3] = cv[k-3];
		cv[k+4] = cv[k-2];
		cv[k+5] = cv[k-1];
	}

	xsiRibTokens tokenData;

	tokenData.put("P", PointType, false, true, false, nverts[0]);
	tokenData.putDetailType(VertexDetail);
	tokenData.assignFloats(cv);
	ribTokens.push_back(tokenData);

	float curveWidth = 0.0f;
	propRef = in_obj.GetProperties().GetItem(L"XSIManWidth");
	if (propRef.IsValid()) {
		Property prop(propRef);
		Parameter width = prop.GetParameters().GetItem(L"width");
		curveWidth = (float)width.GetValue();

		xsiRibTokens* tokenDataWidth = NULL;
		tokenDataWidth = new xsiRibTokens;
		tokenDataWidth->put("constantwidth", FloatType, false, false, false, 0);
		tokenDataWidth->putDetailType(ConstantDetail);
		tokenDataWidth->assignFloat(0, curveWidth);
		ribTokens.push_back(*tokenDataWidth);
		delete tokenDataWidth;
	}
		
	// check for primitive variables
	CRefArray clsRefArray = geom.GetClusters();
	for (long j = 0; j < clsRefArray.GetCount(); ++j) {
		Cluster cls(clsRefArray[j]);
		CRefArray propRefArray = cls.GetProperties();
		for (long k = 0; k < propRefArray.GetCount(); ++k) {
			UserDataMap primvarUDM(propRefArray[k]);
			CString namestem = subString(primvarUDM.GetName(), 0, 4);
			if (namestem == L"rmanP") addPrimvar(PointType, primvarUDM);
			else if (namestem == L"rmanV") addPrimvar(VectorType, primvarUDM);
			else if (namestem == L"rmanN") addPrimvar(NormalType, primvarUDM);
			else if (namestem == L"rmanC") addPrimvar(ColorType, primvarUDM);
			else if (namestem == L"rmanF") addPrimvar(FloatType, primvarUDM);
			else if (namestem == L"rmanS") addPrimvar(StringType, primvarUDM);
		}
	}
}

xsiRibCurveData::~xsiRibCurveData()
{
	if (nverts != NULL) {delete nverts; nverts = NULL;}
	if (cv != NULL) {delete cv; cv = NULL;}
}

void xsiRibCurveData::write()
{
	unsigned numTokens = ribTokens.size();
	RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
	RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
	assignRibTokensV(&ribTokens, tokenArray, pointerArray);

	if (g_Renderer == L"Air" && bevelcurves == true)
		RiArchiveRecord(RI_VERBATIM, "Attribute \"render\" \"integer bevelcurves\" [1]\n");
	if (g_Renderer == L"Air" && flatness != 0.5)
		RiArchiveRecord(RI_VERBATIM, "GeometricApproximation \"flatness\" [%f]\n", flatness);

	RiCurvesV("cubic", ncurves, nverts, "nonperiodic", numTokens, tokenArray, pointerArray);
}

ObjectType xsiRibCurveData::type() const
{
	return CurveObject;
}
