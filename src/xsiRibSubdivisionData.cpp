//*****************************************************************************
// File: xsiRibSubdivisionData.cpp
// Description: xsiRibSubdivisionData object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "xsiRibSubdivisionData.h"
#include "xsiUtilities.h"
#include <xsi_material.h>
#include <xsi_texture.h>
#include <xsi_edge.h>
#include <xsi_vertex.h>

extern CString g_Renderer;

xsiRibSubdivisionData::xsiRibSubdivisionData(X3DObject in_obj, float cframe)
:	numFaces(0),
	nverts(NULL),
	verts(NULL),
	numFaceSets(1),
	scheme(L"catmull-clark"),
	interpolateBoundary(0)
{
	Application app;

	CRef propRef = in_obj.GetProperties().GetItem(L"XSIManSubdiv");
	if (propRef.IsValid()) {
		Property prop = propRef;
		CRefArray paramArray = prop.GetParameters();
		for (long j=0; j<paramArray.GetCount(); ++j) {
			Parameter param(paramArray[j]);
			CString paramName = param.GetScriptName();
			CValue paramValue = param.GetValue();
			if (paramName == L"subdivScheme") {
				scheme = paramValue;
			}
			else if (paramName == L"interpBoundary") {
				interpolateBoundary = paramValue;
			}
		}
	}

	nverts = NULL;
	verts = NULL;

	CLongArray pointIndexArray, nodeIndexArray;

	Geometry geom(in_obj.GetActivePrimitive().GetGeometry(cframe));
	PolygonMesh polymesh(geom);

	name = in_obj.GetName();

	// get the vertices
	CPointRefArray pointRefArray(polymesh.GetPoints());
	MATH::CVector3Array positionArray(pointRefArray.GetPositionArray());
	RtInt numPoints = positionArray.GetCount();

	// get the facets
	CFacetRefArray facetRefArray(polymesh.GetFacets());
	numFaces = facetRefArray.GetCount();

	// set points token data
	xsiRibTokens pointsTokenData;
	pointsTokenData.put("P", PointType, false, true, false, numPoints);
	pointsTokenData.putDetailType(VertexDetail);

	// allocate space
	nverts = new RtInt[numFaces];
	RtInt numFaceVertices(0);
	for (long i = 0; i < numFaces; ++i) {
		Facet facet(facetRefArray.GetItem(i));
		pointRefArray = facet.GetPoints();
		nverts[i] = pointRefArray.GetCount();
		numFaceVertices += pointRefArray.GetCount();
	}
	verts = new RtInt[numFaceVertices];

	// get the UV cluster
	CRefArray clusterRefArray;
	polymesh.GetClusters().Filter(siSampledPointCluster, CStringArray(), L"", clusterRefArray);
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

	// check for boundary info
	CEdgeRefArray edgeRefArray(polymesh.GetEdges());
	//interpolateBoundary = false;
	//for (i = 0; i < edgeRefArray.GetCount(); i++) {
		//Edge edge(edgeRefArray[i]);
		//interpolateBoundary = edge.GetIsBoundary();
		//if (interpolateBoundary)  break;
	//}

	// check edge clusters for crease info
	polymesh.GetClusters().Filter(siEdgeCluster, CStringArray(), L"", clusterRefArray);
	for (i = 0; i < clusterRefArray.GetCount(); i++) {
		Cluster cluster(clusterRefArray[i]);
		CClusterElementArray clsElemArray = cluster.GetElements();
		CLongArray values = clsElemArray.GetArray();
		long numEdges = values.GetCount();
		for (long j = 0; j < numEdges; ++j) {
			Edge edge(edgeRefArray[values[j]]);
			float crease = (float)edge.GetCrease();
			if (crease > 0) {
				CVertexRefArray vertices = edge.GetVertices();
				creases.push_back(crease);
				Vertex vertex0(vertices.GetItem(0));
				long index0 = vertex0.GetIndex();
				Vertex vertex1(vertices.GetItem(1));
				long index1 = vertex1.GetIndex();
				edgeverts.push_back(index0);
				edgeverts.push_back(index1);
			}
		}
	}

	// check vertex clusters for crease info
	CVertexRefArray vertexRefArray(polymesh.GetVertices());
	polymesh.GetClusters().Filter(siVertexCluster, CStringArray(), L"", clusterRefArray);
	for (i = 0; i < clusterRefArray.GetCount(); i++) {
		Cluster cluster(clusterRefArray[i]);
		CClusterElementArray clsElemArray = cluster.GetElements();
		CLongArray values = clsElemArray.GetArray();
		long numPoints = values.GetCount();
		for (long j = 0; j < numPoints; ++j) {
			Vertex vertex(vertexRefArray[values[j]]);
			float crease = (float)vertex.GetCrease();
			if (crease > 0) {
				corners.push_back(crease);
				long index = vertex.GetIndex();
				pointverts.push_back(index);
			}
		}
	}

	// check for shading groups attached to face sets
	addFaceEditTag(in_obj, cframe);

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
	long vertex;
	CLongArray sampleIndexArray;

	for (i = 0; i < numFaces; ++i) {
		Facet facet(facetRefArray.GetItem(i));
		pointRefArray = facet.GetPoints();
		pointIndexArray = pointRefArray.GetIndexArray();

		CSampleRefArray samples(facet.GetSamples());
		sampleIndexArray = samples.GetIndexArray();

		for (long j = 0; j < pointRefArray.GetCount(); ++j) {
			vertex = pointIndexArray[j];
			verts[faceVertex] = vertex;	
			
			// get the sample index
			long sampleIndex = sampleIndexArray[j];

			pointsTokenData.assignFloat(vertex, (float)positionArray[vertex].GetX(), 
				(float)positionArray[vertex].GetY(), (float)positionArray[vertex].GetZ());

			if (numSTs > 0) {
				float S, T = 0;

				if (lnbUV == numPoints) {
					S = (float)uvValueArray[vertex*3];
			        T = (float)uvValueArray[vertex*3 + 1];
					vertexSTTokenData->assignFloat(vertex * 2 + 0, S);
					vertexSTTokenData->assignFloat(vertex * 2 + 1, 1 - T);
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

			for (long j = 0; j < pointRefArray.GetCount(); ++j) {
				// get the sample index
				Sample sample(samples[j]);
				long sampleIndex = sample.GetIndex();
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

void xsiRibSubdivisionData::addFaceEditTag(X3DObject in_obj, float cframe)
{
	Geometry geom(in_obj.GetActivePrimitive().GetGeometry(cframe));
	PolygonMesh polymesh(geom);

	CRefArray polyClusterRefArray;
	geom.GetClusters().Filter(siPolygonCluster, CStringArray(), L"", polyClusterRefArray);
    numFaceSets = polyClusterRefArray.GetCount();

	faces.Resize(geom.GetFacets().GetCount());
	sortOrder.Resize(geom.GetFacets().GetCount());

	for (int i=0; i<faces.GetCount(); i++) {
		faces[i] = -1000000;
		sortOrder[i] = -1000000;
	}

	for (int i=0; i<numFaceSets; ++i) {
		Cluster cluster(polyClusterRefArray[i]);
        faceSets.push_back(cluster.GetFullName());
		CLongArray indexArray = cluster.GetElements().GetArray();
        long sort_order = cluster.GetParameterValue(CString(L"sort_order"));
		for (int j=0; j<indexArray.GetCount(); j++) {
			if (sort_order > sortOrder[indexArray[j]]) {
				sortOrder[indexArray[j]] = sort_order;
				faces[indexArray[j]] = i;
			}
		}
    }
}

xsiRibSubdivisionData::~xsiRibSubdivisionData()
{
	if (nverts != NULL) {delete nverts; nverts = NULL;}
	if (verts != NULL) {delete verts; verts = NULL;}
	creases.clear();
	edgeverts.clear();
	corners.clear();
	pointverts.clear();    
	faces.Clear(); 
	sortOrder.Clear();
	faceSets.clear();
}

void xsiRibSubdivisionData::write()
{
	Application app;

    // set resource handles
	rsrcHandles.Resize(numFaceSets);
    for (int i=0; i<numFaceSets; ++i) {
        rsrcHandles[i] = faceSets[i];
    }

    // sort faces
    int numFaceIndices = faces.GetCount();
	CLongArray* faceList = new CLongArray[numFaceSets];
    for (int i=0; i<numFaceSets; ++i) {
		for (int j=0; j<numFaceIndices; ++j) {
			if (faces[j] == i && faces[j] != -1000000) {
				faceList[i].Add(j);
			}
        }
    }

	RtToken* tags = NULL;
	RtInt ntags = 0;
	RtInt* nargs = NULL;
	RtInt* intargs = NULL;
	RtFloat* floatargs = NULL;
    RtToken* stringargs = NULL;

	long ncreases = creases.size();
	long ncorners = corners.size();
    long creaseMethod = 0;
	if (g_Renderer == L"PRMan") creaseMethod = 1;
	if (g_Renderer == L"Air") numFaceSets = 0;
	

	if (interpolateBoundary || ncreases || ncorners || numFaceSets)  {	
		tags = (RtToken*)alloca(sizeof(RtToken)*(creaseMethod + (long)interpolateBoundary + ncreases + ncorners + numFaceSets));
		nargs = (RtInt*) alloca(sizeof(RtInt) * 3 * (creaseMethod + (long)interpolateBoundary + ncreases + ncorners + numFaceSets));
		intargs = (RtInt*) alloca(sizeof(RtInt) * (1 + 2 * ncreases + ncorners + 2 * numFaceIndices));
		floatargs = (RtFloat*) alloca(sizeof(RtFloat) * (ncreases + ncorners));
		stringargs = (RtToken*) alloca(sizeof(RtToken) * (creaseMethod + 3*numFaceSets));

		long sindex = 0, nindex = 0, findex = 0, vindex = 0;

        // use new crease method
		if (creaseMethod) {
			tags[0] = "creasemethod";
			nargs[nindex] = 0;
			nargs[nindex+1] = 0;
			nargs[nindex+2] = 1;
			stringargs[sindex] = "chaikin";
			ntags++;
			nindex += 3;
			sindex++;
		}

		if (interpolateBoundary) {
            tags[ntags] = "interpolateboundary";
            nargs[nindex] = 1;
            nargs[nindex+1] = 0;
            nargs[nindex+2] = 0;
            intargs[vindex] = 1;
            ntags++;
            nindex += 3;
            vindex++;
        }

        for (int i=0; i<numFaceSets; ++i) {
            tags[ntags] = "faceedit";
            nargs[nindex] = 2*faceList[i].GetCount();
            nargs[nindex+1] = 0;
            nargs[nindex+2] = 3;
            for (int j=0; j<faceList[i].GetCount(); ++j) {
                intargs[vindex] = 1;
                intargs[vindex+1] = faceList[i][j];
                vindex += 2;
            }

            stringargs[sindex] = "attributes";
			char* rsrchandle;
			XSIW2A(&rsrchandle, rsrcHandles[i].GetWideString());
            stringargs[sindex+1] = (RtToken)rsrchandle;
            stringargs[sindex+2] = "shading";
            ntags++;
            nindex += 3;
            sindex += 3;
        };

        vector<float>::iterator iter;
        vector<long>::iterator jter;
        jter=edgeverts.begin();
        for (iter=creases.begin(); iter!=creases.end(); ++iter) {
            tags[ntags] = "crease";
            nargs[nindex] = 2;
            nargs[nindex+1] = 1;
            nargs[nindex+2] = 0;
            floatargs[findex] = (*iter);
            intargs[vindex] = (*jter);
            ++jter;
            intargs[vindex+1] = (*jter);
            ++jter;
            ntags++;
            nindex += 3;
            findex++;
            vindex += 2;
        }

        jter=pointverts.begin();
        for (iter=corners.begin(); iter!=corners.end(); ++iter) {
            tags[ntags] = "corner";
            nargs[nindex] = 1;
            nargs[nindex+1] = 1;
            nargs[nindex+2] = 0;
            floatargs[findex] = (*iter);
            intargs[vindex] = (*jter);
            ++jter;
            ntags++;
            nindex += 3;
            findex++;
            vindex++;
        }
	}

	unsigned numTokens = ribTokens.size();
	RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
	RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
				
	char* subdivScheme = NULL;
	XSIW2A(&subdivScheme, scheme.GetWideString());

	assignRibTokensV(&ribTokens, tokenArray, pointerArray);

	if (g_Renderer == L"Air") {
		RiSubdivisionMeshV(subdivScheme, numFaces, nverts, verts, ntags, tags, nargs, intargs, floatargs, numTokens, tokenArray, pointerArray);
	}
	else {
		RiHierarchicalSubdivisionMeshV(subdivScheme, numFaces, nverts, verts, ntags, tags, nargs, intargs, floatargs, stringargs, numTokens, tokenArray, pointerArray);	

	}
}

ObjectType xsiRibSubdivisionData::type() const
{
	return SubdivisionObject;
}
