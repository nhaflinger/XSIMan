//*****************************************************************************
// File: xsiRibParticlesData.cpp
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

#include "xsiRibParticlesData.h"
#include "xsiUtilities.h" 
#include <xsi_particlecloud.h>
#include <xsi_particlecloudprimitive.h>
#include <xsi_point.h>
#include <xsi_project.h>
#include <xsi_imageclip2.h>
#include <xsi_group.h> 

extern multimap<JobType, jobStruct*> jobList;
extern bool g_MotionBlur;
extern bool g_DeformBlur;
extern signed short g_FramePadding;
extern float g_CurrentFrame;
extern CString g_PixPath;
extern CString g_TexPath;
extern CString g_ShdPath;
extern CString g_RibPath;


xsiRibParticlesData::xsiRibParticlesData(X3DObject in_obj, float cframe, jobStruct* job)
{
	Application app;

	sampleTime = cframe;
	sampleIncrement = job->camera[0].shutter;

	// get camera matrix
	cameraMatrix = job->camera[0].matrix4;

	ParticleCloud particleCloud(in_obj);
	ParticleCloudPrimitive cloudPrim(particleCloud.GetActivePrimitive());

	CRefArray particleTypes = cloudPrim.GetParticleTypes();
    Property parType = particleTypes[0];

	particleType = SpriteParticle;
	CRef propRef = in_obj.GetProperties().GetItem(L"XSIManParticles");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter pType = prop.GetParameters().GetItem(L"ParticleType");
		switch((long)pType.GetValue()) {
			case PointParticle:
				particleType = PointParticle;
				break;
			case SphereParticle:
				particleType = SphereParticle;
				break;
			case BlobbyParticle:
				particleType = BlobbyParticle;
				break;
			case SpriteParticle:
				particleType = SpriteParticle;
				break;
		}
	}

	// check for instancing
	instancing = false;
	Parameter instEnable = parType.GetParameters().GetItem(L"Inst_Enable");
	instancing = instEnable.GetValue();
	Parameter instScalingType = parType.GetParameters().GetItem(L"Inst_ScalingType");
	Parameter instRotationType = parType.GetParameters().GetItem(L"Inst_RotationType");
	parType.GetNestedObjects().Filter(L"#Group", CStringArray(), L"", inst_groups);

	// get shader arguments
	float shape = 0;
	float normalType = 0;
	float falloffType = 0;
	float falloffX = 0.5;
	float falloffY = 0.5;
	float exponent = 3;

	// I'm not sure how to get retrieve this info
	//Parameter spriteParam = parType.GetParameters().GetItem(L"sprite_imagename");

	// get connections
	Parameter renderer = parType.GetParameters().GetItem(L"Renderer");
	CRef rendererSrc = renderer.GetSource();

	if (rendererSrc.IsValid()) {
		Shader shader = rendererSrc;

		Parameter Shape = shader.GetParameters().GetItem(L"Shape");
		shape = (float)Shape.GetValue();
		Parameter NormalType = shader.GetParameters().GetItem(L"NormalType");
		normalType = (float)NormalType.GetValue();

		Parameter shapeColor = shader.GetParameters().GetItem(L"Color");
		CRef colorSrc = shapeColor.GetSource();
		if (colorSrc.IsValid()) {
			Shader subShader = colorSrc;
			Parameter FalloffType = subShader.GetParameters().GetItem(L"FalloffType");
			falloffType = (float)FalloffType.GetValue();
			Parameter FalloffX = subShader.GetParameters().GetItem(L"FalloffX");
			falloffX = (float)FalloffX.GetValue();
			Parameter FalloffY = subShader.GetParameters().GetItem(L"FalloffY");
			falloffY = (float)FalloffY.GetValue();
			Parameter Exponent = subShader.GetParameters().GetItem(L"Exponent");
			exponent = (float)Exponent.GetValue();
		}
	}

	// get particle data
	CDoubleArray positionArray, velocityArray, sizeArray, colorArray, opacityArray, rotationArray, 
		angularVelocityArray, spriteAngleArray;
	CLongArray ageArray, ageLimitArray, IDArray;
	cloudPrim.GetPositionArray(positionArray);
	cloudPrim.GetVelocityArray(velocityArray);
	cloudPrim.GetSizeArray(sizeArray);
	cloudPrim.GetColorArray(colorArray);
	cloudPrim.GetColorArray(opacityArray);
	cloudPrim.GetRotationArray(rotationArray);
	cloudPrim.GetAngularVelocityArray(angularVelocityArray);
	cloudPrim.GetSpriteAngleArray(spriteAngleArray);
	cloudPrim.GetAgeArray(ageArray);
	cloudPrim.GetAgeLimitArray(ageLimitArray);
	cloudPrim.GetIDArray(IDArray);

	nparticles = positionArray.GetCount() / 3;

	// set position token data
	xsiRibTokens pointsTokenData;
	pointsTokenData.put("P", PointType, false, true, false, nparticles);
	pointsTokenData.putDetailType(VertexDetail);

	// set velocity token data
	xsiRibTokens velocityTokenData;
	velocityTokenData.put("velocity", PointType, false, true, false, nparticles);
	velocityTokenData.putDetailType(VertexDetail);

	Property playControl = app.GetActiveProject().GetProperties().GetItem(L"Play Control");
	fps = playControl.GetParameterValue(L"rate");
	float subframe = cframe - (int)cframe;

	for (long i = 0, j = 0; i < positionArray.GetCount(); i+=3,++j) {
		positionArray[i] += subframe * velocityArray[i] / fps;
		positionArray[i+1] += subframe * velocityArray[i+1] / fps;
		positionArray[i+2] += subframe * velocityArray[i+2] / fps;
		pointsTokenData.assignFloat(j, (float)positionArray[i], (float)positionArray[i+1],(float)positionArray[i+2]);
		velocityTokenData.assignFloat(j, (float)velocityArray[i], (float)velocityArray[i+1],(float)velocityArray[i+2]);
	}

	// set rotation token data
	xsiRibTokens rotationTokenData;
	rotationTokenData.put("rotation", PointType, false, true, false, nparticles);
	rotationTokenData.putDetailType(VertexDetail);

	// set angular velocity token data
	xsiRibTokens angularVelocityTokenData;
	angularVelocityTokenData.put("angularVelocity", PointType, false, true, false, nparticles);
	angularVelocityTokenData.putDetailType(VertexDetail);

	for (long i = 0, j = 0; i < rotationArray.GetCount(); i+=3,++j) {
		rotationArray[i] += subframe * angularVelocityArray[i] / fps;
		rotationArray[i+1] += subframe * angularVelocityArray[i+1] / fps;
		rotationArray[i+2] += subframe * angularVelocityArray[i+2] / fps;
		rotationTokenData.assignFloat(j, (float)rotationArray[i], (float)rotationArray[i+1],(float)rotationArray[i+2]);
		angularVelocityTokenData.assignFloat(j, (float)angularVelocityArray[i], (float)angularVelocityArray[i+1],(float)angularVelocityArray[i+2]);
	}

	// set color token data
	xsiRibTokens colorTokenData;
	colorTokenData.put("Cs", ColorType, false, true, false, colorArray.GetCount()/4);
	colorTokenData.putDetailType(VaryingDetail);

	for (long i = 0, j = 0; i < colorArray.GetCount(); i+=4,++j) {
		colorTokenData.assignFloat(j, (float)colorArray[i], (float)colorArray[i+1], 
			(float)colorArray[i+2]);
	}

	// set opacity token data
	xsiRibTokens opacityTokenData;
	opacityTokenData.put("Os", ColorType, false, true, false, opacityArray.GetCount()/4);
	opacityTokenData.putDetailType(VaryingDetail);

	for (long i = 0, j = 0; i < opacityArray.GetCount(); i+=4,++j) {
		opacityTokenData.assignFloat(j, (float)opacityArray[i+3], (float)opacityArray[i+3], 
			(float)opacityArray[i+3]);
	}

	// set width token data
	xsiRibTokens* widthTokenData = NULL;
	widthTokenData = new xsiRibTokens;
	widthTokenData->put("width", FloatType, false, true, false, sizeArray.GetCount());
	widthTokenData->putDetailType(VaryingDetail);

	for (long i = 0; i < sizeArray.GetCount(); ++i) {
		widthTokenData->assignFloat(i, (float)sizeArray[i]);
	}

	// set age token data
	xsiRibTokens* ageTokenData = NULL;
	ageTokenData = new xsiRibTokens;
	ageTokenData->put("age", FloatType, false, true, false, ageArray.GetCount());
	ageTokenData->putDetailType(VaryingDetail);

	for (long i = 0; i < ageArray.GetCount(); ++i) {
		ageTokenData->assignFloat(i, (float)ageArray[i]);
	}

	// set age limit token data
	xsiRibTokens* ageLimitTokenData = NULL;
	ageLimitTokenData = new xsiRibTokens;
	ageLimitTokenData->put("lifespan", FloatType, false, true, false, ageLimitArray.GetCount());
	ageLimitTokenData->putDetailType(VaryingDetail);

	for (long i = 0; i < ageLimitArray.GetCount(); ++i) {
		ageLimitTokenData->assignFloat(i, (float)ageLimitArray[i]);
	}

	// set ID token data
	xsiRibTokens* IDTokenData = NULL;
	IDTokenData = new xsiRibTokens;
	IDTokenData->put("id", FloatType, false, true, false, IDArray.GetCount());
	IDTokenData->putDetailType(VaryingDetail);

	for (long i = 0; i < IDArray.GetCount(); ++i) {
		IDTokenData->assignFloat(i, (float)IDArray[i]);
	}

	// set sprite angle token data
	xsiRibTokens* spriteAngleTokenData = NULL;
	spriteAngleTokenData = new xsiRibTokens;
	spriteAngleTokenData->put("spriteAngle", FloatType, false, true, false, spriteAngleArray.GetCount());
	spriteAngleTokenData->putDetailType(VaryingDetail);

	for (long i = 0; i < spriteAngleArray.GetCount(); ++i) {
		spriteAngleTokenData->assignFloat(i, (float)spriteAngleArray[i]);
	}

	// set shape token data
	xsiRibTokens* shapeTokenData = NULL;
	shapeTokenData = new xsiRibTokens;
	shapeTokenData->put("shape", FloatType, false, false, false, 0);
	shapeTokenData->putDetailType(ConstantDetail);
	shapeTokenData->assignFloat(0, shape);
	
	// set normal type token data
	xsiRibTokens* normalTypeTokenData = NULL;
	normalTypeTokenData = new xsiRibTokens;
	normalTypeTokenData->put("normalType", FloatType, false, false, false, 0);
	normalTypeTokenData->putDetailType(ConstantDetail);
	normalTypeTokenData->assignFloat(0, normalType);
	
	// set falloff type token data
	xsiRibTokens* falloffTypeTokenData = NULL;
	falloffTypeTokenData = new xsiRibTokens;
	falloffTypeTokenData->put("falloffType", FloatType, false, false, false, 0);
	falloffTypeTokenData->putDetailType(ConstantDetail);
	falloffTypeTokenData->assignFloat(0, falloffType);

	// set falloff x token data
	xsiRibTokens* falloffXTokenData = NULL;
	falloffXTokenData = new xsiRibTokens;
	falloffXTokenData->put("falloffX", FloatType, false, false, false, 0);
	falloffXTokenData->putDetailType(ConstantDetail);
	falloffXTokenData->assignFloat(0, falloffX);
	
	// set falloff y token data
	xsiRibTokens* falloffYTokenData = NULL;
	falloffYTokenData = new xsiRibTokens;
	falloffYTokenData->put("falloffY", FloatType, false, false, false, 0);
	falloffYTokenData->putDetailType(ConstantDetail);
	falloffYTokenData->assignFloat(0, falloffY);

	// set exponent token data
	xsiRibTokens* exponentTokenData = NULL;
	exponentTokenData = new xsiRibTokens;
	exponentTokenData->put("exponent", FloatType, false, false, false, 0);
	exponentTokenData->putDetailType(ConstantDetail);
	exponentTokenData->assignFloat(0, exponent);

	// add tokens
	ribTokens.push_back(pointsTokenData);
	ribTokens.push_back(velocityTokenData);
	ribTokens.push_back(rotationTokenData);
	ribTokens.push_back(angularVelocityTokenData);
	ribTokens.push_back(colorTokenData);
	ribTokens.push_back(opacityTokenData);
	ribTokens.push_back(*widthTokenData);
	ribTokens.push_back(*ageTokenData);
	ribTokens.push_back(*ageLimitTokenData);
	ribTokens.push_back(*IDTokenData);
	ribTokens.push_back(*spriteAngleTokenData);
	ribTokens.push_back(*shapeTokenData);
	ribTokens.push_back(*normalTypeTokenData);
	ribTokens.push_back(*falloffTypeTokenData);
	ribTokens.push_back(*falloffXTokenData);
	ribTokens.push_back(*falloffYTokenData);
	ribTokens.push_back(*exponentTokenData);

	// check for primitive variables
	Geometry geom(in_obj.GetActivePrimitive().GetGeometry(cframe));
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

	// cleanup
	delete widthTokenData;
	delete ageTokenData;
	delete ageLimitTokenData;
	delete shapeTokenData;
	delete normalTypeTokenData;
	delete falloffTypeTokenData;
	delete falloffXTokenData;
	delete falloffYTokenData;
	delete exponentTokenData;
	delete spriteAngleTokenData;
	delete IDTokenData;
}

