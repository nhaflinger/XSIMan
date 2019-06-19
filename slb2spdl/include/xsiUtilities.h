//*****************************************************************************
// File: xsiUtilities.h
// Description: xsiUtilities functions.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiUtilities_H
#define xsiUtilities_H

#include "xsiRibTokens.h"

CString replaceSubString(CString oldString, CString wold, CString wnew);
bool stringHasChar(CString bstr, CString lstr);
CString subString(CString oldString, long index1, long index2);
void getCompoundParameter(Parameter param, float vec3[3]);

#endif