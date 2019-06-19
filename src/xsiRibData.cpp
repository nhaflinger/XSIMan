//*****************************************************************************
// File: xsiRibData.cpp
// Description: xsiRibData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibData.h"
#include "xsiUtilities.h"

using namespace std;


xsiRibData::xsiRibData()
{
}

xsiRibData::~xsiRibData()
{
	vector<xsiRibTokens>::iterator iter = ribTokens.begin();
	while (iter != ribTokens.end()) {
		if ((*iter).tokenFloats != NULL) {
			delete (*iter).tokenFloats;
			(*iter).tokenFloats = NULL;
		}
		if ((*iter).tokenString != NULL) {
			delete (*iter).tokenString;
			(*iter).tokenString = NULL;
		}

		++iter;
	}

	ribTokens.clear();
}

void xsiRibData::write()
{
}

void xsiRibData::addPrimvar(ParameterType paramType, UserDataMap primvarMap)
{
	Application app;
	CString udmname = primvarMap.GetName();
	long slen = udmname.Length();
	CString pvname = subString(udmname, 5, slen-1);

	long numPrimvar = 0;

	if (primvarMap.IsValid()) {
		numPrimvar = primvarMap.GetCount();

		xsiRibTokens primvarTokenData;
		char* primvar = NULL;
		XSIW2A(&primvar, pvname.GetWideString());

		// get actual number of non-empty UDM values
		long numpnts = 0;
		for (long i = 0 ; i < numPrimvar; i++) {
			if (!primvarMap.IsItemEmpty(i)) numpnts += 1;
		}

		bool isNurbsType = (type() == NurbsObject || type() == CurveObject) ? true : false;
		bool isMeshType = (type() == MeshObject || type() == SubdivisionObject) ? true : false;
		bool notConstantDetail = (numpnts == 1) ? false : true;

		// need to repeat first point for curves
		long offset = 0;
		if (type() == CurveObject) offset = 1;

		// if curve is not closed then repeat end value
		long tail = 0;
		if (type() == CurveObject && !isUClosed()) tail = 1;

		if (paramType == PointType ) {
			primvarTokenData.put(primvar, PointType, isNurbsType, notConstantDetail, false, numpnts+offset+tail);
			if (notConstantDetail) primvarTokenData.putDetailType(VertexDetail);
			else primvarTokenData.putDetailType(ConstantDetail);
		}
		else if (paramType == NormalType ) {
			primvarTokenData.put(primvar, NormalType, isNurbsType, notConstantDetail, false, numpnts+offset+tail);
			if (notConstantDetail) primvarTokenData.putDetailType(VertexDetail);
			else primvarTokenData.putDetailType(ConstantDetail);
		}
		else if (paramType == VectorType ) {
			primvarTokenData.put(primvar, VectorType, isNurbsType, notConstantDetail, false, numpnts+offset+tail);
			if (notConstantDetail) primvarTokenData.putDetailType(VertexDetail);
			else primvarTokenData.putDetailType(ConstantDetail);
		}
		else if (paramType == ColorType ) {
			primvarTokenData.put(primvar, ColorType, isNurbsType, notConstantDetail, false, numpnts+offset+tail);
			if (notConstantDetail) primvarTokenData.putDetailType(VertexDetail);
			else primvarTokenData.putDetailType(ConstantDetail);
		}
		else if (paramType == FloatType ) {
			primvarTokenData.put(primvar, FloatType, isNurbsType, notConstantDetail, false, numpnts+offset+tail);
			if (type() == CurveObject && pvname == L"width" && notConstantDetail) primvarTokenData.putDetailType(VaryingDetail);
			//else if (isMeshType && notConstantDetail) primvarTokenData.putDetailType(FaceVaryingDetail);
			else if (notConstantDetail) primvarTokenData.putDetailType(VertexDetail);
			else primvarTokenData.putDetailType(ConstantDetail);
		}
		else if (paramType == StringType ) {
			primvarTokenData.put(primvar, StringType, isNurbsType, false, false, 0);
			primvarTokenData.putDetailType(ConstantDetail);
		}

		for (long i = 0 ; i < numpnts; i++) {
			unsigned char* pInternalData = NULL;
			unsigned int cntData = 0;
			primvarMap.GetItemValue(i, pInternalData, cntData);

			if (paramType == FloatType) {
				wchar_t* pData;
				pData = (wchar_t*)pInternalData;
				float fData = (float)_wtof(pData);
				primvarTokenData.assignFloat(i+offset, fData);

				// repeat first point for curves
				if (i == 0 && type() == CurveObject) 
					primvarTokenData.assignFloat(0, fData);

				// repeat last point for closed curves
				if (i == numpnts-1 && type() == CurveObject && tail) 
					primvarTokenData.assignFloat(numpnts+offset+tail-1, fData);
			}
			else if (paramType == StringType) {
				wchar_t* pData;
				pData = (wchar_t*)pInternalData;
				char* cData = NULL;
				XSIW2A(&cData, pData);
				slen = strlen(cData);
				primvarTokenData.assignString(cData, slen);
			}
			else if (paramType == ColorType) {
				struct dataStruct {
					float r;
					float g;
					float b;
					float a;
				};
				dataStruct* pData;
				pData = (dataStruct*)pInternalData;
				primvarTokenData.assignFloat(i+offset, (float)pData->r, (float)pData->g, (float)pData->b, (float)pData->a);

				// repeat first point for curves
				if (i == 0 && type() == CurveObject) 
					primvarTokenData.assignFloat(0, (float)pData->r, (float)pData->g, (float)pData->b, (float)pData->a);
				
				// repeat last point for closed curves
				if (i == numpnts-1 && type() == CurveObject && tail) 
					primvarTokenData.assignFloat(numpnts+offset+tail-1, (float)pData->r, (float)pData->g, (float)pData->b, (float)pData->a);
			}
			else  {
				struct dataStruct {
					float x;
					float y;
					float z;
				};
				dataStruct* pData;
				pData = (dataStruct*)pInternalData;
				if (isNurbsType) primvarTokenData.assignFloat(i+offset, (float)pData->x, (float)pData->y, (float)pData->z, 1);
				else primvarTokenData.assignFloat(i+offset, (float)pData->x, (float)pData->y, (float)pData->z);

				// repeat first point for curves
				if (i == 0 && type() == CurveObject) {
					if (isNurbsType) primvarTokenData.assignFloat(0, (float)pData->x, (float)pData->y, (float)pData->z, 1);
					else primvarTokenData.assignFloat(0, (float)pData->x, (float)pData->y, (float)pData->z);
				}

				// repeat last point for closed curves
				if (i == numpnts-1 && type() == CurveObject && tail) {
					if (isNurbsType) primvarTokenData.assignFloat(numpnts+offset+tail-1, (float)pData->x, (float)pData->y, (float)pData->z, 1);
					else primvarTokenData.assignFloat(numpnts+offset+tail-1, (float)pData->x, (float)pData->y, (float)pData->z);

				}
			}
		}
		
		// add tokens
		ribTokens.push_back(primvarTokenData);
	}
}

