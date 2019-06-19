//*****************************************************************************
// File: xsiRibHairData.cpp
// Description: xsiRibHairData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************


#include "xsiRibHairData.h"
#include "xsiUtilities.h"

extern CString g_Renderer;


xsiRibHairData::xsiRibHairData(X3DObject in_obj, float cframe)
{
	Application app;
	HairPrimitive hairPrim = in_obj.GetActivePrimitive();

	long totalHairs = hairPrim.GetParameter(L"TotalHairs").GetValue();
	double renderPercentage = hairPrim.GetParameter(L"RenderPercentage").GetValue();
	double strandMult = hairPrim.GetParameter(L"StrandMult").GetValue();
	long numTotalHairs;
	if (strandMult <= 1) {
		strandMult = 1;
		numTotalHairs = (long)(strandMult * totalHairs * renderPercentage/100. + 1);
	}
	else {
		numTotalHairs = (long)((long)(strandMult+0.5) * totalHairs * renderPercentage/100.);
	}
	long numChunkHairs = 10000;
	if (numChunkHairs > numTotalHairs) numChunkHairs = numTotalHairs;

	CRenderHairAccessor rha = hairPrim.GetRenderHairAccessor(numTotalHairs, numChunkHairs, cframe);

	long chunkSize = rha.GetRequestedChunkSize();
  	long hairCount = rha.GetRequestedHairCount();
	
	ncurves = numTotalHairs;
	nverts = new RtInt[numTotalHairs];
	 
	// assign memory
	long segments = hairPrim.GetParameterValue(CString(L"Segments"));
	long numverts = numTotalHairs * (segments + 4 + 1);
	long numrad = numTotalHairs * (segments + 2 + 1);	
	cv = new RtFloat[numverts * 3];
	width = new RtFloat[numrad];
	id = new RtFloat[numTotalHairs];
	sroot = new RtFloat[numTotalHairs];
	troot = new RtFloat[numTotalHairs]; 
	tval = new RtFloat[numrad];
	surf_normal = new RtFloat[numTotalHairs*3];

	// hair IDs
	for (long i=0; i<numTotalHairs; i++) {
		id[i] = (float)i;
	}

	// # of UV sets
	long nUVs = rha.GetUVCount();

	// set default value for UVs
	for (long i=0; i<numTotalHairs; i++) {
		sroot[i] = 0;
		troot[i] = 0;
	}

	// get the values in chunks
	long cnum = 0;
	long vnum = 0;
	long rnum = 0;
	long tnum = 0;
	long uvnum = 0;
	long snnum = 0;
	numTotalHairs = 0;
	while (rha.Next()) {

		// get the number of vertices for each render hair
		CLongArray verticesCountArray;
		rha.GetVerticesCount(verticesCountArray);
		
		// get actual hair count (there may be cut or density maps)
		chunkSize = rha.GetChunkHairCount();
		numTotalHairs += chunkSize;
		            
		CFloatArray posVals; CFloatArray radVals;
		rha.GetVertexPositions(posVals);
		rha.GetVertexRadiusValues(radVals);
		long k = 0; long l = 0;
		for (long i=0; i<verticesCountArray.GetCount(); i++ ) {
			// get the render hair positions
			long ncount = verticesCountArray[i];
			nverts[cnum] = ncount + 4;
			RtFloat* vertices = new RtFloat[nverts[cnum]*3];
			long nk = 6;
			bool badHairIndex = false;
			for (long j=0; j<ncount; j++) {
				vertices[nk] = posVals[k++];
				vertices[nk+1] = posVals[k++];
				vertices[nk+2] = posVals[k++];
				// XSI 5.1 sometimes returns corrupt data for hair on nurbs
				if (_isnan(vertices[nk])) {
					vertices[nk] = 0.0f;
					badHairIndex = true;
				}
				if (_isnan(vertices[nk+1])) {
					vertices[nk+1] = 0.0f;
					badHairIndex = true;
				}
				if (_isnan(vertices[nk+2])) {
					vertices[nk+2] = 0.0f;
					badHairIndex = true;
				}
				nk += 3;
			}
			
			// need to double up points on ends
			vertices[0] = vertices[6];
			vertices[1] = vertices[7];
			vertices[2] = vertices[8];
			vertices[3] = vertices[6];
			vertices[4] = vertices[7];
			vertices[5] = vertices[8];
			vertices[nk] = vertices[nk-3];
			vertices[nk+1] = vertices[nk-2];
			vertices[nk+2] = vertices[nk-1];
			vertices[nk+3] = vertices[nk-3];
			vertices[nk+4] = vertices[nk-2];
			vertices[nk+5] = vertices[nk-1];

			for (long j=0; j<nverts[cnum]*3; j++) {
				cv[vnum] = vertices[j];
				vnum++;
			}

			cnum++; 

			// cleanup
			delete vertices;

			// get the render hair radii
			RtFloat* radii = new RtFloat[ncount+2];
			long ni = 1;
			for (long j=0; j<ncount; j++) {
				radii[ni] = radVals[l++];
				if (badHairIndex == true) radii[ni] = 0.0f;
				ni++;
			}
						
			// need to double up points on ends
			radii[0] = radii[1];
			radii[ni] = radii[ni-1];
						
			for (long j=0; j<ncount+2; j++) {
				width[rnum] = radii[j];
				rnum++;
			}

			// cleanup
			delete radii;

			// set the "t" coordinates
			RtFloat* tcoord = new RtFloat[ncount+2];
			ni = 1;
			for (long j=0; j<ncount; j++) {
				tcoord[ni] = (float)j / (float)(ncount-1);
				ni++;
			}
						
			// need to double up points on ends
			tcoord[0] = tcoord[1];
			tcoord[ni] = tcoord[ni-1];
						
			for (long j=0; j<ncount+2; j++) {
				tval[tnum] = tcoord[j];
				tnum++;
			}

			// cleanup
			delete tcoord;
		}	
			
		         
		// get the root UV values
		if (nUVs > 0) {
			CFloatArray uvVals;
			long uvIndex = 0;
			rha.GetUVValues(uvIndex, uvVals);

			for (long j=0; j<uvVals.GetCount(); j+=3) {
				sroot[uvnum] = uvVals[j];
				troot[uvnum] = 1 - uvVals[j+1];
				uvnum++;
			}
		}
 
		// get the render hair surface normal values
		CFloatArray surfNormVals;
		rha.GetHairSurfaceNormalValues(surfNormVals);
		for (long i=0; i<surfNormVals.GetCount(); i+=3) {
			surf_normal[snnum++] = surfNormVals[i];
			surf_normal[snnum++] = surfNormVals[i+1];
			surf_normal[snnum++] = surfNormVals[i+2];
		}
	}
	
	// set actual hair count (there may be cut or density maps)
	ncurves = numTotalHairs;
	numverts = numTotalHairs * (segments + 4 + 1);
	numrad = numTotalHairs * (segments + 2 + 1);
		
	// assign token value pairs
	xsiRibTokens tokenData;
	tokenData.put("P", PointType, false, true, false, numverts);
	tokenData.putDetailType(VertexDetail);
	tokenData.assignFloats(cv);
	ribTokens.push_back(tokenData);
	
	xsiRibTokens tokenDataWidth;
	tokenDataWidth.put("width", FloatType, false, true, false, numrad);
	tokenDataWidth.putDetailType(VaryingDetail);
	tokenDataWidth.assignFloats(width);
	ribTokens.push_back(tokenDataWidth);
		
	xsiRibTokens tokenDataId;
	tokenDataId.put("hair_id", FloatType, false, true, false, numTotalHairs);
	tokenDataId.putDetailType(UniformDetail);
	tokenDataId.assignFloats(id);
	ribTokens.push_back(tokenDataId);
		
	xsiRibTokens tokenDataSroot;
	tokenDataSroot.put("s", FloatType, false, true, false, numTotalHairs);
	tokenDataSroot.putDetailType(UniformDetail);
	tokenDataSroot.assignFloats(sroot);
	ribTokens.push_back(tokenDataSroot);
	
	xsiRibTokens tokeDataTroot;
	tokeDataTroot.put("t", FloatType, false, true, false, numTotalHairs);
	tokeDataTroot.putDetailType(UniformDetail);
	tokeDataTroot.assignFloats(troot);
	ribTokens.push_back(tokeDataTroot);

	xsiRibTokens tokenDataTval;
	tokenDataTval.put("v_linear", FloatType, false, true, false, numrad);
	tokenDataTval.putDetailType(VaryingDetail);
	tokenDataTval.assignFloats(tval);
	ribTokens.push_back(tokenDataTval);
	
	xsiRibTokens tokenDataSurfNorm;
	tokenDataSurfNorm.put("surf_normal", NormalType, false, true, false, numTotalHairs);
	tokenDataSurfNorm.putDetailType(UniformDetail);
	tokenDataSurfNorm.assignFloats(surf_normal);
	ribTokens.push_back(tokenDataSurfNorm);

	// Air specific attributes
	CRef propRef = in_obj.GetProperties().GetItem(L"XSIManAirOptions");
	flatness = 0.5;
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter flatnessp = prop.GetParameters().GetItem(L"flatness");
		flatness = flatnessp.GetValue();
		Parameter bevelcurvesp = prop.GetParameters().GetItem(L"bevelcurves");
		bevelcurves = bevelcurvesp.GetValue();
	}
}

