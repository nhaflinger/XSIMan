//*****************************************************************************
// File: xsiRibTokens.cpp
// Description: xsiRibTokens object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibTokens.h"
#include "xsi_application.h"


const char* xsiRibTokens::strTokenDetailType[] = {"constant", "uniform", "varying", "vertex", "facevarying"};


xsiRibTokens::xsiRibTokens()
{
	tokenFloats = NULL;
	tokenFloatsSize = 0;
	tokenFloatsArraySize = 0;
	tokenString = NULL;
}

xsiRibTokens::~xsiRibTokens()
{
	// these lines crash XSI
	// anyways, this stuff is being cleaned up in the xsiRibData destructor
	//if (tokenFloats) {delete tokenFloats; tokenFloats = NULL;}
	//if (tokenString) {delete tokenString; tokenString = NULL;}
}

int xsiRibTokens::set(const char* name, ParameterType type, bool useNurbs, bool useArray, bool useUArray, unsigned int arraySize)
{
	strcpy(tokenName, name);
	tokenParameterType = type;
	isNurbs = useNurbs;
	isArray = useArray;
	isUArray = useUArray;
	tokenFloatsArraySize = arraySize;

	// cleanup from previous set
	if (tokenString) {delete tokenString; tokenString = NULL;}
	if (tokenFloats) {delete tokenFloats; tokenFloats = NULL;}

	switch(tokenParameterType) {
		case FloatType:
			tokenFloatsSize = 1;
			break;
		case PointType:
			if (isNurbs) tokenFloatsSize = 4;
			else tokenFloatsSize = 3;
			break;
		case NormalType:
		case VectorType:
		case ColorType:
			tokenFloatsSize = 3;
			break;
		case HpointType:
			tokenFloatsSize = 0;
			break;
	}

	// assign memory
	if (useArray || useUArray) {
		tokenFloats = new float[arraySize * tokenFloatsSize];
	}
	else {
		tokenFloats = new float[tokenFloatsSize];
	}

	return 1;
}

char* xsiRibTokens::getTokenName()
{
	return tokenName;
}

void xsiRibTokens::setDetailType(DetailType type) 
{
	tokenDetailType = type;
}

DetailType xsiRibTokens::getDetailType() 
{
	return tokenDetailType;
}

const float* xsiRibTokens::getTokenFloatArray( )
{
	return tokenFloats;
}

void xsiRibTokens::assignFloat(unsigned int i, float x)
{
	tokenFloats[i] = x;
}

void xsiRibTokens::assignFloat(unsigned int i, float x, float y , float z)
{
	tokenFloats[3*i + 0] = x;
	tokenFloats[3*i + 1] = y;
	tokenFloats[3*i + 2] = z;
}

void xsiRibTokens::assignFloat(unsigned int i, float x, float y , float z, float w)
{
	tokenFloats[4*i + 0] = x;
	tokenFloats[4*i + 1] = y;
	tokenFloats[4*i + 2] = z;
	tokenFloats[4*i + 3] = w;
}

void xsiRibTokens::assignFloats(const float* floats)
{
	if (isArray || isUArray) {
		memcpy(tokenFloats, floats, tokenFloatsArraySize * tokenFloatsSize * sizeof(float));
	}
	else {
		memcpy(tokenFloats, floats, tokenFloatsSize * sizeof(float));
	}
}

void xsiRibTokens::assignString(const char* chars, unsigned strlen)
{
	tokenString = new char[strlen + 1];
	strcpy(tokenString, chars);
}

void* xsiRibTokens::getRtPointer()
{
	void* retptr;

	switch(tokenParameterType) {
		case StringType:
			retptr = (void*)&tokenString;
			break;
		case FloatType:
		case PointType:
		case VectorType:
		case NormalType:
		case HpointType:
		case ColorType:
			retptr = (void*)tokenFloats;
			break;
	}
	return retptr;
}
	
void xsiRibTokens::getRiDeclare(char* declare)
{
	switch(tokenParameterType) {
		case StringType:
			sprintf(declare, "%s string", strTokenDetailType[tokenDetailType]);
			break;
		case FloatType:
			if (isUArray) sprintf(declare, "%s float[%d]", strTokenDetailType[tokenDetailType], tokenFloatsArraySize);
			else sprintf(declare, "%s float", strTokenDetailType[tokenDetailType]);
			break;
		case PointType:
			if (isNurbs) {
				if (isArray) sprintf(declare, "%s hpoint", strTokenDetailType[tokenDetailType]); 
				else sprintf(declare, "%s point", strTokenDetailType[tokenDetailType]);
			}
			else {
				sprintf(declare, "%s point", strTokenDetailType[tokenDetailType]);
			}
			break;
		case VectorType:
			sprintf(declare, "%s vector", strTokenDetailType[tokenDetailType]);
			break;
		case NormalType:
			sprintf(declare, "%s normal", strTokenDetailType[tokenDetailType]);
			break;
		case HpointType:
			break;
		case ColorType:
			sprintf(declare, "%s color", strTokenDetailType[tokenDetailType]);
			break;
	}
}