xsiRibParticlesData::~xsiRibParticlesData()
{
}

void xsiRibParticlesData::write(nodeStruct* node)
{
	Application app;
	unsigned numTokens = ribTokens.size();
	RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
	RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
	assignRibTokensV(&ribTokens, tokenArray, pointerArray);

	// determine positions of data in tokenArray
	int pIndex, velocityIndex, rotationIndex, angularVelocityIndex, widthIndex, csIndex, osIndex, idIndex, ageIndex, 
		lifespanIndex, spriteAngleIndex, shapeIndex, normalTypeIndex, falloffTypeIndex, falloffXIndex, falloffYIndex, 
		exponentIndex;
	for (unsigned i = 0; i < ribTokens.size(); i++) {
		if (strcmp(tokenArray[i], "P") == 0) pIndex = i;
		else if (strcmp(tokenArray[i], "velocity") == 0) velocityIndex = i;
		else if (strcmp(tokenArray[i], "rotation") == 0) rotationIndex = i;
		else if (strcmp(tokenArray[i], "angularVelocity") == 0) angularVelocityIndex = i;
		else if (strcmp(tokenArray[i], "width") == 0) widthIndex = i;
		else if (strcmp(tokenArray[i], "Cs") == 0) csIndex = i;
		else if (strcmp(tokenArray[i], "Os") == 0) osIndex = i;
		else if (strcmp(tokenArray[i], "id") == 0) idIndex = i;
		else if (strcmp(tokenArray[i], "age") == 0) ageIndex = i;
		else if (strcmp(tokenArray[i], "lifespan") == 0) lifespanIndex = i;
		else if (strcmp(tokenArray[i], "spriteAngle") == 0) spriteAngleIndex = i;
		else if (strcmp(tokenArray[i], "shape") == 0) shapeIndex = i;
		else if (strcmp(tokenArray[i], "normalType") == 0) normalTypeIndex = i;
		else if (strcmp(tokenArray[i], "falloffType") == 0) falloffTypeIndex = i;
		else if (strcmp(tokenArray[i], "falloffX") == 0) falloffXIndex = i;
		else if (strcmp(tokenArray[i], "falloffY") == 0) falloffYIndex = i;
		else if (strcmp(tokenArray[i], "exponent") == 0) exponentIndex = i;
	}

	if (instancing) {
		for (long i=0; i<inst_groups.GetCount(); i++) {
			CRef ref(inst_groups[i]);
			Group grp(ref);
			CRefArray members = grp.GetMembers();
			vector<CString> objNames;
			for (long j=0; j<members.GetCount(); j++) {
				SIObject obj(members[j]);
				objNames.push_back(obj.GetFullName());
			}
			
			for (int i = 0; i < nparticles; i++) {
				// get archive file name
				wchar_t padframe[10];
				swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);
			
				// select object
				float id = ((float*)pointerArray[idIndex])[i];
				float toss = 0.5*(frand((long)id) + 1.0);
				int grp_mem = (int)(toss * members.GetCount());
				CString objName = objNames[grp_mem];
				
				// find archive job that corresponds to this object
				CString nuFileName;
				multimap<JobType, jobStruct*>::iterator jter;
				for (jter=jobList.begin(); jter!=jobList.end(); ++jter) {
					if ((*jter).first == ArchiveJob) {
						jobStruct* archiveJob = (*jter).second;	
						if (archiveJob->jobType == ArchiveJob) {
							if(archiveJob->name == objName) { 
								nuFileName = constructFileName(true, archiveJob);
								nuFileName = replaceSubString(nuFileName, L"\\", L"/");
							}
						}
					}
				}
				char* archiveFileName;
				XSIW2A(&archiveFileName, nuFileName.GetWideString());

				// get bounding box
				CValueArray in_args(1); CValue retval;
				in_args[0] = CValue(objName);
				app.ExecuteCommand(L"GetBBox", in_args, retval);
				float bbox[6]; 
				swscanf(retval.GetAsText().GetWideString(), L"(%f, %f, %f, %f, %f, %f)", &bbox[0], &bbox[1], &bbox[2], 
					&bbox[3], &bbox[4], &bbox[5]);

				RiAttributeBegin();
				if (g_MotionBlur) {
					RiMotionBegin(2, sampleTime, sampleTime + sampleIncrement);

					RtMatrix transformMatrix;
					MATH::CVector4 position, euler;

					position.PutX(((RtFloat*)pointerArray[pIndex])[i*3]);
					position.PutY(((RtFloat*)pointerArray[pIndex])[i*3+1]);
					position.PutZ(((RtFloat*)pointerArray[pIndex])[i*3+2]);
					position.PutW(1);
					euler.PutX(((RtFloat*)pointerArray[rotationIndex])[i*3]);
					euler.PutY(((RtFloat*)pointerArray[rotationIndex])[i*3+1]);
					euler.PutZ(((RtFloat*)pointerArray[rotationIndex])[i*3+2]);
					euler.PutW(1);
					setMatrix(position, euler, transformMatrix);
					RiTransform(transformMatrix);

					position.PutX(((RtFloat*)pointerArray[pIndex])[i*3] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3]/fps);
					position.PutY(((RtFloat*)pointerArray[pIndex])[i*3+1] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3+1]/fps);
					position.PutZ(((RtFloat*)pointerArray[pIndex])[i*3+2] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3+2]/fps);
					position.PutW(1);
					euler.PutX(((RtFloat*)pointerArray[rotationIndex])[i*3] + sampleIncrement*((RtFloat*)pointerArray[angularVelocityIndex])[i*3]/fps);
					euler.PutY(((RtFloat*)pointerArray[rotationIndex])[i*3+1] + sampleIncrement*((RtFloat*)pointerArray[angularVelocityIndex])[i*3+1]/fps);
					euler.PutZ(((RtFloat*)pointerArray[rotationIndex])[i*3+2] + sampleIncrement*((RtFloat*)pointerArray[angularVelocityIndex])[i*3+2]/fps);
					euler.PutW(1);
				
					setMatrix(position, euler, transformMatrix);
					RiTransform(transformMatrix);
	
					RiMotionEnd();

					RiArchiveRecord(RI_VERBATIM, "Procedural \"DelayedReadArchive\" [ \"%s\" ] [ %f %f %f %f %f %f ]\n", archiveFileName, bbox[0], bbox[3], bbox[1], bbox[4], bbox[2], bbox[5]);
				}
				else {
					RiTransformBegin();

					RtMatrix transformMatrix;
					MATH::CVector4 position, euler;

					position.PutX(((RtFloat*)pointerArray[pIndex])[i*3]);
					position.PutY(((RtFloat*)pointerArray[pIndex])[i*3+1]);
					position.PutZ(((RtFloat*)pointerArray[pIndex])[i*3+2]);
					position.PutW(1);
					euler.PutX(((RtFloat*)pointerArray[rotationIndex])[i*3]);
					euler.PutY(((RtFloat*)pointerArray[rotationIndex])[i*3+1]);
					euler.PutZ(((RtFloat*)pointerArray[rotationIndex])[i*3+2]);
					euler.PutW(1);
					setMatrix(position, euler, transformMatrix);
					RiTransform(transformMatrix);

					RiArchiveRecord(RI_VERBATIM, "Procedural \"DelayedReadArchive\" [ \"%s\" ] [ %f %f %f %f %f %f ]\n", archiveFileName, bbox[0], bbox[3], bbox[1], bbox[4], bbox[2], bbox[5]);
				
					RiTransformEnd();
				}
				RiAttributeEnd();
			}
		}
	}
	else if (particleType == PointParticle) {
		RiPointsV(nparticles, numTokens, tokenArray, pointerArray);
	}
	else if (particleType == SphereParticle) {
		for (int i = 0; i < nparticles; i++) {
			RtFloat radius = ((RtFloat*)pointerArray[widthIndex])[i];
			RtFloat id = ((RtFloat*)pointerArray[idIndex])[i];
			RtFloat age = ((RtFloat*)pointerArray[ageIndex])[i];
			RtFloat lifespan = ((RtFloat*)pointerArray[lifespanIndex])[i];

			RiAttributeBegin();

			RiColor(&((RtFloat*)pointerArray[csIndex])[i*3]);
			RiOpacity(&((RtFloat*)pointerArray[osIndex])[i*3]);

			if (g_MotionBlur) {
				RiMotionBegin(2, sampleTime, sampleTime + sampleIncrement);

				RtMatrix transformMatrix;
				MATH::CVector4 position, euler;

				position.PutX(((RtFloat*)pointerArray[pIndex])[i*3]);
				position.PutY(((RtFloat*)pointerArray[pIndex])[i*3+1]);
				position.PutZ(((RtFloat*)pointerArray[pIndex])[i*3+2]);
				position.PutW(1);
				euler.PutX(((RtFloat*)pointerArray[rotationIndex])[i*3]);
				euler.PutY(((RtFloat*)pointerArray[rotationIndex])[i*3+1]);
				euler.PutZ(((RtFloat*)pointerArray[rotationIndex])[i*3+2]);
				euler.PutW(1);
				setMatrix(position, euler, transformMatrix);
				RiTransform(transformMatrix);

				position.PutX(((RtFloat*)pointerArray[pIndex])[i*3] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3]/fps);
				position.PutY(((RtFloat*)pointerArray[pIndex])[i*3+1] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3+1]/fps);
				position.PutZ(((RtFloat*)pointerArray[pIndex])[i*3+2] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3+2]/fps);
				position.PutW(1);
				euler.PutX(((RtFloat*)pointerArray[rotationIndex])[i*3] + sampleIncrement*((RtFloat*)pointerArray[angularVelocityIndex])[i*3]/fps);
				euler.PutY(((RtFloat*)pointerArray[rotationIndex])[i*3+1] + sampleIncrement*((RtFloat*)pointerArray[angularVelocityIndex])[i*3+1]/fps);
				euler.PutZ(((RtFloat*)pointerArray[rotationIndex])[i*3+2] + sampleIncrement*((RtFloat*)pointerArray[angularVelocityIndex])[i*3+2]/fps);
				euler.PutW(1);
				
				setMatrix(position, euler, transformMatrix);
				RiTransform(transformMatrix);
	
				RiMotionEnd();

				RiSphere(0.5f*radius, -0.5f*radius, 0.5f*radius, 360, "float id", &id, "float age", &age, "float lifespan", &lifespan, RI_NULL);
			}
			else {
				RiTransformBegin();

				RtMatrix transformMatrix;
				MATH::CVector4 position, euler;

				position.PutX(((RtFloat*)pointerArray[pIndex])[i*3]);
				position.PutY(((RtFloat*)pointerArray[pIndex])[i*3+1]);
				position.PutZ(((RtFloat*)pointerArray[pIndex])[i*3+2]);
				position.PutW(1);
				euler.PutX(((RtFloat*)pointerArray[rotationIndex])[i*3]);
				euler.PutY(((RtFloat*)pointerArray[rotationIndex])[i*3+1]);
				euler.PutZ(((RtFloat*)pointerArray[rotationIndex])[i*3+2]);
				euler.PutW(1);
				setMatrix(position, euler, transformMatrix);
				RiTransform(transformMatrix);

				RiSphere(0.5f*radius, -0.5f*radius, 0.5f*radius, 360, "float id", &id, "float age", &age, "float lifespan", &lifespan, RI_NULL);
				
				RiTransformEnd();
			}

			RiAttributeEnd();
		}
	}
	else if (particleType == BlobbyParticle) {
		RtInt* codes = new RtInt[3*nparticles+2];
		RtFloat* flts = new RtFloat[16*nparticles];
		for (int i = 0, j = 0; i < 2*nparticles; i+=2,j+=16) {
			codes[i] = 1001;
			codes[i+1] = j;
		}
		codes[2*nparticles] = 0;
		codes[2*nparticles+1] = nparticles;
		for (int i = 2*nparticles+2, j = 0; i < 3*nparticles+2; i++,j++) {
			codes[i] = j;
		}

		int findex = 0;
		for (int k = 0; k < nparticles; k++) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (i == j) flts[findex] = ((RtFloat*)pointerArray[widthIndex])[k];
					else flts[findex] = 0.f;
					if (i == 3 && j == 0) flts[findex] = ((RtFloat*)pointerArray[pIndex])[k*3];
					if (i == 3 && j == 1) flts[findex] = ((RtFloat*)pointerArray[pIndex])[k*3+1];
					if (i == 3 && j == 2) flts[findex] = ((RtFloat*)pointerArray[pIndex])[k*3+2];
					if (i == 3 && j == 3) flts[findex] = 1;
					findex += 1;
				}
			}
		}

		RtColor* Cs = new RtColor[nparticles];
		RtColor* Os = new RtColor[nparticles];
		RtFloat* id = new RtFloat[nparticles];
		RtFloat* age = new RtFloat[nparticles];
		RtFloat* lifespan = new RtFloat[nparticles];

		for (int i = 0; i < nparticles; i++) {
			Cs[i][0] = ((RtFloat*)pointerArray[csIndex])[i*3];
			Cs[i][1] = ((RtFloat*)pointerArray[csIndex])[i*3+1];
			Cs[i][2] = ((RtFloat*)pointerArray[csIndex])[i*3+2];
			Os[i][0] = ((RtFloat*)pointerArray[osIndex])[i*3];
			Os[i][1] = ((RtFloat*)pointerArray[osIndex])[i*3+1];
			Os[i][2] = ((RtFloat*)pointerArray[osIndex])[i*3+2];
			id[i] = ((RtFloat*)pointerArray[idIndex])[i];
			age[i] = ((RtFloat*)pointerArray[ageIndex])[i];
			lifespan[i] = ((RtFloat*)pointerArray[lifespanIndex])[i];
		}

		RtString str = "";
		RiBlobby(nparticles, 3*nparticles+2, codes, 16*nparticles, flts, 1, &str, 
			"varying color Cs", (RtPointer)Cs, "varying color Os", (RtPointer)Os, 
			"varying float id", (RtPointer)id, "varying float age", (RtPointer)age, 
			"varying float lifespan", (RtPointer)lifespan, RI_NULL);

		// cleanup
		delete codes;
		delete flts;
		delete[] Cs;
		delete[] Os;
		delete id;
		delete age;
		delete lifespan;
	}
	else if (particleType == SpriteParticle) {
		RtFloat shape = ((RtFloat*)pointerArray[shapeIndex])[0]; 
		RtFloat normalType = ((RtFloat*)pointerArray[normalTypeIndex])[0]; 
		RtFloat falloffType = ((RtFloat*)pointerArray[falloffTypeIndex])[0]; 
		RtFloat falloffX = ((RtFloat*)pointerArray[falloffXIndex])[0];
		RtFloat falloffY = ((RtFloat*)pointerArray[falloffYIndex])[0]; 
		RtFloat exponent = ((RtFloat*)pointerArray[exponentIndex])[0];
			
		// sprites should be facing camera
		MATH::CVector3 cameraRight(1, 0, 0);
		MATH::CVector3 cameraUp(0, 1, 0);
		MATH::CVector3 cameraEye(0, 0, 1);
		MATH::CMatrix4 numatrix;
		numatrix = cameraMatrix;
		numatrix.SetValue(3, 0, 0);
		numatrix.SetValue(3, 1, 0);
		numatrix.SetValue(3, 2, 0);
		cameraRight *= numatrix;
		cameraRight.NormalizeInPlace();
		cameraUp *= numatrix;
		cameraUp.NormalizeInPlace();
		cameraEye *= numatrix;
		cameraEye.NormalizeInPlace();

		for (int i = 0; i < nparticles; i++) {
			RtFloat radius = ((RtFloat*)pointerArray[widthIndex])[i];
			RtFloat id = ((RtFloat*)pointerArray[idIndex])[i];
			RtFloat age = ((RtFloat*)pointerArray[ageIndex])[i];
			RtFloat lifespan = ((RtFloat*)pointerArray[lifespanIndex])[i];
			float spriteAngle = ((RtFloat*)pointerArray[spriteAngleIndex])[i];

			MATH::CVector3 upDir = cameraUp;
			MATH::CVector3 rightDir = cameraRight;  

			if (spriteAngle != 0) {
				MATH::CQuaternion quat(spriteAngle, cameraEye.GetX(), cameraEye.GetY(), cameraEye.GetZ());
				MATH::CRotation rotation = quat;
				MATH::CMatrix3 rotationMatrix = rotation.GetMatrix();
				upDir *= rotationMatrix;
				rightDir *= rotationMatrix;
			}

			RiAttributeBegin();

			RiColor(&((RtFloat*)pointerArray[csIndex])[i*3]);
			RiOpacity(&((RtFloat*)pointerArray[osIndex])[i*3]);

			if (g_MotionBlur) {
				RiMotionBegin(2, sampleTime, sampleTime + sampleIncrement);
				
				RiTranslate(((RtFloat*)pointerArray[pIndex])[i*3+0], ((RtFloat*)pointerArray[pIndex])[i*3+1], 
					((RtFloat*)pointerArray[pIndex])[i*3+2]);

				RiTranslate(((RtFloat*)pointerArray[pIndex])[i*3+0] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3]/fps, 
					((RtFloat*)pointerArray[pIndex])[i*3+1] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3+1]/fps, 
					((RtFloat*)pointerArray[pIndex])[i*3+2] + sampleIncrement*((RtFloat*)pointerArray[velocityIndex])[i*3+2]/fps);
				
				RiMotionEnd();
				
                float sprite[12] = { 
					-0.5f*radius * (float)rightDir.GetX() + 0.5f*radius * (float)upDir.GetX(),
					-0.5f*radius * (float)rightDir.GetY() + 0.5f*radius * (float)upDir.GetY(),
					-0.5f*radius * (float)rightDir.GetZ() + 0.5f*radius * (float)upDir.GetZ(),
					 0.5f*radius * (float)rightDir.GetX() + 0.5f*radius * (float)upDir.GetX(),
					 0.5f*radius * (float)rightDir.GetY() + 0.5f*radius * (float)upDir.GetY(),
					 0.5f*radius * (float)rightDir.GetZ() + 0.5f*radius * (float)upDir.GetZ(),
					-0.5f*radius * (float)rightDir.GetX() - 0.5f*radius * (float)upDir.GetX(),
					-0.5f*radius * (float)rightDir.GetY() - 0.5f*radius * (float)upDir.GetY(),
					-0.5f*radius * (float)rightDir.GetZ() - 0.5f*radius * (float)upDir.GetZ(),
					 0.5f*radius * (float)rightDir.GetX() - 0.5f*radius * (float)upDir.GetX(),
					 0.5f*radius * (float)rightDir.GetY() - 0.5f*radius * (float)upDir.GetY(),
					 0.5f*radius * (float)rightDir.GetZ() - 0.5f*radius * (float)upDir.GetZ() 
				};

                RiPatch("bilinear", "P", &sprite, "float id", &id, "float age", &age, "float lifespan", &lifespan, 
					"constant float shape", &shape, "constant float normalType", &normalType, "constant float falloffType", 
					&falloffType, "constant float falloffX", &falloffX, "constant float falloffY", &falloffY, 
					"constant float exponent", &exponent, RI_NULL);
			
			}
			else {
				RiTransformBegin();

				RiTranslate(((RtFloat*)pointerArray[pIndex])[i*3+0], ((RtFloat*)pointerArray[pIndex])[i*3+1], 
					((RtFloat*)pointerArray[pIndex])[i*3+2]);

				float sprite[12] = { 
					-0.5f*radius * (float)rightDir.GetX() + 0.5f*radius * (float)upDir.GetX(),
					-0.5f*radius * (float)rightDir.GetY() + 0.5f*radius * (float)upDir.GetY(),
					-0.5f*radius * (float)rightDir.GetZ() + 0.5f*radius * (float)upDir.GetZ(),
					 0.5f*radius * (float)rightDir.GetX() + 0.5f*radius * (float)upDir.GetX(),
					 0.5f*radius * (float)rightDir.GetY() + 0.5f*radius * (float)upDir.GetY(),
					 0.5f*radius * (float)rightDir.GetZ() + 0.5f*radius * (float)upDir.GetZ(),
					-0.5f*radius * (float)rightDir.GetX() - 0.5f*radius * (float)upDir.GetX(),
					-0.5f*radius * (float)rightDir.GetY() - 0.5f*radius * (float)upDir.GetY(),
					-0.5f*radius * (float)rightDir.GetZ() - 0.5f*radius * (float)upDir.GetZ(),
					 0.5f*radius * (float)rightDir.GetX() - 0.5f*radius * (float)upDir.GetX(),
					 0.5f*radius * (float)rightDir.GetY() - 0.5f*radius * (float)upDir.GetY(),
					 0.5f*radius * (float)rightDir.GetZ() - 0.5f*radius * (float)upDir.GetZ() 
				};
		
                RiPatch("bilinear", "P", &sprite, "float id", &id, "float age", &age, "float lifespan", &lifespan, 
					"constant float shape", &shape, "constant float normalType", &normalType, "constant float falloffType", 
					&falloffType, "constant float falloffX", &falloffX, "constant float falloffY", &falloffY, 
					"constant float exponent", &exponent, RI_NULL);

				RiTransformEnd();
				
			}

			RiAttributeEnd();
		}
	}
}

ObjectType xsiRibParticlesData::type() const
{
	return ParticlesObject;
}


void xsiRibParticlesData::setMatrix(MATH::CVector4 position, MATH::CVector4 euler, RtMatrix transformMatrix)
{
	RtMatrix mat1;
	identityMatrix(mat1);

	rotateMatrix(transformMatrix, mat1, (float)euler[0], 0);
	rotateMatrix(mat1, transformMatrix, (float)euler[1], 1);
	rotateMatrix(transformMatrix, mat1, (float)euler[2], 2);

	for(int i = 0; i < 4 ; i++) {
		for(int j = 0; j < 4 ; j++) {
			if (i == 3) transformMatrix[i][j] = (float)position[j];
		}
	}
}

CString xsiRibParticlesData::constructFileName(bool ribFile, jobStruct* myjob)
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
