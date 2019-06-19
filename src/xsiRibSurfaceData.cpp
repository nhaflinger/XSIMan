//*****************************************************************************
// File: xsiRibSurfaceData.cpp
// Description: xsiRibSurfaceData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibSurfaceData.h"
#include "xsiUtilities.h" 
#include <xsi_material.h>
#include <xsi_texture.h>
#include <xsi_nurbssample.h>
#include <xsi_nurbssurface.h>
#include <xsi_point.h>


xsiRibSurfaceData::xsiRibSurfaceData(X3DObject in_obj, float cframe)
{
	Application app;
	Geometry geom(in_obj.GetActivePrimitive().GetGeometry(cframe));
	NurbsSurfaceMesh nurbssurfacemesh(geom);

	CNurbsSurfaceDataArray nurbsSurfaceDataArray;
	nurbssurfacemesh.Get(siIGESNurbs, nurbsSurfaceDataArray);

	cv = NULL;
	uknot = NULL;
	vknot = NULL;
	tcurves = NULL;
	torder = NULL;
	tn = NULL;
	tu = NULL;
	tv = NULL;
	tw = NULL;
	tknot = NULL;
	tmin = NULL;
	tmax = NULL;
	hasTrims = false;

	int uKnotScale, vKnotScale;
	for (long i = 0; i < nurbsSurfaceDataArray.GetCount(); ++i) {
		uorder = nurbsSurfaceDataArray[i].m_lUDegree + 1;
		vorder = nurbsSurfaceDataArray[i].m_lVDegree + 1;
		umin = 0; umax = 1; vmin = 0; vmax = 1;
		nu = nurbsSurfaceDataArray[i].m_lNbUControlPoints;
		nv = nurbsSurfaceDataArray[i].m_lNbVControlPoints;
		unsigned uKnotLength = nurbsSurfaceDataArray[i].m_aUKnots.GetCount();
		unsigned vKnotLength = nurbsSurfaceDataArray[i].m_aVKnots.GetCount();
		uClosed = nurbsSurfaceDataArray[i].m_bUClosed;
		vClosed = nurbsSurfaceDataArray[i].m_bVClosed;

		cv = new RtFloat[nu*nv*4];
		uknot = new RtFloat[uKnotLength];
		vknot = new RtFloat[vKnotLength];

		uKnotScale = nu + uorder + 1;
		vKnotScale = nv + vorder + 1;

		for (unsigned j = 0; j < uKnotLength; j++) {
			uknot[j] = (RtFloat)nurbsSurfaceDataArray[i].m_aUKnots[j] / uKnotScale;
		}
		umin = uknot[uorder-1]; 
		umax = uknot[uKnotLength-uorder];
		
		for (unsigned j = 0; j < vKnotLength; j++) {
			vknot[j] = (RtFloat)nurbsSurfaceDataArray[i].m_aVKnots[j] / vKnotScale;
		}
		vmin = vknot[vorder-1]; 
		vmax = vknot[vKnotLength-vorder];

		unsigned k = 0;
		for (j = 0; j < (unsigned)(nu * nv); j++) {
			cv[k] = (RtFloat)nurbsSurfaceDataArray[i].m_aControlPoints[j].GetX();
			cv[k+1] = (RtFloat)nurbsSurfaceDataArray[i].m_aControlPoints[j].GetY();
			cv[k+2] = (RtFloat)nurbsSurfaceDataArray[i].m_aControlPoints[j].GetZ();
			cv[k+3] = (RtFloat)nurbsSurfaceDataArray[i].m_aControlPoints[j].GetW();
			k += 4;
		}

		xsiRibTokens tokenData;
		tokenData.put("Pw", PointType, true, true, false, nu*nv);
		tokenData.putDetailType(VertexDetail);
		tokenData.assignFloats(cv);
		ribTokens.push_back(tokenData);
	}

	// get trim curves
	CNurbsSurfaceRefArray nurbsSurfArray = nurbssurfacemesh.GetSurfaces();
	NurbsSurface nurbssurface(nurbsSurfArray[0]);

	// get total number of curves
	long numTrimCurves = 0;
	long numKnotValues = 0;
	long numVerts = 0;
	tloops = nurbssurface.GetTrimCount();
	for (long l = 0; l < tloops; ++l) {
		CTrimCurveData trimCurve;
		CStatus stat = nurbssurface.GetTrim(l, siIGESNurbs, trimCurve);
		CNurbsCurveDataArray nurbsCurveDataArray = trimCurve.m_aCurvesData;
		numTrimCurves += nurbsCurveDataArray.GetCount();
		for (long i = 0; i < nurbsCurveDataArray.GetCount(); ++i) {
			numKnotValues += nurbsCurveDataArray[i].m_aKnots.GetCount();
			numVerts += nurbsCurveDataArray[i].m_aControlPoints.GetCount();
		}
	}
	
	if (tloops > 0) {
		hasTrims = true;
		tcurves = new RtInt[tloops];
		torder = new RtInt[numTrimCurves];
		tn = new RtInt[numTrimCurves];
		tu = new RtFloat[numVerts];
		tv = new RtFloat[numVerts];
		tw = new RtFloat[numVerts];
		tknot = new RtFloat[numKnotValues];
		tmin = new RtFloat[numTrimCurves];
		tmax = new RtFloat[numTrimCurves];
	}

	long curveIndex = 0;
	long knotIndex = 0;
	long vertexIndex = 0;
	for (long l = 0; l < tloops; ++l) {
		CTrimCurveData trimCurve;
		CStatus stat = nurbssurface.GetTrim(l, siIGESNurbs, trimCurve);
		bool isBoundary = trimCurve.m_bIsBoundary;
		long projType = trimCurve.m_lProjectionType;
		CNurbsCurveDataArray nurbsCurveDataArray = trimCurve.m_aCurvesData;

		tcurves[l] = nurbsCurveDataArray.GetCount();

		for (long i = 0; i < tcurves[l]; ++i) {		
			torder[curveIndex] = nurbsCurveDataArray[i].m_lDegree + 1; 

			int nverts = nurbsCurveDataArray[i].m_aControlPoints.GetCount();
			tn[curveIndex] = nverts;

			tmin[curveIndex] = (float)nurbsCurveDataArray[i].m_aKnots[0]; 
			tmax[curveIndex] = (float)nurbsCurveDataArray[i].m_aKnots[0];
			for (long j = 0; j < nurbsCurveDataArray[i].m_aKnots.GetCount(); ++j) {
				tknot[knotIndex] = (float)nurbsCurveDataArray[i].m_aKnots[j];
				if (tknot[knotIndex] <= tmin[curveIndex]) tmin[curveIndex] = tknot[knotIndex];
				if (tknot[knotIndex] >= tmax[curveIndex]) tmax[curveIndex] = tknot[knotIndex];
				knotIndex += 1;
			}

			double du, dv, dw;
			for (long j = 0; j < nverts; j++) {
				long k = j;
				if (isBoundary) k = nverts - j - 1;
				du = nurbsCurveDataArray[i].m_aControlPoints[k].GetX();
				dv = nurbsCurveDataArray[i].m_aControlPoints[k].GetY();
				dw = nurbsCurveDataArray[i].m_aControlPoints[k].GetW();
				tu[vertexIndex] = (RtFloat)(du * dw) / uKnotScale;
				tv[vertexIndex] = (RtFloat)(dv * dw) / vKnotScale;
				tw[vertexIndex] = (RtFloat)dw;
				vertexIndex += 1;
			}

			curveIndex += 1;
		}
	}

	// get the UV cluster
	CRefArray clusterRefArray;
	nurbssurfacemesh.GetClusters().Filter(siSampledPointCluster, CStringArray(), L"", clusterRefArray);
	Cluster	samplePointClusterUV;
	CRefArray uvClusterPropertiesRefArray;
	for (i = 0; i < clusterRefArray.GetCount(); i++) {
		Cluster cluster(clusterRefArray[i]);
		if(cluster.GetProperties().Filter(siClsUVSpaceTxtType, CStringArray(), L"", uvClusterPropertiesRefArray) == CStatus::OK) {
			samplePointClusterUV = cluster;	
			break;
		}
	}

	// get UVs from texture projection definition
	CDoubleArray uvValueArray;
	long lnbUV = 0;
	long numSTs = uvClusterPropertiesRefArray.GetCount();
	Material material = in_obj.GetMaterial();
	ClusterProperty uvProp(material.GetCurrentUV());
	CString currentUV = uvProp.GetName();
	Texture texture = material.GetCurrentTexture();
	CStatus st = texture.GetTransformValues(127, uvValueArray);
	lnbUV = uvValueArray.GetCount() / 3;
	
	// store additional UV sets here
	CDoubleArray stValueArray[16];
	CString uvSetName[16];

	int numSet = 0;
	for (i = 0;  i < numSTs; ++i) {
		ClusterProperty uvProp(uvClusterPropertiesRefArray[i]);
		CString uvPropName = uvProp.GetName();
		// get UVs directly from cluster if unable to access texture projection definition
		//if (uvPropName == currentUV && st == CStatus::Fail) {	
		if (i == 0 && st == CStatus::Fail) {
			CClusterPropertyElementArray uvElementArray = uvProp.GetElements();
			uvValueArray = uvElementArray.GetArray();
			lnbUV = uvValueArray.GetCount() / 3;
		}
		//else if (uvPropName != currentUV) {
		else {
			CClusterPropertyElementArray uvElementArray = uvProp.GetElements();
			if (numSet < 16) {
				stValueArray[numSet] = uvElementArray.GetArray();
				uvSetName[numSet] = uvPropName;
			}
			numSet += 1;
		}
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

	/*CNurbsSurfaceRefArray surfaces = nurbssurfacemesh.GetSurfaces();
	NurbsSurface nurbSurface = surfaces.GetItem(0);
	CNurbsSampleRefArray nurbSamples = nurbSurface.GetNurbsSamples();
	double dU,dV;
	for(long i = 0;i < nurbSamples.GetCount(); ++i) {
		NurbsSample sample(nurbSamples[i]);
		sample.GetUNormalized(dU);
		sample.GetVNormalized(dV);
	}*/
}

xsiRibSurfaceData::~xsiRibSurfaceData()
{
	if (cv != NULL) {delete cv; cv = NULL;}
	if (uknot != NULL) {delete uknot; uknot = NULL;}
	if (vknot != NULL) {delete vknot; vknot = NULL;}
	if (tcurves != NULL) {delete tcurves; tcurves = NULL;}
	if (torder != NULL) {delete torder; torder = NULL;}
	if (tn != NULL) {delete tn; tn = NULL;}
	if (tu != NULL) {delete tu; tu = NULL;}
	if (tv != NULL) {delete tv; tv = NULL;}
	if (tw != NULL) {delete tw; tw = NULL;}
	if (tknot != NULL) {delete tknot; tknot = NULL;}
	if (tmin != NULL) {delete tmin; tmin = NULL;}
	if (tmax != NULL) {delete tmax; tmax = NULL;}
}

void xsiRibSurfaceData::write()
{	
	unsigned numTokens = ribTokens.size();
	RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
	RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
	assignRibTokensV(&ribTokens, tokenArray, pointerArray);
	RiNuPatchV(nu, uorder, uknot, umin, umax, nv, vorder, vknot, vmin, vmax, numTokens, tokenArray, pointerArray);
}

ObjectType xsiRibSurfaceData::type() const
{
	return NurbsObject;
}

bool xsiRibSurfaceData::hasTrimCurves() const
{
	return hasTrims;
}

void xsiRibSurfaceData::writeTrims() const
{  
	RiTrimCurve(tloops, tcurves, torder, tknot, tmin, tmax, tn, tu, tv, tw);
}