xsiRibHairData::~xsiRibHairData()
{
	if (nverts != NULL) {delete nverts; nverts = NULL;}
	if (cv != NULL) {delete cv; cv = NULL;}
	if (width != NULL) {delete width; width = NULL;}
	if (id != NULL) {delete id; id = NULL;}
	if (sroot!= NULL) {delete sroot; sroot = NULL;}
	if (troot!= NULL) {delete troot; troot = NULL;}
	if (tval!= NULL) {delete tval; tval = NULL;}
	if (surf_normal!= NULL) {delete surf_normal; surf_normal = NULL;}
}

void xsiRibHairData::write()
{
	if (g_Renderer == L"Air" && bevelcurves == true)
		RiArchiveRecord(RI_VERBATIM, "Attribute \"render\" \"integer bevelcurves\" [1]\n");
	if (g_Renderer == L"Air" && flatness != 0.5)
		RiArchiveRecord(RI_VERBATIM, "GeometricApproximation \"flatness\" [%f]\n", flatness);

	unsigned numTokens = ribTokens.size();
	RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
	RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
	assignRibTokensV(&ribTokens, tokenArray, pointerArray);

	RiCurvesV("cubic", ncurves, nverts, "nonperiodic", numTokens, tokenArray, pointerArray);
}

ObjectType xsiRibHairData::type() const
{
	return HairObject;
}
