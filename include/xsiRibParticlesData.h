//*****************************************************************************
// File: xsiRibParticlesData.h
// Description: xsiRibParticlesData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#ifndef xsiRibParticlesData_H
#define xsiRibParticlesData_H

#include "xsiRibData.h"
#include <map>

//*****************************************************************************
//    class: xsiRibParticlesData xsiRibParticlesData.h
//*****************************************************************************

class xsiRibParticlesData : public xsiRibData
{
public:
	xsiRibParticlesData(X3DObject, float, jobStruct*);
	~xsiRibParticlesData();
	virtual void write(nodeStruct*);
	virtual ObjectType type() const;
	void setMatrix(MATH::CVector4, MATH::CVector4, RtMatrix);

private:
	XSI::CString constructFileName(bool, jobStruct*);
	friend class xsiRibTranslator;
	RtInt nparticles;
	ParticleType particleType;
	bool instancing;
	CRefArray inst_groups;
	MATH::CMatrix4 cameraMatrix;
	float sampleTime;
	float sampleIncrement;
	float fps;
};

#endif