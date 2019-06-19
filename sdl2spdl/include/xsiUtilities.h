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

#include <ri.h>
#include "xsiRibTokens.h"

void assignRibTokensV(std::vector<xsiRibTokens>* ribTokens, RtToken tokens[], RtPointer pointers[]);
CString replaceSubString(CString oldString, CString wold, CString wnew);
bool stringHasChar(CString bstr, CString lstr);
CString subString(CString oldString, long index1, long index2);
void getCompoundParameter(Parameter param, float vec3[3]);
void rotateMatrix(RtMatrix mo, RtMatrix mi, float angle, int axis);
void identityMatrix(RtMatrix a);
void multMatrix(RtMatrix a, RtMatrix b, RtMatrix c);
CValue TransferAllPropertiesAcrossGenOp(const CValue& in_operator, const CValue& in_object, const CString& in_propertytransfertype, bool in_shareproperty);
CRef getOperator(CustomProperty& oProp, CString in_op);

#endif