//*****************************************************************************
// File: xsiRibMeshData.cpp
// Description: xsiRibMeshData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibMeshData.h"
#include "xsiUtilities.h" 
#include <xsi_application.h>
#include <xsi_material.h>
#include <xsi_texture.h>
#include <xsi_polygonface.h>

extern bool g_ApproximatedMesh;
extern bool g_PolygonClusters;


xsiRibMeshData::xsiRibMeshData(X3DObject in_obj, float cframe, CString clusterName)
{
	Application app;
	RtInt numPoints, numNormals;
	CLongArray pointIndexArray, nodeIndexArray;	
	MATH::CVector3Array normalArray, positionArray;
	CDoubleArray uvValueArray;
	long numFaceVertices, lnbUV;
	vector<long> exclusionList;
	vector<long> inclusionList;

	nverts = NULL;
	verts = NULL;

	Geometry geom(in_obj.GetActivePrimitive().GetGeometry(cframe));
	PolygonMesh polymesh(geom);

	// get approximated mesh
	CRef propRef = in_obj.GetProperties().GetItem(L"Geometry Approximation");
	long subdivisionLevel = 0;
	long subdivisionRule = 0;
	bool discontinuity = true;
	double discontinuityAngle = 60;
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter gapproxmosr = prop.GetParameters().GetItem(L"gapproxmosr");
		subdivisionRule = gapproxmosr.GetValue();
		Parameter gapproxmordrsl = prop.GetParameters().GetItem(L"gapproxmordrsl");
		subdivisionLevel = gapproxmordrsl.GetValue();
		Parameter gapproxmoad = prop.GetParameters().GetItem(L"gapproxmoad");
		discontinuity = gapproxmoad.GetValue();
		Parameter gapproxmoan = prop.GetParameters().GetItem(L"gapproxmoan");
		discontinuityAngle = gapproxmoan.GetValue();
	}

	if (subdivisionLevel > 0 && g_ApproximatedMesh == true) {
		CValue retval = GetApproximatedMeshAndAttributes(polymesh, subdivisionRule, subdivisionLevel, discontinuity, discontinuityAngle);
		CValueArray attribs(retval);
		long nattribs = attribs.GetCount();
		if(nattribs > 0) {
			//	vertex data
			CValueArray attrib(attribs[0]);
			long nelem = attrib.GetCount();		
			positionArray.Resize(nelem/3);
			numPoints = nelem/3;
			for (long i = 0; i < numPoints; i++) {
				positionArray[i].PutX(attrib[i*3 + 0]);
				positionArray[i].PutY(attrib[i*3 + 1]);
				positionArray[i].PutZ(attrib[i*3 + 2]);
			}

			// polygon data
			attrib = attribs[1];
			nelem = attrib.GetCount();	
			long npnt = 0;
			numFaces = 0;
			numFaceVertices = 0;

			// allocate space
			for (long i = 0; i < nelem; i+=(npnt+1)) {
				numFaces += 1;
				npnt = attrib[i];
				for (long j = 1; j <= npnt; j++) {
					numFaceVertices += 1;
				}
			}
			nverts = new RtInt[numFaces];
			verts = new RtInt[numFaceVertices];
			
			npnt = 0;
			pointIndexArray.Clear();
			for (long i = 0, k = 0; i < nelem; i+=(npnt+1),k++) {
				npnt = attrib[i];
				nverts[k] = npnt;
				for (long j = 1; j <= npnt; j++) {
					pointIndexArray.Add(attrib[i + j]);
				}
			}

			// normal data
			attrib = attribs[2];
			nelem = attrib.GetCount();	
			normalArray.Resize(nelem/3);
			numNormals = nelem/3;
			for (long i = 0; i < numNormals; i++ ){
				normalArray[i].PutX(attrib[i*3 + 0]);
				normalArray[i].PutY(attrib[i*3 + 1]);
				normalArray[i].PutZ(attrib[i*3 + 2]);
			}

			// polygon face index array
			attrib = attribs[3];
			nelem = attrib.GetCount();
			long nnode = 0;
			for (long i = 0; i < nelem; i+=(nnode+1) ) {
				nnode = attrib[i];
				for (long j = 1; j <= nnode; j++) {
					nodeIndexArray.Add(attrib[i + j]);
				}
			}
		
			// store additional UV sets here
			CDoubleArray stValueArray[16];
			CString uvSetName[16];

			// UV Cluster Properties
			attrib = attribs[5];
			long numSTs = attrib.GetCount() / 2;
			long numSet = 0;
			for (long i = 0,k = 0; i < numSTs; i++,k+=2) {
				if (i == 0) {
					CValueArray attrib1 = attrib[k + 1];
					nelem = attrib1.GetCount();	
					uvValueArray.Resize(nelem);
					lnbUV = uvValueArray.GetCount() / 3;
					for (long j = 0; j < lnbUV; j++ ){
						uvValueArray[j*3 + 0] = attrib1[j*3 + 0];
						uvValueArray[j*3 + 1] = attrib1[j*3 + 1];
						uvValueArray[j*3 + 2] = attrib1[j*3 + 2];
					}
				}
				else {				
					if (numSet < 16) {
						ClusterProperty uvcluster(attrib[k]);
						uvSetName[numSet] = uvcluster.GetName();
						CValueArray attrib1 = attrib[k + 1];
						nelem = attrib1.GetCount();
						stValueArray[numSet].Resize(nelem);
						for (long j = 0; j < stValueArray[numSet].GetCount()/3; j++) {
							stValueArray[numSet][j*3 + 0] = attrib1[j*3 + 0];
							stValueArray[numSet][j*3 + 1] = attrib1[j*3 + 1];
							stValueArray[numSet][j*3 + 2] = attrib1[j*3 + 2];
						}
					}
					numSet += 1;
				}
			}
						
			// Material Cluster Properties
			if (g_PolygonClusters == true) {
				attrib = attribs[7];
				long numMCs = attrib.GetCount();
				for (long i = 0; i < numMCs; i++) {
					CValueArray attrib2 = attrib[i*2 + 1];
					nelem = attrib2.GetCount();	
					CLongArray clusterIndices;
					clusterIndices.Resize(nelem);
					for (long j = 0; j < clusterIndices.GetCount(); ++j){
						clusterIndices[j] = attrib2[j];
					}

					Material material(attrib[i*2]);
					
					CRefArray clsref = material.GetUsedBy();
					bool clsname = false;
					for (long k=0; k<clsref.GetCount(); k++) {
						if (clsref[0].GetAsText() == clusterName) {
							clsname = true;
							break;
						}
					}

					if (material.IsValid() && clusterName == L"") {
						for (long j = 0; j < (long)clusterIndices.GetCount(); ++j) { 
								exclusionList.push_back(clusterIndices[j]);
						}
					}
					else if (material.IsValid() && clsname == true) {
						for (long j = 0; j < (long)clusterIndices.GetCount(); ++j) { 
								inclusionList.push_back(clusterIndices[j]);
						}
					}
				}
			}

			// set points token data
			xsiRibTokens pointsTokenData;
			pointsTokenData.put("P", PointType, false, true, false, numPoints);
			pointsTokenData.putDetailType(VertexDetail);

			// set normals token data
			xsiRibTokens normalsTokenData;
			if (numNormals == numPoints) {
				normalsTokenData.put("N", NormalType, false, true, false, numPoints);
				normalsTokenData.putDetailType(VertexDetail);
			} 
			else {
				normalsTokenData.put("N", NormalType, false, true, false, numFaceVertices);
				normalsTokenData.putDetailType(FaceVaryingDetail);
			}

			xsiRibTokens* vertexSTTokenData = NULL;
			xsiRibTokens* faceVertexSTokenData = NULL;
			xsiRibTokens* faceVertexTTokenData = NULL;

			if (numSTs > 0) {
				if (lnbUV == numPoints) {
					vertexSTTokenData = new xsiRibTokens;
					vertexSTTokenData->put("st", FloatType, false, true, true, 2 * numPoints);
					vertexSTTokenData->putDetailType(VertexDetail);
				} 
				else {
					faceVertexSTokenData = new xsiRibTokens;
					faceVertexSTokenData->put("s", FloatType, false, true, false, numFaceVertices);
					faceVertexSTokenData->putDetailType(FaceVaryingDetail);
					faceVertexTTokenData = new xsiRibTokens;
					faceVertexTTokenData->put("t", FloatType, false, true, false, numFaceVertices);
					faceVertexTTokenData->putDetailType(FaceVaryingDetail);
				}
			}

			// build polygon list
			attrib = attribs[3];
			nelem = attrib.GetCount();
			long faceVertex = 0;
			long vertex, normal;
			nnode = 0;
			for (long i = 0, k = 0; i < nelem; i+=(nnode+1),k++) {
				nnode = attrib[i];
							
				bool excludeFace = false;
				vector<long>::iterator iter;
				for (iter=exclusionList.begin(); iter!=exclusionList.end(); ++iter) {
					if ((*iter) == k) {
						excludeFace = true;
						break;
					}
				}

				for (iter=inclusionList.begin(); iter!=inclusionList.end(); ++iter) {
					excludeFace = true;
					if ((*iter) == k) {
						excludeFace = false;
						break;
					}
				}

				for (long j = 1; j <= nnode; j++) {
					vertex = pointIndexArray[attrib[i + j]];
					verts[faceVertex] = vertex;
					if (excludeFace == true) verts[faceVertex] = -1;
									
					pointsTokenData.assignFloat(vertex, (float)positionArray[vertex].GetX(), 
						(float)positionArray[vertex].GetY(), (float)positionArray[vertex].GetZ());

					normal = nodeIndexArray[attrib[i + j]];

					if (numNormals == numPoints) {
						normalsTokenData.assignFloat(vertex, (float)normalArray[normal].GetX(),
							(float)normalArray[normal].GetY(), (float)normalArray[normal].GetZ());
					} 
					else {
						normalsTokenData.assignFloat(faceVertex, (float)normalArray[normal].GetX(), 
							(float)normalArray[normal].GetY(), (float)normalArray[normal].GetZ());
					}
									
					if (numSTs > 0) {
						float S, T = 0;

						if (lnbUV == numPoints) {
							S = (float)uvValueArray[vertex*3];
							T = (float)uvValueArray[vertex*3 + 1];
							vertexSTTokenData->assignFloat(vertex*2 + 0, S);
							vertexSTTokenData->assignFloat(vertex*2 + 1, 1 - T);
						} 
						else {	
							S = (float)uvValueArray[faceVertex*3];
							T = (float)uvValueArray[faceVertex*3 + 1];
							faceVertexSTokenData->assignFloat(faceVertex, S);
							faceVertexTTokenData->assignFloat(faceVertex, 1 - T);
						} 
					}

					++faceVertex;
				}
			}
					
			// add tokens and cleanup
			ribTokens.push_back(pointsTokenData);
			ribTokens.push_back(normalsTokenData);
			if (vertexSTTokenData != NULL) {
				ribTokens.push_back(*vertexSTTokenData);
				delete vertexSTTokenData;
			}
			if (faceVertexSTokenData != NULL) {
				ribTokens.push_back(*faceVertexSTokenData);
				delete faceVertexSTokenData;
			}
			if (faceVertexTTokenData != NULL) {
				ribTokens.push_back(*faceVertexTTokenData);
				delete faceVertexTTokenData;
			} 

			// check for primitive variables
			/*CRefArray clsRefArray;
			geom.GetClusters().Filter(siVertexCluster, CStringArray(), L"", clsRefArray);
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
			}*/

			// additional UV sets
			for (long k = 0; k < numSTs-1; ++k) {
				lnbUV = stValueArray[k].GetCount() / 3;			
				
				if (lnbUV == numPoints) {
					CString stName = L"uv_" + uvSetName[k];
					char* ststr = NULL;
					XSIW2A(&ststr, stName.GetWideString());
					vertexSTTokenData = new xsiRibTokens;
					vertexSTTokenData->put(ststr, FloatType, false, true, true, 2 * numPoints);
					vertexSTTokenData->putDetailType(VertexDetail);
				} 
				else {
					CString stName = L"u_" + uvSetName[k];
					char* sstr = NULL;
					XSIW2A(&sstr, stName.GetWideString());
					faceVertexSTokenData = new xsiRibTokens;
					faceVertexSTokenData->put(sstr, FloatType, false, true, false, numFaceVertices);
					faceVertexSTokenData->putDetailType(FaceVaryingDetail);
					stName = L"v_" + uvSetName[k];
					char* tstr = NULL;
					XSIW2A(&tstr, stName.GetWideString());
					faceVertexTTokenData = new xsiRibTokens;
					faceVertexTTokenData->put(tstr, FloatType, false, true, false, numFaceVertices);
					faceVertexTTokenData->putDetailType(FaceVaryingDetail);
				}
						
				attrib = attribs[3];
				nelem = attrib.GetCount();
				long faceVertex = 0;
				long vertex;
				nnode = 0;
				for (long i = 0; i < nelem; i+=(nnode+1)) {
					nnode = attrib[i];
					
					for (long j = 1; j <= nnode; j++) {
						vertex = pointIndexArray[attrib[i + j]];

						if (numSTs > 0) {
							float S, T = 0;

							if (lnbUV == numPoints) {
								S = (float)stValueArray[k][vertex*3];
								T = (float)stValueArray[k][vertex*3 + 1];
								vertexSTTokenData->assignFloat(vertex*2 + 0, S);
								vertexSTTokenData->assignFloat(vertex*2 + 1, 1 - T);
							} 
							else {	
								S = (float)stValueArray[k][faceVertex*3];
								T = (float)stValueArray[k][faceVertex*3 + 1];
								faceVertexSTokenData->assignFloat(faceVertex, S);
								faceVertexTTokenData->assignFloat(faceVertex, 1 - T);
							} 
						}

						++faceVertex;
					}
				}
						
				// add tokens and cleanup
				if (vertexSTTokenData != NULL) {
					ribTokens.push_back(*vertexSTTokenData);
					delete vertexSTTokenData;
				}
				if (faceVertexSTokenData != NULL) {
					ribTokens.push_back(*faceVertexSTokenData);
					delete faceVertexSTokenData;
				}
				if (faceVertexTTokenData != NULL) {
					ribTokens.push_back(*faceVertexTTokenData);
					delete faceVertexTTokenData;
				} 
			}
		}
	}
	else {	
		// get the vertices
		CPointRefArray pointRefArray(polymesh.GetPoints());
		positionArray = pointRefArray.GetPositionArray();
		numPoints = positionArray.GetCount();

		// get the facets
		CFacetRefArray facetRefArray(polymesh.GetFacets());
		CLongArray facetIndices = facetRefArray.GetIndexArray();
		numFaces = facetRefArray.GetCount();
		        
		// build exclusion list for faces in poly clusters
		if (g_PolygonClusters == true) {
			CRefArray polyClusterRefArray;
			geom.GetClusters().Filter(siPolygonCluster, CStringArray(), L"", polyClusterRefArray);

			for (long i = 0; i < polyClusterRefArray.GetCount(); i++) {
				Cluster cluster(polyClusterRefArray[i]);
				Material material = cluster.GetMaterial();
				CLongArray clusterIndices;
				cluster.FindIndices(facetIndices, clusterIndices); 
				if (material.IsValid() && clusterName == L"") {
					for (long j = 0; j < (long)clusterIndices.GetCount(); ++j) {
						if (clusterIndices[j] != -1) 
							exclusionList.push_back(facetIndices[j]);
					}
				}
				else if (material.IsValid() && clusterName == cluster.GetFullName()) {
					for (long j = 0; j < (long)clusterIndices.GetCount(); ++j) {
						if (clusterIndices[j] == -1) 
							exclusionList.push_back(facetIndices[j]);
					}
				}
			}
		}
	
		// get polygon faces 
		CPolygonFaceRefArray faceRefArray(polymesh.GetPolygons());

		// get the normals
		CPolygonNodeRefArray nodeRefArray(polymesh.GetNodes());
		normalArray = nodeRefArray.GetNormalArray();
		numNormals = normalArray.GetCount();
		nodeIndexArray = nodeRefArray.GetIndexArray();

		// get the UV cluster
		CRefArray clusterRefArray;
		polymesh.GetClusters().Filter(siSampledPointCluster, CStringArray(), L"", clusterRefArray);
		Cluster	samplePointClusterUV;
		CRefArray uvClusterPropertiesRefArray;
		for (long i = 0; i < clusterRefArray.GetCount(); i++) {
			Cluster cluster(clusterRefArray[i]);
			if(cluster.GetProperties().Filter(siClsUVSpaceTxtType, CStringArray(), L"", uvClusterPropertiesRefArray) == CStatus::OK) {
				samplePointClusterUV = cluster;	
				break;
			}
		}
	
		// get UVs from texture projection definition
		lnbUV = 0;
		long numSTs = uvClusterPropertiesRefArray.GetCount();
		Material material = in_obj.GetMaterial();
		ClusterProperty uvProp(material.GetCurrentUV());;
		CString currentUV = uvProp.GetName();
		Texture texture = material.GetCurrentTexture();
		CStatus st = texture.GetTransformValues(127, uvValueArray);
		lnbUV = uvValueArray.GetCount() / 3;

		// store additional UV sets here
		CDoubleArray stValueArray[16];
		CString uvSetName[16];

		for (i = 0;  i < numSTs; ++i) {
			ClusterProperty uvProp(uvClusterPropertiesRefArray[i]);
			CString uvPropName = uvProp.GetName();
			CClusterPropertyElementArray uvElementArray = uvProp.GetElements();
			// get UVs directly from cluster if unable to access texture projection definition
			if (i == 0 && st == CStatus::Fail) {
				uvValueArray = uvElementArray.GetArray();
				lnbUV = uvValueArray.GetCount() / 3;
			}

			if (i < 16) {
				stValueArray[i] = uvElementArray.GetArray();
				uvSetName[i] = uvPropName;
			}
		}

		// allocate space
		RtInt numFaceVertices(0);
		nverts = new RtInt[numFaces];
		for (long i = 0; i < numFaces; ++i) {
			Facet facet(facetRefArray.GetItem(i));
			pointRefArray = facet.GetPoints();
			nverts[i] = pointRefArray.GetCount();
			numFaceVertices += pointRefArray.GetCount();
		}
		verts = new RtInt[numFaceVertices];

		// set points token data
		xsiRibTokens pointsTokenData;
		pointsTokenData.put("P", PointType, false, true, false, numPoints);
		pointsTokenData.putDetailType(VertexDetail);

		// set normals token data
		xsiRibTokens normalsTokenData;
		if (numNormals == numPoints) {
			normalsTokenData.put("N", NormalType, false, true, false, numPoints);
			normalsTokenData.putDetailType(VertexDetail);
		} 
		else {
			normalsTokenData.put("N", NormalType, false, true, false, numFaceVertices);
			normalsTokenData.putDetailType(FaceVaryingDetail);
		}

		xsiRibTokens* vertexSTTokenData = NULL;
		xsiRibTokens* faceVertexSTokenData = NULL;
		xsiRibTokens* faceVertexTTokenData = NULL;

		if (numSTs > 0) {
			if (lnbUV == numPoints) {
				vertexSTTokenData = new xsiRibTokens;
				vertexSTTokenData->put("st", FloatType, false, true, true, 2 * numPoints);
				vertexSTTokenData->putDetailType(VertexDetail);
			} 
			else {
				faceVertexSTokenData = new xsiRibTokens;
				faceVertexSTokenData->put("s", FloatType, false, true, false, numFaceVertices);
				faceVertexSTokenData->putDetailType(FaceVaryingDetail);
				faceVertexTTokenData = new xsiRibTokens;
				faceVertexTTokenData->put("t", FloatType, false, true, false, numFaceVertices);
				faceVertexTTokenData->putDetailType(FaceVaryingDetail);
			}
		}

		// build polygon list
		long faceVertex = 0;
		long vertex, normal;
		CLongArray sampleIndexArray;

		for (i = 0; i < numFaces; ++i) {
			Facet facet(facetRefArray.GetItem(i));
			pointRefArray = facet.GetPoints();
			pointIndexArray = pointRefArray.GetIndexArray();

			CSampleRefArray samples(facet.GetSamples());
			sampleIndexArray = samples.GetIndexArray();

			PolygonFace polyface(faceRefArray.GetItem(i));
			CPolygonNodeRefArray nodeRefArray(polyface.GetNodes());
			normalArray = nodeRefArray.GetNormalArray();

			bool excludeFace = false;
			vector<long>::iterator iter;
			for (iter=exclusionList.begin(); iter!=exclusionList.end(); ++iter) {
				if ((*iter) == i) {
					excludeFace = true;
					break;
				}
			}

			for (long j = 0; j < pointRefArray.GetCount(); ++j) {
				vertex = pointIndexArray[j];
				verts[faceVertex] = vertex;	
				if (excludeFace == true) verts[faceVertex] = -1;

				long sampleIndex = sampleIndexArray[j];

				pointsTokenData.assignFloat(vertex, (float)positionArray[vertex].GetX(), 
					(float)positionArray[vertex].GetY(), (float)positionArray[vertex].GetZ());

				normal = nodeIndexArray[j];

				if (numNormals == numPoints) {
					normalsTokenData.assignFloat(vertex, (float)normalArray[normal].GetX(),
						(float)normalArray[normal].GetY(), (float)normalArray[normal].GetZ());
				} 
				else {
					normalsTokenData.assignFloat(faceVertex, (float)normalArray[normal].GetX(), 
						(float)normalArray[normal].GetY(), (float)normalArray[normal].GetZ());
				}

				if (numSTs > 0) {
					float S, T = 0;

					if (lnbUV == numPoints) {
						S = (float)uvValueArray[vertex*3];
						T = (float)uvValueArray[vertex*3 + 1];
						vertexSTTokenData->assignFloat(vertex*2 + 0, S);
						vertexSTTokenData->assignFloat(vertex*2 + 1, 1 - T);
					} 
					else {	
						S = (float)uvValueArray[sampleIndex*3];
						T = (float)uvValueArray[sampleIndex*3 + 1];
						faceVertexSTokenData->assignFloat(faceVertex, S);
						faceVertexTTokenData->assignFloat(faceVertex, 1 - T);
					} 
				}

				++faceVertex;
			}
		}

		// add tokens and cleanup
		ribTokens.push_back(pointsTokenData);
		ribTokens.push_back(normalsTokenData);
		if (vertexSTTokenData != NULL) {
			ribTokens.push_back(*vertexSTTokenData);
			delete vertexSTTokenData;
		}
		if (faceVertexSTokenData != NULL) {
			ribTokens.push_back(*faceVertexSTokenData);
			delete faceVertexSTokenData;
		}
		if (faceVertexTTokenData != NULL) {
			ribTokens.push_back(*faceVertexTTokenData);
			delete faceVertexTTokenData;
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

		// additional UV sets
		for (long k = 0; k < numSTs; ++k) {
			faceVertex = 0;
			lnbUV = stValueArray[k].GetCount() / 3;

			if (lnbUV == numPoints) {
				CString stName = L"uv_" + uvSetName[k];
				char* ststr = NULL;
				XSIW2A(&ststr, stName.GetWideString());
				vertexSTTokenData = new xsiRibTokens;
				vertexSTTokenData->put(ststr, FloatType, false, true, true, 2 * numPoints);
				vertexSTTokenData->putDetailType(VertexDetail);
			} 
			else {
				CString stName = L"u_" + uvSetName[k];
				char* sstr = NULL;
				XSIW2A(&sstr, stName.GetWideString());
				faceVertexSTokenData = new xsiRibTokens;
				faceVertexSTokenData->put(sstr, FloatType, false, true, false, numFaceVertices);
				faceVertexSTokenData->putDetailType(FaceVaryingDetail);
				stName = L"v_" + uvSetName[k];
				char* tstr = NULL;
				XSIW2A(&tstr, stName.GetWideString());
				faceVertexTTokenData = new xsiRibTokens;
				faceVertexTTokenData->put(tstr, FloatType, false, true, false, numFaceVertices);
				faceVertexTTokenData->putDetailType(FaceVaryingDetail);
			}

			for (i = 0; i < numFaces; ++i) {
				Facet facet(facetRefArray.GetItem(i));

				pointRefArray = facet.GetPoints();
				pointIndexArray = pointRefArray.GetIndexArray();

				CSampleRefArray samples(facet.GetSamples());
				sampleIndexArray = samples.GetIndexArray();

				for (long j = 0; j < pointRefArray.GetCount(); ++j) {
					
					long sampleIndex = sampleIndexArray[j];

					vertex = pointIndexArray[j];
					float S, T = 0;

					if (lnbUV == numPoints) {
						S = (float)stValueArray[k][vertex*3];
						T = (float)stValueArray[k][vertex*3 + 1];
						vertexSTTokenData->assignFloat(vertex*2 + 0, S);
						vertexSTTokenData->assignFloat(vertex*2 + 1, 1 - T);
					} 
					else {
						S = (float)stValueArray[k][sampleIndex*3];
						T = (float)stValueArray[k][sampleIndex*3 + 1];
						faceVertexSTokenData->assignFloat(faceVertex, S);
						faceVertexTTokenData->assignFloat(faceVertex, 1 - T);
					}

					++faceVertex;
				}
			}
		
			// add tokens and cleanup
			if (vertexSTTokenData != NULL) {
				ribTokens.push_back(*vertexSTTokenData);
				delete vertexSTTokenData;
			}
			if (faceVertexSTokenData != NULL) {
				ribTokens.push_back(*faceVertexSTokenData);
				delete faceVertexSTokenData;
			}
			if (faceVertexTTokenData != NULL) {
				ribTokens.push_back(*faceVertexTTokenData);
				delete faceVertexTTokenData;
			} 
		}
	}

	// cleanup
	exclusionList.clear();
	inclusionList.clear();
}

xsiRibMeshData::~xsiRibMeshData()
{
	if (nverts != NULL) {delete nverts; nverts = NULL;}
	if (verts != NULL) {delete verts; verts = NULL;}
}

void xsiRibMeshData::write()
{
	RtInt* nloops = (RtInt*)alloca(sizeof(RtInt) * numFaces);
	for (long i = 0; i < numFaces; ++i) {
		nloops[i] = 1;
	}

	unsigned numTokens = ribTokens.size();
	RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
	RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);

	assignRibTokensV(&ribTokens, tokenArray, pointerArray);

	RiPointsGeneralPolygonsV(numFaces, nloops, nverts, verts, numTokens, tokenArray, pointerArray);
}

ObjectType xsiRibMeshData::type() const
{
	return MeshObject;
}

