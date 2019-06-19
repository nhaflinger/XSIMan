//*****************************************************************************
// File: xsiRibTranslator.cpp
// Description: xsiRibTranslator object class.
//
// Author: Doug Creel
//         Graphic Primitives, LLC
//         Copyright (C) 2005-2006. All Rights Reserved
//
// The RenderMan (R) Interface Procedures and Protocol are:
// Copyright 1988, 1989, Pixar. All Rights Reserved
//
//*****************************************************************************

#include "stdafx.h"

#include "xsiRibTranslator.h"
#include "xsiRibCameraData.h"
#include "xsiRibLightData.h"
#include "xsiRibMap.h"
#include "xsiRibMeshData.h"
#include "xsiRibSurfaceData.h"
#include "xsiRibCurveData.h"
#include "xsiRibSubdivisionData.h"
#include "xsiRibNullData.h"
#include "xsiRibParticlesData.h"
#include "xsiRibHairData.h"
#include "xsiUtilities.h"
#include <SemanticLayer.h>
#include <xsi_application.h>
#include <xsi_model.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_nurbsdata.h>
#include <xsi_null.h>
#include <xsi_camera.h>
#include <xsi_light.h>
#include <xsi_ref.h>
#include <xsi_primitive.h>
#include <xsi_project.h>
#include <xsi_scene.h>
#include <xsi_ogllight.h>
#include <xsi_point.h>
#include <xsi_facet.h>
#include <xsi_polygonnode.h>
#include <xsi_cluster.h>
#include <xsi_clusterproperty.h>
#include <xsi_shader.h>
#include <xsi_math.h>
#include <xsi_decl.h>
#include <xsi_uitoolkit.h>
#include <xsi_progressbar.h>
#include <xsi_source.h>
#include <xsi_imageclip.h>
#include <xsi_imageclip2.h>
#include <xsi_group.h>
#include <xsi_ppglayout.h>
#include <xsi_selection.h>
#include <xsi_griddata.h>
#include <xsi_gridwidget.h>
#include <xsi_pass.h>
#include <xsi_particlecloud.h>
#include <xsi_particlecloudprimitive.h>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#ifdef LINUX 
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace XSI;

extern XSI::CStatus CValueTOSIVariant(XSI::CValue& in_value, SI_TinyVariant* io_slVariant);


// Global data
multimap<JobType, jobStruct*> jobList;
signed g_MotionSamples;
signed g_MaxMotionSamples;
float g_CurrentFrame;
bool g_MotionBlur;
bool g_DeformBlur;
bool g_CameraBlur;
CString g_CameraName;
CString g_RibPath;
CString g_JobPath;
CString g_PixPath;
CString g_ShaderPath;
CString g_TexPath;
CString g_ShdPath;
CString g_SloPath;
CString g_DsoPath;
CString g_ImgPath;
float g_ShadowMapBias;
CString g_DicingStrategy;
CString g_DicingCamera;
bool g_RasterOriented;
bool g_EnableRayTracing;
signed g_MaxRayDepth;
float g_RayTraceBias;
float g_RayTraceBreadthFactor;
float g_RayTraceDepthFactor;
signed g_RayTraceMaxSpecularDepth;
signed g_RayTraceMaxDiffuseDepth;
bool g_RayTraceDisplacements;
bool g_RayTraceSampleMotion;
bool g_EnableAmbientOcclusion;
long g_OcclusionSamples;
float g_OcclusionMaxdist;
bool g_EnableIndirectIllumination;
long g_IndirectSamples;
float g_RayTraceMaxError;
float g_RayTraceMaxPixDist;
CString g_IrradianceCache;
CString g_IrradianceCacheMode;
CString g_PhotonShadingModel;
CString g_PhotonCausticMap;
CString g_PhotonGlobalMap;
long g_PhotonEstimator;
bool g_BinaryRib;
bool g_GzipRib;
bool g_ExportAsArchive;
signed short g_FramePadding;
bool g_RenderFrames;
long g_StartFrame;
long g_EndFrame;
long g_StepSize;
bool g_SkipRenderedFrames;
CString g_RenderMode;
unsigned g_Cleanup;
CString g_ShutterTiming;
bool g_FrontplaneGeom;
bool g_BackplaneGeom;
GridData g_AOVList;
bool g_PrimaryDisplay;
bool g_UseJobScript;
CString g_JobScriptType;
signed g_MinProcs;
signed g_MaxProcs;
CString g_AlfredTags;
CString g_AlfredServices;
CString g_Imager;
bool g_RemoteCmd;
CString g_PreJobCommand;
CString g_PreFrameCommand;
CString g_PostFrameCommand;
CString g_PostJobCommand;
bool g_CleanJobScript;
CString g_PreJobScript;
CString g_PreFrameScript;
CString g_PostFrameScript;
CString g_PostJobScript;
CString g_ScriptLanguage;
bool g_LaunchRender;
//bool g_BypassRib;
CString g_Renderer;
CString g_RenderCommand;
CString g_CustomCommand;
signed g_NumProcs;
bool g_ShowProgress;
long g_RenderStatistics;
CString g_StatisticsFileName;
CString g_StatisticsXMLFileName;
bool g_JobSummary;
bool g_RenderSelectedObjects;
bool g_RenderSelectedLights;
bool g_ApproximatedMesh;
bool g_PolygonClusters;
bool g_InstanceGeometry;
bool g_ReuseInstanceGeometry;
bool g_RenderTreeSupport;
float g_MotionFactor;
float g_OpacityThresholdR;
float g_OpacityThresholdG;
float g_OpacityThresholdB;
float g_OpacityCullingR;
float g_OpacityCullingG;
float g_OpacityCullingB;
CString g_CurrentNode;
bool g_BinaryDicing;
bool prejobState;
bool g_BatchMode;
bool g_NamedHandles;
CString g_HandlePrefix;


xsiRibTranslator::xsiRibTranslator()
{
	m_sceneRoot = m_app.GetActiveSceneRoot();

	// set globals
	g_MotionSamples = 2;
	g_MaxMotionSamples = 6;
	g_CurrentFrame = 0;
	g_MotionBlur = false;
	g_DeformBlur = false;
	g_CameraBlur = false;
	g_CameraName = L"Camera";
	g_ShadowMapBias = 1.0f;
	g_DicingStrategy = L"planarprojection";
	g_DicingCamera = L"worldcamera";
	g_RasterOriented = true;
	g_EnableRayTracing = false;
	g_MaxRayDepth = 10;
	g_RayTraceBias = 0.05f;
	g_RayTraceBreadthFactor = 1;
	g_RayTraceDepthFactor = 1;
	g_RayTraceMaxSpecularDepth = 2;
	g_RayTraceMaxDiffuseDepth = 1;
	g_RayTraceDisplacements = false;
	g_RayTraceSampleMotion = false;
	g_EnableAmbientOcclusion = false;
	g_OcclusionSamples = 16;
	g_OcclusionMaxdist = 1000000.0f;
	g_EnableIndirectIllumination = false;
	g_IndirectSamples = 16;
	g_RayTraceMaxError = -1;
	g_RayTraceMaxPixDist = -1;
	g_IrradianceCache = L"";
	g_IrradianceCacheMode = L"";
	g_PhotonShadingModel = L"";
	g_PhotonCausticMap = L"";
	g_PhotonGlobalMap = L"";
	g_PhotonEstimator = 50;
	g_BinaryRib = false;
	g_GzipRib = false;
	g_ExportAsArchive = false;
	g_FramePadding = 1;
	g_RenderFrames = false;
	g_StartFrame = 1;
	g_EndFrame = 100;
	g_StepSize = 1;
	g_SkipRenderedFrames = false;
	g_RenderMode = L"immediate";
	g_RibPath = L"";
	g_JobPath = L"";
	g_PixPath = L"";
	g_TexPath = L"";
	g_ShdPath = L"";
	g_ShaderPath = L"";
	g_SloPath = L"";
	g_DsoPath = L"";
	g_ImgPath = L"";
	g_Cleanup = 0;
	g_ShutterTiming = L"OpenOnFrame";
	g_FrontplaneGeom = false;
	g_BackplaneGeom = false;
	g_UseJobScript = false;
	g_JobScriptType = L"alfred";
	g_AlfredTags = L"prman";
	g_AlfredServices = L"pixarRender";
	g_MinProcs = 1;
	g_MaxProcs = 3;
	g_CleanJobScript = true;
	g_PreJobScript = L"";
	g_PreFrameScript = L"";
	g_PostFrameScript = L"";
	g_PostJobScript = L"";
	g_ScriptLanguage = L"VBScript";
	g_LaunchRender = true;
	//g_BypassRib = false;
	g_Renderer = L"PRMan";
	g_RenderCommand = L"prman";
	g_CustomCommand = L"";
	g_NumProcs = 1;
	g_ShowProgress = false;
	g_RenderStatistics = 0;
	g_StatisticsFileName = L"";
	g_StatisticsXMLFileName = L"";
	g_JobSummary = false;
	g_RenderSelectedObjects = false;
	g_RenderSelectedLights = false;
	g_ApproximatedMesh = false;
	g_PolygonClusters = false;
	g_InstanceGeometry = false;
	g_ReuseInstanceGeometry = false;
	g_RenderTreeSupport = false;
	g_RemoteCmd = false;
	g_Imager = L"sho";
	g_PreJobCommand = L"";
	g_PreFrameCommand = L"";
	g_PostFrameCommand = L"";
	g_PostJobCommand = L"";
	g_MotionFactor = 1.0f;
	g_OpacityThresholdR = 1.0f;
	g_OpacityThresholdG = 1.0f;
	g_OpacityThresholdB = 1.0f;
	g_OpacityCullingR = 0.996f;
	g_OpacityCullingG = 0.996f;
	g_OpacityCullingB = 0.996f;
	g_BinaryDicing = false;
	prejobState = true;
    g_BatchMode = false;
	g_NamedHandles = false;
	g_HandlePrefix = false;

	// default RenderMan settings
	bucketSize[0] = 16;
	bucketSize[1] = 16;
	gridSize = 256;
	textureMemory = 8192;
	eyeSplits = 10;
	shadowBias = 1.0f;
	pixelSamples[0] = 3;
	pixelSamples[1] = 3;
	shadingRate = 1;
	pixelFilter = CatmullRomFilter;
	filterSize[0] = 2;
	filterSize[1] = 2;
	quantizeOne = 255;
	quantizeMin = 0;
	quantizeMax = 255;
	dither = 0.5;
	gainValue = 1.0;
	gammaValue = 1.0;
	cropWindowX1 = 0;
	cropWindowY1 = 0;
	cropWindowX2 = 1;
	cropWindowY2 = 1;
	job = new jobStruct;
	job->name = L"beauty";
	job->width = 640;
	job->height = 486;
	job->aspectRatio = 1.;
	job->format = L"file";
	job->imageMode = L"rgba";
	job->imageName = L"untitled";
	job->ribFileName = L"untitled";
	job->camera[0].nearClip = 0.01f;
	job->camera[0].farClip = 1000.f;
	job->camera[0].orthoWidth = 1.f;
	job->camera[0].orthoHeight = 1.f;
	job->camera[0].focalDistance = 0.f;
	job->camera[0].focalLength = 50.f;
	job->camera[0].fStop = 0.f;
	job->camera[0].horizFOV = 1.0f;
	job->camera[0].motionBlur = g_MotionBlur;
	job->camera[0].shutter = 0.5f;
	job->camera[0].orthogonal = false;
	job->camera[0].name = g_CameraName;
	job->camera[0].matrix4.SetIdentity();
	job->reuseData = false;
	job->isPointLight = false;
	job->isEnvmap = false;
	job->cubeMapDir = 0;
	job->isPhotonMap = false;
	job->photons = 10000;
	job->photonMapType = L"Caustic";
	job->fileExt = L"";
	job->writeFrequency = L"Every Frame";
	job->rewriteData = true;
	job->jobType = BeautyJob;

	// shader network data
	shaderNetworkData();
}

xsiRibTranslator::~xsiRibTranslator()
{
	delete job;
	jobList.clear();
	prejobList.clear();
	preframeList.clear();
	renderList.clear();
	cleanupList.clear();
	ribTokens.clear();
	shaderFuncs.clear();
	shaderArgs.clear();
	shaderArgDefs.clear();
	shaderArgTypes.clear();
	shaderTemplates.clear();
	shaderList.clear();
	shaderParamList.clear();
	lightSwitch.clear();
}

void xsiRibTranslator::shaderNetworkData() 
{
	// shader functions database
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"unimplemented", L"unimplemented"));
	
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"global", L"global"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt2d-image-explicit.1", L"txt2d_image_explicit"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt2d-scalarimage-explicit.1", L"txt2d_scalarimage_explicit"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"image_clip", L"image_clip"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-cellular_v3.1", L"txt3d_cellular_v3"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-checkerboard.1", L"txt3d_checkerboard"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-cloud.1", L"txt3d_cloud"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-fractal_v3.1", L"txt3d_fractal_v3"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_bumpmap_gen.1", L"sib_bumpmap_gen"));
	
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_environment.1", L"sib_environment"));
	
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_color_intensity.1", L"mib_color_intensity"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_invert.1", L"sib_color_invert"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_correction.1", L"sib_color_correction"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_combine.1", L"sib_color_combine"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_hsva_combine.1", L"sib_hsva_combine"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_hlsa_combine.1", L"sib_hlsa_combine"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_twosided.1", L"mib_twosided"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_switch.1", L"sib_scalar_switch"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_switch.1", L"sib_vector_switch"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_switch.1", L"sib_color_switch"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_2mix.1", L"sib_color_2mix"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_hsv_to_rgb.1", L"sib_color_hsv_to_rgb"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_rgb_to_hsv.1", L"sib_color_rgb_to_hsv"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_to_scalar.1", L"sib_color_to_scalar"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_to_vector.1", L"sib_color_to_vector"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_to_color.1", L"sib_vector_to_color"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_integer_to_scalar.1", L"sib_integer_to_scalar"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_to_color.1", L"sib_scalar_to_color"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_to_integer.1", L"sib_scalar_to_integer"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalars_to_vector.1", L"sib_scalars_to_vector"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_to_scalar.1", L"sib_vector_to_scalar"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-phong.1", L"material_phong"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-blinn.1", L"material_blinn"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-lambert.1", L"material_lambert"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-cooktorr.1", L"material_cooktorr"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-ward.1", L"material_ward"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-constant.1", L"material_constant"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-strauss.1", L"material_strauss"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_phong.1", L"simple_phong"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_lambert.1", L"simple_lambert"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_blinn.1", L"simple_blinn"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_cooktorr.1", L"simple_cooktorr"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_aniward.1", L"simple_ward"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_translucent.1", L"simple_translucent"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.rh_renderer.1", L"rh_renderer"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.XSIAmbientOcclusion.1", L"xsiambientocclusion"));
	//shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_incidence.1", L"sib_incidence"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_shadowmaterial.1", L"sib_illum_shadowmaterial"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_incidence_v2.1", L"sib_incidence_v2"));
		
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_state_scalar.1", L"sib_state_scalar"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_state_vector.1", L"sib_state_vector"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.dense.vol.1", L"dense_vol"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.soft_fog.1", L"soft_fog"));
	
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_reflect.1", L"mib_reflect"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_diffuse_reflection.1", L"sib_diffuse_reflection"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_refract.1", L"mib_refract"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_diffuse_refraction_phen.1", L"sib_diffuse_refraction_phen"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_opacity.1", L"mib_opacity"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_transparency.1", L"sib_transparency"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_dielectric_ior.1", L"sib_dielectric_ior"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_basic.1", L"sib_color_math_basic"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_exponent.1", L"sib_color_math_exponent"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_unary.1", L"sib_color_math_unary"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_logic.1", L"sib_color_math_logic"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_color_average.1", L"mib_color_average"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_interp_linear.1", L"sib_interp_linear"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_basic.1", L"sib_scalar_math_basic"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_exponent.1", L"sib_scalar_math_exponent"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_unary.1", L"sib_scalar_math_unary"));
	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_logic.1", L"sib_scalar_math_logic"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.misss_fast_simple_phen.1", L"misss_fast_simple_phen"));

	shaderFuncs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.bg_pic.out.1", L"bg_pic_out"));

	// shader function arguments database
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"unimplemented", L"UNIMPLEMENTED_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt2d-image-explicit.1", L"TXT2D_IMAGE_EXPLICIT_PARAMS, IMAGE_CLIP_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt2d-scalarimage-explicit.1", L"TXT2D_SCALARIMAGE_EXPLICIT_PARAMS, IMAGE_CLIP_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"image_clip", L"IMAGE_CLIP_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-cellular_v3.1", L"TXT3D_CELLULAR_V3_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-checkerboard.1", L"TXT3D_CHECKERBOARD_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-cloud.1", L"TXT3D_CLOUD_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-fractal_v3.1", L"TXT3D_FRACTAL_V3_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_bumpmap_gen.1", L"SIB_BUMPMAP_GEN_PARAMS, IMAGE_CLIP_PARAMS"));
	
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_environment.1", L"SIB_ENVIRONMENT_PARAMS, IMAGE_CLIP_PARAMS"));
	
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_color_intensity.1", L"MIB_COLOR_INTENSITY_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_invert.1", L"SIB_COLOR_INVERT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_correction.1", L"SIB_COLOR_CORRECTION_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_2mix.1", L"SIB_COLOR_2MIX_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_combine.1", L"SIB_COLOR_COMBINE_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_hsva_combine.1", L"SIB_HSVA_COMBINE_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_hlsa_combine.1", L"SIB_HLSA_COMBINE_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_twosided.1", L"MIB_TWOSIDED_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_switch.1", L"SIB_SCALAR_SWITCH_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_switch.1", L"SIB_VECTOR_SWITCH_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_switch.1", L"SIB_COLOR_SWITCH_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_hsv_to_rgb.1", L"SIB_COLOR_HSV_TO_RGB_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_rgb_to_hsv.1", L"SIB_COLOR_RGB_TO_HSV_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_to_scalar.1", L"SIB_COLOR_TO_SCALAR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_to_vector.1", L"SIB_COLOR_TO_VECTOR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_to_color.1", L"SIB_VECTOR_TO_COLOR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_integer_to_scalar.1", L"SIB_INTEGER_TO_SCALAR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_to_color.1", L"SIB_SCALAR_TO_COLOR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_to_integer.1", L"SIB_SCALAR_TO_INTEGER_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalars_to_vector.1", L"SIB_SCALARS_TO_VECTOR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_to_scalar.1", L"SIB_VECTOR_TO_SCALAR_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-phong.1", L"MATERIAL_PHONG_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-blinn.1", L"MATERIAL_BLINN_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-lambert.1", L"MATERIAL_LAMBERT_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-cooktorr.1", L"MATERIAL_COOKTORR_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-ward.1", L"MATERIAL_WARD_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-constant.1", L"MATERIAL_CONSTANT_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-strauss.1", L"MATERIAL_STRAUSS_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_phong.1", L"SIMPLE_PHONG_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_lambert.1", L"SIMPLE_LAMBERT_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_blinn.1", L"SIMPLE_BLINN_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_cooktorr.1", L"SIMPLE_COOKTORR_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_aniward.1", L"SIMPLE_WARD_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_translucent.1", L"SIMPLE_TRANSLUCENT_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.rh_renderer.1", L"RH_RENDERER_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.XSIAmbientOcclusion.1", L"XSIAMBIENTOCCLUSION_PARAMS, GLOBAL_PARAMS"));
	//shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_incidence.1", L"SIB_INCIDENCE_PARAMS, GLOBAL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_shadowmaterial.1", L"SIB_ILLUM_SHADOWMATERIAL_PARAMS, GLOBAL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_incidence_v2.1", L"SIB_INCIDENCE_V2_PARAMS, GLOBAL_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_state_scalar.1", L"SIB_STATE_SCALAR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_state_vector.1", L"SIB_STATE_VECTOR_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.dense.vol.1", L"DENSE_VOL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.soft_fog.1", L"SOFT_FOG_PARAMS"));
	
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_reflect.1", L"MIB_REFLECT_PARAMS, GLOBAL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_diffuse_reflection.1", L"SIB_DIFFUSE_REFLECTION_PARAMS, GLOBAL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_refract.1", L"MIB_REFRACT_PARAMS, GLOBAL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_diffuse_refraction_phen.1", L"SIB_DIFFUSE_REFRACTION_PHEN_PARAMS, GLOBAL_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_opacity.1", L"MIB_OPACITY_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_transparency.1", L"SIB_TRANSPARENCY_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_dielectric_ior.1", L"SIB_DIELECTRIC_IOR_PARAMS"));
	
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_basic.1", L"SIB_COLOR_MATH_BASIC_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_exponent.1", L"SIB_COLOR_MATH_EXPONENT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_unary.1", L"SIB_COLOR_MATH_UNARY_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_logic.1", L"SIB_COLOR_MATH_LOGIC_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_color_average.1", L"MIB_COLOR_AVERAGE_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_interp_linear.1", L"SIB_INTERP_LINEAR_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_basic.1", L"SIB_SCALAR_MATH_BASIC_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_exponent.1", L"SIB_SCALAR_MATH_EXPONENT_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_unary.1", L"SIB_SCALAR_MATH_UNARY_PARAMS"));
	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_logic.1", L"SIB_SCALAR_MATH_LOGIC_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.misss_fast_simple_phen.1", L"MISSS_FAST_SIMPLE_PHEN_PARAMS, GLOBAL_PARAMS, OUTPUT_PARAMS"));

	shaderArgs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.bg_pic.out.1", L"BG_PIC_OUT_PARAMS, IMAGE_CLIP_PARAMS"));

	// shader function arguments definitions database
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"unimplemented", L"DEF_UNIMPLEMENTED_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt2d-image-explicit.1", L"DEF_TXT2D_IMAGE_EXPLICIT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt2d-scalarimage-explicit.1", L"DEF_TXT2D_SCALARIMAGE_EXPLICIT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"image_clip", L"DEF_IMAGE_CLIP_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-cellular_v3.1", L"DEF_TXT3D_CELLULAR_V3_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-checkerboard.1", L"DEF_TXT3D_CHECKERBOARD_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-cloud.1", L"DEF_TXT3D_CLOUD_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.txt3d-fractal_v3.1", L"DEF_TXT3D_FRACTAL_V3_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_bumpmap_gen.1", L"DEF_SIB_BUMPMAP_GEN_PARAMS"));
	
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_environment.1", L"DEF_SIB_ENVIRONMENT_PARAMS"));
	
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_color_intensity.1", L"DEF_MIB_COLOR_INTENSITY_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_invert.1", L"DEF_SIB_COLOR_INVERT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_correction.1", L"DEF_SIB_COLOR_CORRECTION_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_2mix.1", L"DEF_SIB_COLOR_2MIX_PARAMS"));
	
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_combine.1", L"DEF_SIB_COLOR_COMBINE_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_hsva_combine.1", L"DEF_SIB_HSVA_COMBINE_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_hlsa_combine.1", L"DEF_SIB_HLSA_COMBINE_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_twosided.1", L"DEF_MIB_TWOSIDED_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_switch.1", L"DEF_SIB_SCALAR_SWITCH_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_switch.1", L"DEF_SIB_VECTOR_SWITCH_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_switch.1", L"DEF_SIB_COLOR_SWITCH_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_hsv_to_rgb.1", L"DEF_SIB_COLOR_HSV_TO_RGB_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_rgb_to_hsv.1", L"DEF_SIB_COLOR_RGB_TO_HSV_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_to_scalar.1", L"DEF_SIB_COLOR_TO_SCALAR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_to_vector.1", L"DEF_SIB_COLOR_TO_VECTOR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_to_color.1", L"DEF_SIB_VECTOR_TO_COLOR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_integer_to_scalar.1", L"DEF_SIB_INTEGER_TO_SCALAR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_to_color.1", L"DEF_SIB_SCALAR_TO_COLOR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_to_integer.1", L"DEF_SIB_SCALAR_TO_INTEGER_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalars_to_vector.1", L"DEF_SIB_SCALARS_TO_VECTOR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_vector_to_scalar.1", L"DEF_SIB_VECTOR_TO_SCALAR_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-phong.1", L"DEF_MATERIAL_PHONG_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-blinn.1", L"DEF_MATERIAL_BLINN_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-lambert.1", L"DEF_MATERIAL_LAMBERT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-cooktorr.1", L"DEF_MATERIAL_COOKTORR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-ward.1", L"DEF_MATERIAL_WARD_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-constant.1", L"DEF_MATERIAL_CONSTANT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.material-strauss.1", L"DEF_MATERIAL_STRAUSS_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_phong.1", L"DEF_SIMPLE_PHONG_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_lambert.1", L"DEF_SIMPLE_LAMBERT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_blinn.1", L"DEF_SIMPLE_BLINN_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_cooktorr.1", L"DEF_SIMPLE_COOKTORR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.simple_aniward.1", L"DEF_SIMPLE_WARD_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_translucent.1", L"DEF_SIMPLE_TRANSLUCENT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.rh_renderer.1", L"DEF_RH_RENDERER_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.XSIAmbientOcclusion.1", L"DEF_XSIAMBIENTOCCLUSION_PARAMS"));
	//shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_incidence.1", L"DEF_SIB_INCIDENCE_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_illum_shadowmaterial.1", L"DEF_SIB_ILLUM_SHADOWMATERIAL_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_incidence_v2.1", L"DEF_SIB_INCIDENCE_V2_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_state_scalar.1", L"DEF_SIB_STATE_SCALAR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_state_vector.1", L"DEF_SIB_STATE_VECTOR_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.dense.vol.1", L"DEF_DENSE_VOL_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.soft_fog.1", L"DEF_SOFT_FOG_PARAMS"));
	
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_reflect.1", L"DEF_MIB_REFLECT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_diffuse_reflection.1", L"DEF_SIB_DIFFUSE_REFLECTION_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_refract.1", L"DEF_MIB_REFRACT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_diffuse_refraction_phen.1", L"DEF_SIB_DIFFUSE_REFRACTION_PHEN_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_opacity.1", L"DEF_MIB_OPACITY_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_transparency.1", L"DEF_SIB_TRANSPARENCY_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_dielectric_ior.1", L"DEF_SIB_DIELECTRIC_IOR_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_basic.1", L"DEF_SIB_COLOR_MATH_BASIC_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_exponent.1", L"DEF_SIB_COLOR_MATH_EXPONENT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_unary.1", L"DEF_SIB_COLOR_MATH_UNARY_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_color_math_logic.1", L"DEF_SIB_COLOR_MATH_LOGIC_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.mib_color_average.1", L"DEF_MIB_COLOR_AVERAGE_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_interp_linear.1", L"DEF_SIB_INTERP_LINEAR_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_basic.1", L"DEF_SIB_SCALAR_MATH_BASIC_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_exponent.1", L"DEF_SIB_SCALAR_MATH_EXPONENT_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_unary.1", L"DEF_SIB_SCALAR_MATH_UNARY_PARAMS"));
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.sib_scalar_math_logic.1", L"DEF_SIB_SCALAR_MATH_LOGIC_PARAMS"));

	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.misss_fast_simple_phen.1", L"DEF_MISSS_FAST_SIMPLE_PHEN_PARAMS"));
	
	shaderArgDefs.insert(pair<const wchar_t*, wchar_t*>(L"Softimage.bg_pic.out.1", L"DEF_BG_PIC_OUT_PARAMS"));

	// shader parameter database
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"unimplemented_input", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"global_environment", L"color"));
		
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_tex", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_bump_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_image_explicit_tspace_id", L"string"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_tex", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_bump_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt2d_scalarimage_explicit_tspace_id", L"string"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Source_File", L"string"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Display_Gamma", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Display_Gamma_Affects_Rendering", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Brightness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Gain", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Saturation", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Hue", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Amount", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Radius", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_XRes", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_YRes", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Flip_Horizontal", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Flip_Vertical", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_X_Min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_X_Max", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Y_Min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"image_clip_Y_Max", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_color1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_color2", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_alpha_output", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_alpha_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_bump_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_alt_x", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_alt_y", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cellular_v3_alt_z", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_color1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_color2", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_xsize", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_ysize", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_zsize", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_alpha_output", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_alpha_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_bump_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_alt_x", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_alt_y", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_checkerboard_alt_z", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_color1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_color2", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_contrast", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_noise_type", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_iter_max", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_level_min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_level_decay", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_freq_mul", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_time", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_alt_x", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_alt_y", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_alt_z", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_bump_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_alpha_output", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_alpha_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_cloud_absolute", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_color1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_color2", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_time", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_noise_type", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_absolute", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_iterations", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_level_min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_level_decay", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_freq_mul", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_threshold", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_upper_bound", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_diffusion", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_alt_x", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_alt_y", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_alt_z", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_step", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_bump_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_alpha_output", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"txt3d_fractal_v3_alpha_factor", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_bumpmap_gen_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_bumpmap_gen_repeats", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_bumpmap_gen_min", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_bumpmap_gen_max", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_bumpmap_gen_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_bumpmap_gen_factor", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_environment_tex", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_environment_reflection_intensity", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_color_intensity_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_color_intensity_input", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_invert_input", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_correction_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_correction_gamma", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_correction_contrast", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_correction_hue", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_correction_saturation", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_correction_level", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_2mix_base_color", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_2mix_color1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_2mix_weight1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_2mix_mode1", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_twosided_front", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_twosided_back", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_switch_input", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_switch_scalar1", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_switch_scalar2", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_switch_input", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_switch_vector1", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_switch_vector2", L"vector"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_switch_switch", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_switch_input1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_switch_input2", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_combine_red", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_combine_green", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_combine_blue", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_combine_alpha", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hsva_combine_hue", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hsva_combine_sat", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hsva_combine_val", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hsva_combine_alpha", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hlsa_combine_hue", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hlsa_combine_sat", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hlsa_combine_lum", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_hlsa_combine_alpha", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_scalar_alpha", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_scalar_input", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_rgb_to_hsv_rgba", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_hsv_to_rgb_hsva", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_vector_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_vector_modeR", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_vector_modeG", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_vector_modeB", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_to_vector_modeA", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_to_color_input", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_to_color_modeX", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_to_color_modeY", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_to_color_modeZ", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_integer_to_scalar_input", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_to_color_input", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_to_color_alpha", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_to_integer_input", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_inputX", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_inputY", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_inputZ", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_modeX", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_modeY", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_modeZ", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalars_to_vector_math_op", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_to_scalar_input", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_vector_to_scalar_component", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_bump", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_diffuse_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_specular_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_specular", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_shiny", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_translucent_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_translucency", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_phong_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_diffuse_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_specular_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_specular", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_roughness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_translucent_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_translucency", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_blinn_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_bump", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_diffuse_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_translucent_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_translucency", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_lambert_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_diffuse_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_specular_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_specular", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_roughness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_translucent_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_translucency", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_cooktorr_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_diffuse_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_specular_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_glossy", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_shiny_u", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_shiny_v", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_surfderiv_mode", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_image", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_translucent_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_translucency", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_ward_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_color", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_constant_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_bump", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_diffuse_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_specular_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_smoothness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_metalness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_refract_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_transparency", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_trans_glossy", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_transparent_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_index_of_refraction", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_inverttrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_usealphatrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_scaletrans", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_reflect_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_reflectivity", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_reflect_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_reflect_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_invertrefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_usealpharefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_scalerefl", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_radiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_translucent_inuse", L"float")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_translucency", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_incand_inuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_incandescence", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_inc_inten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"material_strauss_environment", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_phong_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_phong_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_phong_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_phong_specular", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_phong_shiny", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_phong_radiance", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_lambert_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_lambert_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_lambert_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_lambert_radiance", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_blinn_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_blinn_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_blinn_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_blinn_specular", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_blinn_roughness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_blinn_radiance", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_cooktorr_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_cooktorr_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_cooktorr_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_cooktorr_specular", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_cooktorr_roughness", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_cooktorr_radiance", L"color"));
 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_diffuse", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_glossy", L"color")); 
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_shiny_u", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_shiny_v", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_surfderiv_mode", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_image", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_ward_radiance", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"simple_translucent_diffuse", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_ambience", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_Ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_DiffuseRoot", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_DiffuseTipA", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_DiffuseTipB", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_FlatShadeRoot", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_FlatShadeTip", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_TipBalance", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_DiffuseCrossoverCenter", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_DiffuseCrossoverRange", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_DiffuseVariation", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_SpecularInuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_Specular", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_SpecularDecay", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_Irradiance", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_TransparencyInuse", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_TransparencyRoot", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_TransparencyTip", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_TransparencyCrossoverCenter", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"rh_renderer_TransparencyCrossoverRange", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_samples", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_bright", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_dark", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_spread", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_max_distance", L"uniform float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_reflective", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"xsiambientocclusion_output_mode", L"float"));
	
	//shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_mode", L"float"));
	//shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_exponent", L"float"));
	//shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_custom_vector", L"vector"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_illum_shadowmaterial_min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_illum_shadowmaterial_max", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_illum_shadowmaterial_rgb", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_illum_shadowmaterial_shadowvisible", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_v2_mode", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_v2_custom_vector", L"vector"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_v2_invert", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_v2_bias", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_incidence_v2_gain", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_state_scalar_mode", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_state_vector_mode", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"dense_vol_density", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"dense_vol_transmit", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"dense_vol_one_to_one", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"dense_vol_one_to_ten", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"dense_vol_one_to_hundred", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"dense_vol_profile", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"soft_fog_start", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"soft_fog_stop", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"soft_fog_rate", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"soft_fog_transmit", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"soft_fog_lightrays", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_reflect_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_reflect_reflect", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_reflect_notrace", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_enabled", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_reflect", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_notrace", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_invert", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_scale", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_max_depth", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_reflection_samples", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_refract_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_refract_refract", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_refract_ior", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_enabled", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_refract", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_ior", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_invert", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_scale", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_max_depth", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_glossy", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_diffuse_refraction_phen_samples", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_opacity_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_opacity_opacity", L"color"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_transparency_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_transparency_transp", L"color"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_dielectric_ior_mode", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_dielectric_ior_refract", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_dielectric_ior_reflect", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_dielectric_ior_ior", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_dielectric_ior_exponent", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_basic_input1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_basic_input2", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_basic_op", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_exponent_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_exponent_factor", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_exponent_op", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_unary_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_unary_op", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_logic_input1", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_logic_input2", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_color_math_logic_op", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_color_average_input", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"mib_color_average_factor", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_interp_linear_input", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_interp_linear_oldrange_min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_interp_linear_oldrange_max", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_interp_linear_newrange_min", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_interp_linear_newrange_max", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_basic_input1", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_basic_input2", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_basic_op", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_exponent_input", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_exponent_factor", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_exponent_op", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_unary_input", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_unary_op", L"float"));

	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_logic_input1", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_logic_input2", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"sib_scalar_math_logic_op", L"float"));
	
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_samples", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_scatter_bias", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_falloff", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_overall_color", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_ambient", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_diffuse_color", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_diffuse_weight", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_specular", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_exponent", L"float"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_front_sss_color", L"color"));
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"misss_fast_simple_phen_front_sss_weight", L"float"));
		
	shaderArgTypes.insert(pair<const wchar_t*, wchar_t*>(L"bg_pic_out_map", L"color"));
}

CStatus xsiRibTranslator::xsiManOptions()
{
	CStatus st = CStatus::OK;

	// get settings from XSIManOptions
	Property prop(m_sceneRoot.GetProperties().GetItem(L"XSIManOptions"));

	Parameter RibName = prop.GetParameters().GetItem(L"RibName");
	job->ribFileName = RibName.GetValue();
	Parameter ImageName = prop.GetParameters().GetItem(L"ImageName");
	job->imageName = ImageName.GetValue();
	Parameter fStop = prop.GetParameters().GetItem(L"fStop");
	job->camera[0].fStop = fStop.GetValue(g_CurrentFrame);
	Parameter FocalDistance = prop.GetParameters().GetItem(L"FocalDistance");
	job->camera[0].focalDistance = FocalDistance.GetValue(g_CurrentFrame);
	Parameter ExportCamera = prop.GetParameters().GetItem(L"ExportCamera");
	job->camera[0].name = ExportCamera.GetValue();
	g_CameraName = job->camera[0].name;
	Parameter UseJobScript = prop.GetParameters().GetItem(L"UseJobScript");
	g_UseJobScript = UseJobScript.GetValue();
	Parameter JobScriptType = prop.GetParameters().GetItem(L"JobScriptType");
	g_JobScriptType = JobScriptType.GetValue();
	Parameter MinProcs = prop.GetParameters().GetItem(L"MinProcs");
	g_MinProcs = (long)MinProcs.GetValue();
	Parameter MaxProcs = prop.GetParameters().GetItem(L"MaxProcs");
	g_MaxProcs = (long)MaxProcs.GetValue();
	Parameter AlfredTags = prop.GetParameters().GetItem(L"AlfredTags");
	g_AlfredTags = AlfredTags.GetValue();
	Parameter AlfredServices = prop.GetParameters().GetItem(L"AlfredServices");
	g_AlfredServices = AlfredServices.GetValue();
	Parameter Imager = prop.GetParameters().GetItem(L"Imager");
	g_Imager = Imager.GetValue();
	Parameter RemoteCmd = prop.GetParameters().GetItem(L"RemoteCmd");
	g_RemoteCmd = RemoteCmd.GetValue();
	Parameter PreJobCommand = prop.GetParameters().GetItem(L"PreJobCommand");
	g_PreJobCommand = PreJobCommand.GetValue();
	Parameter PreFrameCommand = prop.GetParameters().GetItem(L"PreFrameCommand");
	g_PreFrameCommand = PreFrameCommand.GetValue();
	Parameter PostFrameCommand = prop.GetParameters().GetItem(L"PostFrameCommand");
	g_PostFrameCommand = PostFrameCommand.GetValue();
	Parameter PostJobCommand = prop.GetParameters().GetItem(L"PostJobCommand");
	g_PostJobCommand = PostJobCommand.GetValue();
	Parameter CleanJobScript = prop.GetParameters().GetItem(L"CleanJobScript");
	g_CleanJobScript = CleanJobScript.GetValue();
	Parameter PreJobScript = prop.GetParameters().GetItem(L"PreJobScript");
	g_PreJobScript = PreJobScript.GetValue();
	Parameter PreFrameScript = prop.GetParameters().GetItem(L"PreFrameScript");
	g_PreFrameScript = PreFrameScript.GetValue();
	Parameter PostFrameScript = prop.GetParameters().GetItem(L"PostFrameScript");
	g_PostFrameScript = PostFrameScript.GetValue();
	Parameter PostJobScript = prop.GetParameters().GetItem(L"PostJobScript");
	g_PostJobScript = PostJobScript.GetValue();
	Parameter ScriptLanguage = prop.GetParameters().GetItem(L"ScriptLanguage");
	g_ScriptLanguage = ScriptLanguage.GetValue();
	Parameter LaunchRender = prop.GetParameters().GetItem(L"LaunchRender");
	g_LaunchRender = LaunchRender.GetValue();
	//Parameter BypassRib = prop.GetParameters().GetItem(L"BypassRib");
	//g_BypassRib = BypassRib.GetValue();
	Parameter Renderer = prop.GetParameters().GetItem(L"Renderer");
	g_Renderer = Renderer.GetValue();
	Parameter RenderCommand = prop.GetParameters().GetItem(L"RenderCommand");
	g_RenderCommand = RenderCommand.GetValue();
	Parameter CustomCommand = prop.GetParameters().GetItem(L"CustomCommand");
	g_CustomCommand = CustomCommand.GetValue();
	Parameter NumProcs = prop.GetParameters().GetItem(L"NumProcs");
	g_NumProcs = (long)NumProcs.GetValue();
	Parameter ShowProgress = prop.GetParameters().GetItem(L"ShowProgress");
	g_ShowProgress = ShowProgress.GetValue();
	Parameter RenderStatistics = prop.GetParameters().GetItem(L"RenderStatistics");
	g_RenderStatistics = (long)RenderStatistics.GetValue();
	Parameter StatisticsFileName = prop.GetParameters().GetItem(L"StatisticsFileName");
	g_StatisticsFileName = StatisticsFileName.GetValue();
	Parameter StatisticsXMLFileName = prop.GetParameters().GetItem(L"StatisticsXMLFileName");
	g_StatisticsXMLFileName = StatisticsXMLFileName.GetValue();
	Parameter JobSummary = prop.GetParameters().GetItem(L"JobSummary");
	g_JobSummary = JobSummary.GetValue();
	Parameter RenderSelectedObjects = prop.GetParameters().GetItem(L"RenderSelectedObjects");
	g_RenderSelectedObjects = RenderSelectedObjects.GetValue();
	Parameter RenderSelectedLights = prop.GetParameters().GetItem(L"RenderSelectedLights");
	g_RenderSelectedLights = RenderSelectedLights.GetValue();
	Parameter ApproximatedMesh = prop.GetParameters().GetItem(L"ApproximatedMesh");
	g_ApproximatedMesh = ApproximatedMesh.GetValue();
	Parameter PolygonClusters = prop.GetParameters().GetItem(L"PolygonClusters");
	g_PolygonClusters = PolygonClusters.GetValue();
	Parameter InstanceGeometry = prop.GetParameters().GetItem(L"InstanceGeometry");
	g_InstanceGeometry = InstanceGeometry.GetValue();
	Parameter ReuseInstanceGeometry = prop.GetParameters().GetItem(L"ReuseInstanceGeometry");
	g_ReuseInstanceGeometry = ReuseInstanceGeometry.GetValue();	
	Parameter CropWindowX1 = prop.GetParameters().GetItem(L"CropWindowX1");
	cropWindowX1 = CropWindowX1.GetValue();
	Parameter CropWindowX2 = prop.GetParameters().GetItem(L"CropWindowX2");
	cropWindowX2 = CropWindowX2.GetValue();
	Parameter CropWindowY1 = prop.GetParameters().GetItem(L"CropWindowY1");
	cropWindowY1 = CropWindowY1.GetValue();
	Parameter CropWindowY2 = prop.GetParameters().GetItem(L"CropWindowY2");
	cropWindowY2 = CropWindowY2.GetValue();
	Parameter RenderTreeSupport = prop.GetParameters().GetItem(L"RenderTreeSupport");
	g_RenderTreeSupport = RenderTreeSupport.GetValue();
	Parameter ResolutionX = prop.GetParameters().GetItem(L"ResolutionX");
	job->width = (long)ResolutionX.GetValue();
	Parameter ResolutionY = prop.GetParameters().GetItem(L"ResolutionY");
	job->height = (long)ResolutionY.GetValue();
	Parameter PixelAspectRatio = prop.GetParameters().GetItem(L"PixelAspectRatio");
	job->aspectRatio = (float)PixelAspectRatio.GetValue();
	Parameter Shadingrate = prop.GetParameters().GetItem(L"ShadingRate");
	shadingRate = (float)Shadingrate.GetValue();
	Parameter PixelSamplesX = prop.GetParameters().GetItem(L"PixelSamplesX");
	pixelSamples[0] = (long)PixelSamplesX.GetValue();
	Parameter PixelSamplesY = prop.GetParameters().GetItem(L"PixelSamplesY");
	pixelSamples[1] = (long)PixelSamplesY.GetValue();
	Parameter PixelFilter = prop.GetParameters().GetItem(L"PixelFilter");
	pixelFilter = (PixelFilterType)((long)PixelFilter.GetValue());
	Parameter FilterSizeX = prop.GetParameters().GetItem(L"FilterSizeX");
	filterSize[0] = (float)FilterSizeX.GetValue();
	Parameter FilterSizeY = prop.GetParameters().GetItem(L"FilterSizeY");
	filterSize[1] = (float)FilterSizeY.GetValue();
	Parameter Motionblur = prop.GetParameters().GetItem(L"MotionBlur");
	g_MotionBlur = Motionblur.GetValue();
	Parameter Deformblur = prop.GetParameters().GetItem(L"DeformBlur");
	g_DeformBlur = Deformblur.GetValue();
	Parameter Camerablur = prop.GetParameters().GetItem(L"CameraBlur");
	g_CameraBlur = Camerablur.GetValue();
	Parameter ShutterAngle = prop.GetParameters().GetItem(L"ShutterAngle");
	job->camera[0].shutter = ShutterAngle.GetValue(g_CurrentFrame);
	job->camera[0].shutter /= 360.f;
	Parameter ShutterTiming = prop.GetParameters().GetItem(L"ShutterTiming");
	g_ShutterTiming = ShutterTiming.GetValue();
	Parameter FrontplaneGeom = prop.GetParameters().GetItem(L"FrontplaneGeom");
	g_FrontplaneGeom = FrontplaneGeom.GetValue();
	Parameter BackplaneGeom = prop.GetParameters().GetItem(L"BackplaneGeom");
	g_BackplaneGeom = BackplaneGeom.GetValue();
	Parameter BucketSizeX = prop.GetParameters().GetItem(L"BucketSizeX");
	bucketSize[0] = (long)BucketSizeX.GetValue();
	Parameter BucketSizeY = prop.GetParameters().GetItem(L"BucketSizeY");
	bucketSize[1] = (long)BucketSizeY.GetValue();
	Parameter GridSize = prop.GetParameters().GetItem(L"GridSize");
	gridSize = (long)GridSize.GetValue();
	Parameter TextureMemory = prop.GetParameters().GetItem(L"TextureMemory");
	textureMemory = (long)TextureMemory.GetValue();
	Parameter Eyesplits = prop.GetParameters().GetItem(L"Eyesplits");
	eyeSplits = (long)Eyesplits.GetValue();
	Parameter ShadowBias = prop.GetParameters().GetItem(L"ShadowBias");
	shadowBias = ShadowBias.GetValue();
	g_ShadowMapBias = shadowBias;
	Parameter DicingStrategy = prop.GetParameters().GetItem(L"DicingStrategy");
	g_DicingStrategy = DicingStrategy.GetValue();
	Parameter DicingCamera = prop.GetParameters().GetItem(L"DicingCamera");
	g_DicingCamera = DicingCamera.GetValue();
	Parameter RasterOriented = prop.GetParameters().GetItem(L"RasterOriented");
	g_RasterOriented = RasterOriented.GetValue();
	Parameter MotionFactor = prop.GetParameters().GetItem(L"MotionFactor");
	g_MotionFactor = MotionFactor.GetValue();
	Parameter Quantization = prop.GetParameters().GetItem(L"Quantization");
	quantValue = (long)Quantization.GetValue();
	Parameter QuantizeOne = prop.GetParameters().GetItem(L"QuantizeOne");
	quantizeOne = (long)QuantizeOne.GetValue(); 
	Parameter QuantizeMin = prop.GetParameters().GetItem(L"QuantizeMin");
	quantizeMin = (long)QuantizeMin.GetValue();
	Parameter QuantizeMax = prop.GetParameters().GetItem(L"QuantizeMax");
	quantizeMax = (long)QuantizeMax.GetValue();
	Parameter QuantizeDither = prop.GetParameters().GetItem(L"QuantizeDither");
	dither = (float)QuantizeDither.GetValue();
	Parameter ExposureGain = prop.GetParameters().GetItem(L"ExposureGain");
	gainValue = (float)ExposureGain.GetValue();
	Parameter ExposureGamma = prop.GetParameters().GetItem(L"ExposureGamma");
	gammaValue = (float)ExposureGamma.GetValue();
	Parameter DisplayMode = prop.GetParameters().GetItem(L"DisplayMode");
	job->imageMode = DisplayMode.GetValue().GetAsText();
	Parameter DisplayDriver = prop.GetParameters().GetItem(L"DisplayDriver");
	job->format = DisplayDriver.GetValue().GetAsText();
	Parameter EnableRayTracing = prop.GetParameters().GetItem(L"EnableRayTracing");
	g_EnableRayTracing = EnableRayTracing.GetValue();
	Parameter MaxRayDepth = prop.GetParameters().GetItem(L"MaxRayDepth");
	g_MaxRayDepth = (long)MaxRayDepth.GetValue();
	Parameter RayTraceBias = prop.GetParameters().GetItem(L"RayTraceBias");
	g_RayTraceBias = (float)RayTraceBias.GetValue();
	Parameter RayTraceBreadthFactor = prop.GetParameters().GetItem(L"RayTraceBreadthFactor");
	g_RayTraceBreadthFactor = (float)RayTraceBreadthFactor.GetValue();
	Parameter RayTraceDepthFactor = prop.GetParameters().GetItem(L"RayTraceDepthFactor");
	g_RayTraceDepthFactor = (float)RayTraceDepthFactor.GetValue();
	Parameter RayTraceMaxSpecularDepth = prop.GetParameters().GetItem(L"RayTraceMaxSpecularDepth");
	g_RayTraceMaxSpecularDepth = (long)RayTraceMaxSpecularDepth.GetValue();
	Parameter RayTraceMaxDiffuseDepth = prop.GetParameters().GetItem(L"RayTraceMaxDiffuseDepth");
	g_RayTraceMaxDiffuseDepth = (long)RayTraceMaxDiffuseDepth.GetValue();
	Parameter RayTraceDisplacements = prop.GetParameters().GetItem(L"RayTraceDisplacements");
	g_RayTraceDisplacements = RayTraceDisplacements.GetValue();
	Parameter RayTraceSampleMotion = prop.GetParameters().GetItem(L"RayTraceSampleMotion");
	g_RayTraceSampleMotion = RayTraceSampleMotion.GetValue();
	Parameter EnableAmbientOcclusion = prop.GetParameters().GetItem(L"EnableAmbientOcclusion");
	g_EnableAmbientOcclusion = EnableAmbientOcclusion.GetValue();
	Parameter OcclusionSamples = prop.GetParameters().GetItem(L"OcclusionSamples");
	g_OcclusionSamples = OcclusionSamples.GetValue();
	Parameter OcclusionMaxdist = prop.GetParameters().GetItem(L"OcclusionMaxdist");
	g_OcclusionMaxdist = (float)OcclusionMaxdist.GetValue();
	Parameter EnableIndirectIllumination = prop.GetParameters().GetItem(L"EnableIndirectIllumination");
	g_EnableIndirectIllumination = EnableIndirectIllumination.GetValue();
	Parameter IndirectSamples = prop.GetParameters().GetItem(L"IndirectSamples");
	g_IndirectSamples = IndirectSamples.GetValue();
	Parameter RayTraceMaxError = prop.GetParameters().GetItem(L"RayTraceMaxError");
	g_RayTraceMaxError = (float)RayTraceMaxError.GetValue();
	Parameter RayTraceMaxPixDist = prop.GetParameters().GetItem(L"RayTraceMaxPixDist");
	g_RayTraceMaxPixDist = (float)RayTraceMaxPixDist.GetValue();
	Parameter IrradianceCache = prop.GetParameters().GetItem(L"IrradianceCache");
	g_IrradianceCache = IrradianceCache.GetValue();
	Parameter IrradianceCacheMode = prop.GetParameters().GetItem(L"IrradianceCacheMode");
	g_IrradianceCacheMode = IrradianceCacheMode.GetValue();
	Parameter PhotonShadingModel = prop.GetParameters().GetItem(L"PhotonShadingModel");
	g_PhotonShadingModel = PhotonShadingModel.GetValue();
	Parameter PhotonCausticMap = prop.GetParameters().GetItem(L"PhotonCausticMap");
	g_PhotonCausticMap = PhotonCausticMap.GetValue();
	Parameter PhotonGlobalMap = prop.GetParameters().GetItem(L"PhotonGlobalMap");
	g_PhotonGlobalMap = PhotonGlobalMap.GetValue();
	Parameter PhotonEstimator = prop.GetParameters().GetItem(L"PhotonEstimator");
	g_PhotonEstimator = PhotonEstimator.GetValue();
	Parameter RibOutput = prop.GetParameters().GetItem(L"RibOutput");
	if (RibOutput.GetValue() == L"ascii") g_BinaryRib = false;
	else if (RibOutput.GetValue() == L"binary") g_BinaryRib = true;
	else g_BinaryRib = false;
	Parameter GzipRib = prop.GetParameters().GetItem(L"GzipRib");
	g_GzipRib = GzipRib.GetValue();;
	Parameter FramePadding = prop.GetParameters().GetItem(L"FramePadding");
	g_FramePadding = FramePadding.GetValue();
	Parameter RenderFrames = prop.GetParameters().GetItem(L"RenderFrames");
	g_RenderFrames = RenderFrames.GetValue();
	Parameter StartFrame = prop.GetParameters().GetItem(L"StartFrame");
	g_StartFrame = StartFrame.GetValue();
	Parameter EndFrame = prop.GetParameters().GetItem(L"EndFrame");
	g_EndFrame = EndFrame.GetValue();
	Parameter StepSize = prop.GetParameters().GetItem(L"StepSize");
	g_StepSize = StepSize.GetValue();
	Parameter SkipRenderedFrames = prop.GetParameters().GetItem(L"SkipRenderedFrames");
	g_SkipRenderedFrames = SkipRenderedFrames.GetValue();
	Parameter RenderMode = prop.GetParameters().GetItem(L"RenderMode");
	g_RenderMode = RenderMode.GetValue();
	Parameter NamedHandles = prop.GetParameters().GetItem(L"NamedHandles");
	g_NamedHandles = NamedHandles.GetValue();
	Parameter HandlePrefix = prop.GetParameters().GetItem(L"HandlePrefix");
	g_HandlePrefix = HandlePrefix.GetValue();
	Parameter RibPath = prop.GetParameters().GetItem(L"RibPath");
	g_RibPath = RibPath.GetValue();
	Parameter PixPath = prop.GetParameters().GetItem(L"PixPath");
	g_PixPath = PixPath.GetValue(); 
	Parameter TexPath = prop.GetParameters().GetItem(L"TexPath");
	g_TexPath = TexPath.GetValue();
	Parameter ShdPath = prop.GetParameters().GetItem(L"ShdPath");
	g_ShdPath = ShdPath.GetValue();
	Parameter JobPath = prop.GetParameters().GetItem(L"JobPath");
	g_JobPath = JobPath.GetValue();
	Parameter ShaderPath = prop.GetParameters().GetItem(L"ShaderPath");
	g_ShaderPath = ShaderPath.GetValue();
	Parameter SloPath = prop.GetParameters().GetItem(L"SloPath");
	g_SloPath = L".:@:" + CString(SloPath.GetValue()); 

	// add XSIMan directory to slo path
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	if (CValue(xsimanhome).GetAsText() != L"") g_SloPath += L":" + CValue(xsimanhome).GetAsText() + L"/shaders";
	else m_app.LogMessage(L"Warning: the XSIMAN environment variable is not defined!");

	Parameter DsoPath = prop.GetParameters().GetItem(L"DsoPath");
	g_DsoPath = L".:@:" + CString(DsoPath.GetValue());
	Parameter ImgPath = prop.GetParameters().GetItem(L"ImgPath");
	g_ImgPath = L".:@:" + CString(ImgPath.GetValue());
	Parameter Cleanup = prop.GetParameters().GetItem(L"Cleanup");
	g_Cleanup = (long)Cleanup.GetValue();
	Parameter OpacityThresholdR = prop.GetParameters().GetItem(L"OpacityThresholdR");
	g_OpacityThresholdR = OpacityThresholdR.GetValue();
	Parameter OpacityThresholdG = prop.GetParameters().GetItem(L"OpacityThresholdG");
	g_OpacityThresholdG = OpacityThresholdG.GetValue();
	Parameter OpacityThresholdB = prop.GetParameters().GetItem(L"OpacityThresholdB");
	g_OpacityThresholdB = OpacityThresholdB.GetValue();
	Parameter OpacityCullingR = prop.GetParameters().GetItem(L"OpacityCullingR");
	g_OpacityCullingR = OpacityCullingR.GetValue();
	Parameter OpacityCullingG = prop.GetParameters().GetItem(L"OpacityCullingG");
	g_OpacityCullingG = OpacityCullingG.GetValue();
	Parameter OpacityCullingB = prop.GetParameters().GetItem(L"OpacityCullingB");
	g_OpacityCullingB = OpacityCullingB.GetValue();
	Parameter BinaryDicing = prop.GetParameters().GetItem(L"BinaryDicing");
	g_BinaryDicing = BinaryDicing.GetValue();
	Parameter aovList = prop.GetParameter(L"AOVList");
	g_AOVList = aovList.GetValue();
	Parameter PrimaryDisplay = prop.GetParameters().GetItem(L"PrimaryDisplay");
	g_PrimaryDisplay = PrimaryDisplay.GetValue();
	
	return st; 
}

//*****************************************************************************
//	Exports the scene root and its children
//*****************************************************************************

CStatus xsiRibTranslator::ExportFile(commandLineOptions* options, bool exportAsArchive)
{
	char* ribFileName = NULL; CString fileName;

	// do we export as RIB archive
	g_ExportAsArchive = exportAsArchive;

	// get the play control property
	Property playControl = m_app.GetActiveProject().GetProperties().GetItem(L"Play Control");
	g_CurrentFrame = playControl.GetParameterValue(L"current");
	unsigned long startFrame = playControl.GetParameterValue(L"start");

	// get XSIManOptions
	CStatus st = xsiManOptions();

	// set project directories
	CString projPath = m_app.GetActiveProject().GetFullName();
	makedir(projPath);

	// export the scene information.	
	// begin rib file output

	// do we render the whole frame range?
	if (!g_RenderFrames) {
		g_StartFrame = (long)g_CurrentFrame;
		g_EndFrame = (long)g_CurrentFrame;
		g_StepSize = 1;
	}

	// get command line arguments
	if (options != NULL) {
		if (options->RibName != L"") job->ribFileName = options->RibName;
		if (options->ImageName != L"") job->imageName = options->ImageName;
		if (options->ExportCamera != L"") job->camera[0].name = options->ExportCamera;
		if (options->DisplayDriver != L"") job->format = options->DisplayDriver;
		if (options->StartFrame != -1000000) g_StartFrame = options->StartFrame;
		if (options->EndFrame != -1000000) g_EndFrame = options->EndFrame;
		if (options->StepSize != -1000000) g_StepSize = options->StepSize;
		if (options->ShadingRate != -1000000) shadingRate = options->ShadingRate;
		if (options->PixelSamplesX != -1000000) pixelSamples[0] = options->PixelSamplesX;
		if (options->PixelSamplesY != -1000000) pixelSamples[1] = options->PixelSamplesY;
		if (options->ResolutionX != -1000000) job->width = options->ResolutionX;
		if (options->ResolutionY != -1000000) job->height = options->ResolutionY;
		if (options->BatchMode) g_BatchMode = true;
		if (options->RibPath != L"") g_RibPath = options->RibPath + L"\\";
		if (options->JobPath != L"") g_JobPath = options->JobPath + L"\\";
		if (options->PixPath != L"") g_PixPath = options->PixPath + L"\\";
		if (options->TexPath != L"") g_TexPath = options->TexPath + L"\\";
		if (options->ShdPath != L"") g_ShdPath = options->ShdPath + L"\\";
		if (options->ShaderPath != L"") g_ShaderPath = options->ShaderPath + L"\\";
	}

	// scan scene
	SceneItem sceneitem(m_sceneRoot);
	scanScene(sceneitem, L"");

	// check if we are instancing geometry
	if (g_InstanceGeometry == true) 
		appendInstanceJob();

	if (g_BinaryRib) {
		RtString format = "binary";
		RiOption((RtToken) "rib", (RtToken) "format", (RtPointer)&format, RI_NULL);
	} 
	else {
		RtString format = "ascii";
		RiOption((RtToken) "rib", (RtToken) "format", (RtPointer)&format, RI_NULL);
	}

	if (g_GzipRib) {
		RtString gzip = "gzip";
		RiOption((RtToken) "rib", (RtToken) "compression", &gzip, RI_NULL);
	} 
	else {
		RtString gzip = "none";
		RiOption((RtToken) "rib", (RtToken) "compression", &gzip, RI_NULL);
	}

	// set original and first frame
	CValueArray args; CValue index;
	playControl.PutParameterValue(L"current", (long)g_StartFrame);
	m_app.ExecuteCommand(L"Refresh", args, index);
	long currentFrame = (long)g_CurrentFrame;

	// progress bar
	UIToolkit kit = m_app.GetUIToolkit();
    ProgressBar bar = kit.GetProgressBar();
	bar.PutMaximum((long)(g_EndFrame - g_StartFrame + 1));
    bar.PutStep((long)g_StepSize);
    bar.PutVisible(true);
    bar.PutCaption(L"Processing");

	// execute prejob script
	if (g_PreJobScript != L"") {
		m_app.ExecuteScriptCode(g_PreJobScript, g_ScriptLanguage);
	}

	for (long fnum = g_StartFrame; fnum <= g_EndFrame; fnum+=g_StepSize) {
		// set current frame
		g_CurrentFrame = (float)fnum;

		// execute preframe script
		if (g_PreFrameScript != L"") {
			m_app.ExecuteScriptCode(g_PreFrameScript, g_ScriptLanguage);
		}

		// check progress
		if (bar.IsCancelPressed()) break;

		// frame padding
		wchar_t padframe[10];
		swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);

		// update node visibility params
		vector<xsiRibNode>::iterator jter;
		for(jter=nodeData.nodeMap.begin(); jter!=nodeData.nodeMap.end(); ++jter) {
			CRefArray propRefArray = (*jter).node.sceneItem.GetProperties();
			for (long i = 0; i < propRefArray.GetCount(); i++ ) {
				if (SIObject(propRefArray[i]).GetName() == L"Visibility") {
					Property prop = propRefArray[i];
					Parameter rendvis = prop.GetParameters().GetItem(L"rendvis");
					(*jter).node.renderVisibility = rendvis.GetValue();
					Parameter primray = prop.GetParameters().GetItem(L"primray");
					(*jter).node.primaryVisibility = primray.GetValue();
					Parameter scndray = prop.GetParameters().GetItem(L"scndray");
					(*jter).node.secondaryVisibility = scndray.GetValue();	
					Parameter shdwcast = prop.GetParameters().GetItem(L"shdwcast");
					(*jter).node.shadowVisibility = shdwcast.GetValue();
				}
			}
		}

		// update camera info
		multimap<JobType, jobStruct*>::iterator iter;
		for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {
			// get shutter timing
			float sampleTime = g_CurrentFrame;
			if (g_ShutterTiming == L"OpenOnFrame") sampleTime = g_CurrentFrame;
			else if (g_ShutterTiming == L"CloseOnFrame") sampleTime = g_CurrentFrame - job->camera[0].shutter;
			else if (g_ShutterTiming == L"CenterOnFrame") sampleTime = g_CurrentFrame - 0.5f * job->camera[0].shutter;
			float sampleIncrement = job->camera[0].shutter / (g_MotionSamples - 1);

			for (signed i = 0; i < g_MotionSamples; i++) {
				xsiRibCameraData cam((*iter).second->camera[0].x3dobject, sampleTime);
				sampleTime += sampleIncrement;
				cameraStruct camData = cam.get();
				(*iter).second->camera[i].matrix4 = camData.matrix4;
			}
		}

		// shadow passes
		for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {

			if (((*iter).first == ShadowJob) && (g_ExportAsArchive == false)) {
				jobStruct* shadowJob = (*iter).second;
				
				if ((shadowJob->jobType == ShadowJob) && shadowJob->rewriteData) {
					// get job information
					job->jobType = ShadowJob;
					job->camera[0].name = shadowJob->camera[0].name;
					job->name = shadowJob->name;
					job->width = shadowJob->width;
					job->height = shadowJob->height;
					job->aspectRatio = shadowJob->aspectRatio;
					job->format = shadowJob->format;
					job->imageMode = shadowJob->imageMode;
					job->imageName = shadowJob->imageName;
					job->ribFileName = shadowJob->ribFileName;
					job->camera[0].orthogonal = shadowJob->camera[0].orthogonal;
					job->camera[0].nearClip = shadowJob->camera[0].nearClip;
					job->camera[0].farClip = shadowJob->camera[0].farClip;
					job->camera[0].orthoWidth = shadowJob->camera[0].orthoWidth;
					job->camera[0].orthoHeight = shadowJob->camera[0].orthoHeight;
					job->camera[0].horizFOV = shadowJob->camera[0].horizFOV;
					job->camera[0].matrix4 = shadowJob->camera[0].matrix4;
					job->fileExt = shadowJob->fileExt;
					job->reuseData = shadowJob->reuseData;
					job->shadowMapType = shadowJob->shadowMapType;
					job->shadowVolInterp = shadowJob->shadowVolInterp;
					job->shadowPixelSamples = shadowJob->shadowPixelSamples;
					job->isPointLight = shadowJob->isPointLight;
					job->writeFrequency = shadowJob->writeFrequency;
					job->rewriteData = shadowJob->rewriteData;
					job->shadowBlur = shadowJob->shadowBlur;
					job->rayTraceShadows = shadowJob->rayTraceShadows;
				    job->isEnvmap = shadowJob->isEnvmap; 

					// are we ray-tracing shadows?
					if (job->rayTraceShadows)
						continue;

					// point lights need 6 shadow maps!
					if (job->isPointLight) {
						for (int faceView = 0; faceView < 6; faceView++) {
							job->cubeMapDir = faceView;
							fileName = constructFileName(true, job);
							XSIW2A(&ribFileName, fileName.GetWideString());
							CString imageName = constructFileName(false, job);
							char* imageFileName;
							XSIW2A(&imageFileName, imageName.GetWideString());

							// reuse existing shadow maps?
							if ((!job->reuseData || (job->reuseData && access(imageFileName, 0)))) {
								RiBegin(ribFileName);	
								if (ribPrologue() == CStatus::OK) {
									framePrologue();
									frameBody();
									frameEpilogue();
									ribEpilogue();
								}
								RiEnd();

								// add to render list
								taskStruct* task = new taskStruct;
								task->fileName = fileName;
								task->imageName = constructFileName(false, job);
								task->frame = (long)g_CurrentFrame;
								task->name = job->name;
								if (job->writeFrequency == L"Once Per Job" && g_CurrentFrame == g_StartFrame) {
									task->type = GlobalTask;
									renderList.push_back(task);
								}
								else if (job->writeFrequency == L"Every Frame") {
									task->type = SubTask;
									renderList.push_back(task);
								}

								//add to cleanup list
								if (g_Cleanup == 1 || g_Cleanup == 5 || g_Cleanup == 17 || g_Cleanup == 21) 
									cleanupList.push_back(fileName);
							}
						}
					}
					else {
						fileName = constructFileName(true, job);
						XSIW2A(&ribFileName, fileName.GetWideString());
						CString imageName = constructFileName(false, job);
						char* imageFileName;
						XSIW2A(&imageFileName, imageName.GetWideString());

						// reuse existing shadow maps?
						if ((!job->reuseData || (job->reuseData && access(imageFileName, 0)))) {
							RiBegin(ribFileName);
							if (ribPrologue() == CStatus::OK) {
								framePrologue();
								frameBody();
								frameEpilogue();
								ribEpilogue();
							}
							RiEnd();

							// add to render list
							taskStruct* task = new taskStruct;
							task->fileName = fileName;
							task->imageName = constructFileName(false, job);
							task->frame = (long)g_CurrentFrame;
							task->name = job->name;
							if (job->writeFrequency == L"Once Per Job" && g_CurrentFrame == g_StartFrame) {
								task->type = GlobalTask;
								renderList.push_back(task);
							}
							else if (job->writeFrequency == L"Every Frame") {
								task->type = SubTask;
								renderList.push_back(task);
							}

							//add to cleanup list
							if (g_Cleanup == 1 || g_Cleanup == 5 || g_Cleanup == 17 || g_Cleanup == 21)
								cleanupList.push_back(fileName);
						}
					}
				}

				// check write frequency
				if (shadowJob->writeFrequency == L"Once Per Job" && shadowJob->rewriteData) {
					shadowJob->rewriteData = false;
				}
			}
		}

		// photon map pass;
		for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {

			if (((*iter).first == PhotonMapJob) && (g_ExportAsArchive == false)) {
				jobStruct* photonJob = (*iter).second;

				if ((photonJob->jobType == PhotonMapJob) && photonJob->rewriteData) {
					// get job information
					job->jobType = PhotonMapJob;
					job->camera[0].name = photonJob->camera[0].name;
					job->name = photonJob->name;
					job->photons = photonJob->photons;
					job->photonMapType = photonJob->photonMapType;
					job->imageName = photonJob->imageName;
					job->ribFileName = photonJob->ribFileName;
					job->fileExt = photonJob->fileExt;
					job->reuseData = photonJob->reuseData;
					job->writeFrequency = photonJob->writeFrequency;
					job->rewriteData = photonJob->rewriteData;

					fileName = constructFileName(true, job);
					XSIW2A(&ribFileName, fileName.GetWideString());
					CString imageName = constructFileName(false, job);
					char* imageFileName;
					XSIW2A(&imageFileName, imageName.GetWideString());

					// reuse existing photon maps?
					if ((!job->reuseData || (job->reuseData && access(imageFileName, 0)))) {
						RiBegin(ribFileName);
						if (ribPrologue() == CStatus::OK) {
							framePrologue();
							frameBody();
							frameEpilogue();
							ribEpilogue();
						}
						RiEnd();

						// add to render list
						taskStruct* task = new taskStruct;
						task->fileName = fileName;
						task->imageName = constructFileName(false, job);
						task->frame = (long)g_CurrentFrame;
						task->name = job->name;
						if (job->writeFrequency == L"Once Per Job" && g_CurrentFrame == g_StartFrame) {
							task->type = GlobalTask;
							renderList.push_back(task);
						}
						else if (job->writeFrequency == L"Every Frame") {
							task->type = SubTask;
							renderList.push_back(task);
						}
					
						//add to cleanup list
						if (g_Cleanup == 1 || g_Cleanup == 5 || g_Cleanup == 17 || g_Cleanup == 21)
							cleanupList.push_back(fileName);
					}
				}
				
				// check write frequency
				if (photonJob->writeFrequency == L"Once Per Job" && photonJob->rewriteData) {
					photonJob->rewriteData = false;
				}
			}
		}

		// environment map passes
		for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {

			if (((*iter).first == EnvmapJob) && (g_ExportAsArchive == false)) {
				jobStruct* envmapJob = (*iter).second;
				
				if ((envmapJob->jobType == EnvmapJob) && envmapJob->rewriteData) {	
					// get job information
					job->jobType = EnvmapJob;
					job->camera[0].name = envmapJob->camera[0].name;
					job->name = envmapJob->name;
					job->width = envmapJob->width;
					job->height = envmapJob->height;
					job->aspectRatio = envmapJob->aspectRatio;
					job->format = envmapJob->format;
					job->imageMode = envmapJob->imageMode;
					job->imageName = envmapJob->imageName;
					job->ribFileName = envmapJob->ribFileName;
					job->camera[0].nearClip = envmapJob->camera[0].nearClip;
					job->camera[0].farClip = envmapJob->camera[0].farClip;
					job->camera[0].horizFOV = envmapJob->camera[0].horizFOV;
					job->camera[0].matrix4 = envmapJob->camera[0].matrix4;
					job->fileExt = envmapJob->fileExt;
					job->reuseData = envmapJob->reuseData;
					job->isEnvmap = envmapJob->isEnvmap;
					job->writeFrequency = envmapJob->writeFrequency;
					job->rewriteData = envmapJob->rewriteData;

					// create cubic environment map
					if (job->isEnvmap) {
						for (int faceView = 0; faceView < 6; faceView++) {
							job->cubeMapDir = faceView;
							fileName = constructFileName(true, job);
							XSIW2A(&ribFileName, fileName.GetWideString());
							CString imageName = constructFileName(false, job);
							char* imageFileName;
							XSIW2A(&imageFileName, imageName.GetWideString());

							// reuse existing environment map?
							if ((!job->reuseData || (job->reuseData && access(imageFileName, 0)))) {
								RiBegin(ribFileName);	
								if (ribPrologue() == CStatus::OK) {
									framePrologue();
									frameBody();
									frameEpilogue();
									ribEpilogue();
								}
								RiEnd();

								// add to render list
								taskStruct* task = new taskStruct;
								task->fileName = fileName;
								task->imageName = constructFileName(false, job);
								task->frame = (long)g_CurrentFrame;
								task->name = job->name;
								if (job->writeFrequency == L"Once Per Job" && g_CurrentFrame == g_StartFrame) {
									task->type = GlobalTask;
									renderList.push_back(task);
								}
								else if (job->writeFrequency == L"Every Frame") {
									task->type = SubTask;
									renderList.push_back(task);
								}

								//add to cleanup list
								if (g_Cleanup == 1 || g_Cleanup == 5 || g_Cleanup == 17 || g_Cleanup == 21) 
									cleanupList.push_back(fileName);
							}
						}
					}
				}

				// check write frequency
				if (envmapJob->writeFrequency == L"Once Per Job" && envmapJob->rewriteData) {
					envmapJob->rewriteData = false;
				}
			}
		}

		// beauty pass;
		for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {

			if ((*iter).first == BeautyJob) {
				jobStruct* beautyJob = (*iter).second;

				if (beautyJob->camera[0].name == g_CameraName) {
					// get job information
					job->jobType = BeautyJob;
					job->camera[0].name = beautyJob->camera[0].name;
					job->name = beautyJob->name;
					job->width = beautyJob->width;
					job->height = beautyJob->height;
					job->aspectRatio = beautyJob->aspectRatio;
					job->format = beautyJob->format;
					job->imageMode = beautyJob->imageMode;
					job->imageName = beautyJob->imageName;
					job->ribFileName = beautyJob->ribFileName;
					job->fileExt = beautyJob->fileExt;
					job->writeFrequency = beautyJob->writeFrequency;
		
					// check if we are skipping this frame
					if (g_SkipRenderedFrames) {			
						CString imageName = constructFileName(false, job);
						char* imageFileName;
						XSIW2A(&imageFileName, imageName.GetWideString());
						if (!access(imageFileName, 0)) {
							m_app.LogMessage(L"Skipping existing rendered image for frame number " + CValue(g_CurrentFrame).GetAsText());
							continue;
						}
					}

					// get camera information
					for (signed i = 0; i < g_MotionSamples; i++) {
						job->camera[i].orthogonal = beautyJob->camera[i].orthogonal;
						job->camera[i].nearClip = beautyJob->camera[i].nearClip;
						job->camera[i].farClip = beautyJob->camera[i].farClip;
						job->camera[i].orthoWidth = beautyJob->camera[i].orthoWidth;
						job->camera[i].orthoHeight = beautyJob->camera[i].orthoHeight;
						job->camera[i].horizFOV = beautyJob->camera[i].horizFOV;
						job->camera[i].focalLength = beautyJob->camera[i].focalLength;
						job->camera[i].matrix4 = beautyJob->camera[i].matrix4;
					}

					fileName = constructFileName(true, job);
					XSIW2A(&ribFileName, fileName.GetWideString());
	
					// if bypassing RIB generation open pipe directly to command
					//FILE *renderpipe;
					//if (g_BypassRib == false)
						RiBegin(ribFileName); 
					//else {
#ifdef WIN32
					//	renderpipe = _wpopen(g_RenderCommand.GetWideString(), L"w");
#endif
#ifdef LINUX
					//	renderpipe = wpopen(g_RenderCommand.GetWideString(), L"w");
#endif
					//	if (renderpipe) {
					//		RtInt fd = fileno(renderpipe);
					//		RiOption("rib", "pipe", (RtPointer)&fd, RI_NULL);
					//		RiBegin(RI_NULL);
					//	}
					//}

					if (ribPrologue() == CStatus::OK) {
						framePrologue();
						frameBody();
						frameEpilogue();
						ribEpilogue();
					}
					else {
						exportGeometry();		// g_ExportAsArchive == true
					}

					RiEnd();
		
					//if (g_BypassRib == true)
					//	_pclose(renderpipe);

					// add to render list
					if (g_ExportAsArchive == false) {
						taskStruct* task = new taskStruct;
						task->fileName = fileName;
						task->imageName = constructFileName(false, job);
						task->frame = (long)g_CurrentFrame;
						task->name = job->name;
						task->type = SuperTask;
						renderList.push_back(task);
					}
					
					//add to cleanup list
					if (g_Cleanup == 1 || g_Cleanup == 5 || g_Cleanup == 17 || g_Cleanup == 21)
						cleanupList.push_back(fileName);
				}
			}
		}

		// archive passes
		for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {

			if ((*iter).first == ArchiveJob) {
				jobStruct* archiveJob = (*iter).second;

				if (archiveJob->rewriteData) {
					// get job information
					job->name = archiveJob->name;
					job->ribFileName = archiveJob->ribFileName;
					job->writeFrequency = archiveJob->writeFrequency;
					job->rewriteData = archiveJob->rewriteData;
					job->jobType = ArchiveJob;
					job->reuseData = archiveJob->reuseData;

					// check write frequency
					char* archiveFileName; CString fileName;
					fileName = constructFileName(true, job);
					XSIW2A(&archiveFileName, fileName.GetWideString());

					// reuse existing archive? check if file already exists
					if (!job->reuseData || (job->reuseData && access(archiveFileName, 0))) {
						RiBegin(archiveFileName);
						frameBody();
						RiEnd();
					}
				}

				// check write frequency
				if (archiveJob->writeFrequency == L"Once Per Job" && archiveJob->rewriteData) {
					archiveJob->rewriteData = false;
				}
			}
		}

		// create instance archive
		if (g_InstanceGeometry == true) {
			for (iter=jobList.begin(); iter!=jobList.end(); ++iter) {
				if ((*iter).first == InstanceJob) {	
					jobStruct* instanceJob = (*iter).second;

					if (instanceJob->jobType == InstanceJob) {
						// get job information
						job->jobType = InstanceJob;	
						job->reuseData = instanceJob->reuseData;

						char* instanceFileName; CString fileName;
						fileName = constructFileName(true, job);
						XSIW2A(&instanceFileName, fileName.GetWideString());

						// reuse existing archive? check if file already exists
						if (!job->reuseData || (job->reuseData && access(instanceFileName, 0))) {
							RiBegin(instanceFileName);
							instanceGeometry();
							RiEnd();
						}
					}
				}
			}
		}

		// output job summary
		if (g_JobSummary) {
			FILE* stream;	
			CString jobSummary, sceneName, userID;
			wchar_t* userid = _wgetenv(L"USER");
			userID = CString(userid);

			sceneName = stringAfterLastAppearance(m_app.GetActiveProject().GetFullName(), L"\\");
			if (g_RenderMode == L"immediate")
				jobSummary = g_JobPath + sceneName + L"." + job->ribFileName + L".job_summary." + CValue((long)g_CurrentFrame).GetAsText() + L".dat"; 
			else if (g_RenderMode == L"deferred")
				jobSummary = g_JobPath + sceneName + L"." + job->ribFileName + L".job_summary.dat";

			if ((stream = _wfopen(jobSummary.GetWideString(), L"w")) == NULL)
				m_app.LogMessage(L"Could not create job summary " + jobSummary + L"!");
			else {		
				fwprintf(stream, L"# XSIMan 1.0 Job Summary\n");
				vector<taskStruct*>::iterator iter;
				for (iter=renderList.begin(); iter!=renderList.end(); ++iter) {
					taskStruct* task = *iter;
					CString fileName = task->fileName + L"\n";
					fwprintf(stream, fileName.GetWideString());
				}
			}

			fclose(stream);
		}

		// execute prejob (and frame) commands here (textures, shaders)
		if (prejobState == true) {
			FILE* stream;		
			CString sceneName = stringAfterLastAppearance(m_app.GetActiveProject().GetFullName(), L"\\");
			CString prejobScript = g_JobPath + sceneName + L"_" + job->ribFileName + L"." + g_CameraName + L"_prejob.bat"; 

			if ((stream = _wfopen(prejobScript.GetWideString(), L"w")) == NULL) {
				m_app.LogMessage(L"Could not create pre-job script " + prejobScript + L"!");
			}
			else {
				vector<CString>::iterator jter;
				for (jter=prejobList.begin(); jter!=prejobList.end(); ++jter) {
					CString jobCommand = *jter + L"\n"; 
					fwprintf(stream, jobCommand.GetWideString());
				}

				fclose(stream);

				execute(L"\"" + prejobScript + L"\"");
				cleanupList.push_back(prejobScript);
			}
		}
		else {
			FILE* stream;		
			CString sceneName = stringAfterLastAppearance(m_app.GetActiveProject().GetFullName(), L"\\");
			CString preframeScript = g_JobPath + sceneName + L"_" + job->ribFileName + L"." + g_CameraName + L"_preframe.bat"; 

			if ((stream = _wfopen(preframeScript.GetWideString(), L"w")) == NULL) {
				m_app.LogMessage(L"Could not create pre-frame script " + preframeScript + L"!");
			}
			else {
				vector<CString>::iterator jter;
				for (jter=preframeList.begin(); jter!=preframeList.end(); ++jter) {
					CString jobCommand = *jter + L"\n"; 
					fwprintf(stream, jobCommand.GetWideString());
				}

				fclose(stream);

				execute(L"\"" + preframeScript + L"\"");
				cleanupList.push_back(preframeScript);
			}
		}

		// cleanup
		prejobList.clear();
		preframeList.clear();

		// check for custom command
		if (g_RenderCommand == L"custom") {
			if (g_CustomCommand != L"") g_RenderCommand = g_CustomCommand;
			else g_RenderCommand = L"none";
		}
	
		if (g_RenderCommand == L"netrender") {
			g_RenderCommand = L"netrender %%H ";
		}

		// multiproc support
		if (g_NumProcs > 0) {
			if (g_Renderer == L"PRMan" && g_RenderCommand == L"prman") 
				g_RenderCommand += L" -p:" + CValue((long)g_NumProcs).GetAsText();
			if (g_Renderer == L"3Delight" && g_RenderCommand == L"renderdl") 
				g_RenderCommand += L" -p " + CValue((long)g_NumProcs).GetAsText();
			if (g_Renderer == L"Air" && g_RenderCommand == L"air" && job->jobType != ShadowJob) 
				g_RenderCommand += L" -threads " + CValue((long)g_NumProcs).GetAsText();
		}
		
		if (g_ShowProgress) {
			g_RenderCommand += L" -Progress";
		}

		// execute render command
		if (!(g_RenderCommand == L"none") && (g_RenderMode == L"immediate") && !g_UseJobScript) {
					
			if (g_LaunchRender) {
				// execute shell script job 
				fileName = shellJobScript();
				CString jobScript = L"cmd.exe /c call \"" + fileName + L"\"";

				// when in immediate mode jobs should hang until finished (to not run out of renderer licenses)
				execute(jobScript);
			}

			renderList.clear();
			cleanupList.clear();
		}
				
		// execute postframe script
		if (g_PostFrameScript != L"") {
			m_app.ExecuteScriptCode(g_PostFrameScript, g_ScriptLanguage);
		}

		// we need to step through one frame at a time otherwise simulations don't update correctly
		if (fnum < g_EndFrame) {
			for (long next = fnum + 1; next <= fnum + g_StepSize; next++) {
				CValueArray args; CValue index;
				//playControl.PutParameterValue(L"current", (long)next);
				m_app.ExecuteCommand(L"NextFrame", args, index);
				m_app.ExecuteCommand(L"Refresh", args, index);
			}
		}

		// increment progress
		CValue newVal(bar.Increment());

		// set prejob state
		prejobState = false;

	}
	
	// execute postjob script
	if (g_PostJobScript != L"") {
		m_app.ExecuteScriptCode(g_PostJobScript, g_ScriptLanguage);
	}

	// if using job script then render in deferred mode
	if (g_UseJobScript) g_RenderMode = L"deferred";

	// render deferred jobs here
	if (!(g_RenderCommand == L"none") && (g_RenderMode == L"deferred")) {

		// are we using a job script?
		CString jobScript, fileName;
		if (g_UseJobScript) {
			fileName = alfredJobScript();
			if (g_JobScriptType == L"Alfred") jobScript = L"alfred \"" + fileName + L"\"";
			if (g_LaunchRender) execute(jobScript);
		}
		else if (g_LaunchRender) {
			// execute shell script job 
			fileName = shellJobScript();
			CString jobScript = L"cmd.exe /c call \"" + fileName + L"\"";
			
			if (g_BatchMode) {
				execute(jobScript);
			}
			else {
				createProcess(jobScript);
			}
		}

		// cleanup
		renderList.clear();
		cleanupList.clear();
	}

	// reset to original frame
	playControl.PutParameterValue(L"current", currentFrame);
	m_app.ExecuteCommand(L"Refresh", args, index);
	
	return CStatus::OK;
}


CStatus xsiRibTranslator::scanScene(XSI::SceneItem& in_obj, CString clusterName)
{
	if (!in_obj.IsValid()) {
		return CStatus::InvalidArgument;
	}

	X3DObject x3dobject(in_obj);
	CString objName = in_obj.GetFullName();

	Geometry geom(x3dobject.GetActivePrimitive().GetGeometry(g_CurrentFrame));

	if (in_obj.IsA(siModelID)) {
		nodeData.insert(in_obj, g_CurrentFrame, 0, ModelObject, clusterName);
	}
	else if (in_obj.IsA(siCameraID)) {
		nodeData.insert(in_obj, g_CurrentFrame, 0, CameraObject, clusterName);
		appendBeautyJob(x3dobject);
	}
	else if (in_obj.IsA(siLightID)) {
		// check if rendering selected lights
		bool renderLight = true;
		if (g_RenderSelectedLights) {
			renderLight = false;
			Selection sel(m_app.GetSelection());
			for (long i = 0; i < sel.GetCount(); i++) {
				if (objName == SIObject(sel[i]).GetFullName()) {
					renderLight = true;
					break;
				}
			}
		}
		
		// don't export invisible lights
		bool renderVisibility = true;
		CRefArray propRefArray = in_obj.GetProperties();
		for (long i = 0; i < propRefArray.GetCount(); i++ ) {
			if (SIObject(propRefArray[i]).GetName() == L"Visibility") {
				Property prop = propRefArray[i];;
				Parameter rendvis = prop.GetParameters().GetItem(L"rendvis");
				renderVisibility = rendvis.GetValue();
				break;
			}
		}

		if (renderLight && renderVisibility) {
			nodeData.insert(in_obj, g_CurrentFrame, 0, LightObject, clusterName);

			Light xsilight(x3dobject);
			if (xsilight.GetParameterValue(CString(L"ShadowMap"))) appendShadowJob(x3dobject);

			// check if we need to create photon map
			if (xsilight.GetParameterValue(CString(L"Caustics"))) appendCausticMapJob(x3dobject);
			if (xsilight.GetParameterValue(CString(L"GlobalIllum"))) appendGlobalMapJob(x3dobject);
		}
	}
	else if (in_obj.IsA(siNullID)){	
		// check if rendering selected objects
		bool renderObject = true;
		if (g_RenderSelectedObjects) {
			renderObject = false;
			Selection sel(m_app.GetSelection());
			CRef selectedItem = sel.GetItem(objName);
			if (selectedItem.IsValid())
				renderObject = true;
		}

		if (renderObject) {
			nodeData.insert(in_obj, g_CurrentFrame, 0, NullObject, clusterName);
		}
	}
	else if (in_obj.IsA(siX3DObjectID)){
		// check if rendering selected objects
		bool renderObject = true;
		if (g_RenderSelectedObjects) {
			renderObject = false;
			Selection sel(m_app.GetSelection());
			CRef selectedItem = sel.GetItem(objName);
			if (selectedItem.IsValid())
				renderObject = true;
		}

		if (renderObject) {

			// check if this is meant to be rib archive
			CRef propRef = in_obj.GetProperties().GetItem(L"XSIManArchive");
			if (propRef.IsValid()) {
				appendArchiveJob(x3dobject);
			}

			// check if we need to create environment map
			propRef = in_obj.GetProperties().GetItem(L"XSIManEnvmap");
			if (propRef.IsValid()) {
				Property prop = propRef;
				Parameter envmap = prop.GetParameters().GetItem(L"envmap");
				if (envmap.GetValue() == true)
					appendEnvmapJob(x3dobject);
			}

			switch (geom.GetRef().GetClassID()) {
				case siPolygonMeshID: {
					bool isSubdivMesh = false;
					CRef propRef = in_obj.GetProperties().GetItem(L"XSIManSubdiv");
					if (propRef.IsValid()) {
						Property prop = propRef;
						Parameter subdiv = prop.GetParameters().GetItem(L"subdiv");
						isSubdivMesh = subdiv.GetValue();
					}

					if (isSubdivMesh) {
						nodeData.insert(in_obj, g_CurrentFrame, 0, SubdivisionObject, clusterName);
					}
					else {
						// if there are polyclusters, bypass object material
						bool bypass = false;
						CRefArray clusterRefArray;
						geom.GetClusters().Filter(siPolygonCluster, CStringArray(), L"", clusterRefArray);
						for (long i = 0; i < clusterRefArray.GetCount(); i++) {
							Cluster cluster(clusterRefArray[i]);
							Material material = cluster.GetMaterial(); 
							if (material.IsValid()) {
								nodeData.insert(in_obj, g_CurrentFrame, 0, MeshObject, cluster.GetFullName());
								bypass = true;
							}
						}

						if (bypass == false) nodeData.insert(in_obj, g_CurrentFrame, 0, MeshObject, clusterName);
					}

					break;
				}
				case siNurbsSurfaceMeshID:
					nodeData.insert(in_obj, g_CurrentFrame, 0, NurbsObject, clusterName);
					break;
				case siNurbsCurveListID: {
					CRef propRef = in_obj.GetProperties().GetItem(L"XSIManWidth");
					if (propRef.IsValid()) {
						nodeData.insert(in_obj, g_CurrentFrame, 0, CurveObject, clusterName);
					}
					break;
				 }
				case siGeometryID:	{	// this could be a hair or particle cloud object
					X3DObject x3dobject(in_obj);
					Primitive prim  = x3dobject.GetActivePrimitive();
					if (prim.IsA(siParticleCloudPrimitiveID)) {
						nodeData.insert(in_obj, g_CurrentFrame, 0, ParticlesObject, clusterName);

						// check for instancing to create archive jobs
						ParticleCloud particleCloud(x3dobject);
						ParticleCloudPrimitive cloudPrim(particleCloud.GetActivePrimitive());
						CRefArray particleTypes = cloudPrim.GetParticleTypes();
						Property parType = particleTypes[0];
						bool instancing = false;
						Parameter instEnable = parType.GetParameters().GetItem(L"Inst_Enable");
						instancing = instEnable.GetValue();
						if (instancing) {
							CRefArray inst_groups;
							parType.GetNestedObjects().Filter(L"#Group", CStringArray(), L"", inst_groups);
							for (long i=0; i<inst_groups.GetCount(); i++) {
								CRef ref(inst_groups[i]);
								Group grp(ref);
								CRefArray members = grp.GetMembers();
								for (long j=0; j<members.GetCount(); j++) {
									X3DObject x3dobj(members[j]);
									appendArchiveJob(x3dobj);
								}
							}
						}
					}
					else if (prim.IsA(siHairPrimitiveID)) {
						nodeData.insert(in_obj, g_CurrentFrame, 0, HairObject, clusterName);
					}
					break;
				}
				default:
					break;
			}
		}
	}

	// add any poly clusters with material
	/*if (geom.GetRef().GetClassID() == siPolygonMeshID) {
		bool isSubdivMesh = false;
		CRef propRef = in_obj.GetProperties().GetItem(L"XSIManSubdiv");
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter subdiv = prop.GetParameters().GetItem(L"subdiv");
			isSubdivMesh = subdiv.GetValue();
		}

		if (isSubdivMesh == false && g_PolygonClusters == true) { 
			CRefArray clusterRefArray;
			geom.GetClusters().Filter(siPolygonCluster, CStringArray(), L"", clusterRefArray);
			for (long i = 0; i < clusterRefArray.GetCount(); i++) {
				Cluster cluster(clusterRefArray[i]);
				Material material = cluster.GetMaterial(); 
				if (material.IsValid() && clusterName == L"") {
					scanScene(in_obj, cluster.GetFullName());
				}
			}
		}
	}*/
	
	// if any inclusive lights have associated models then turn off illumination globally for that light 
	CRefArray owners(x3dobject.GetOwners());
	for (long j = 0; j < owners.GetCount(); ++j) {
		SIObject siobj(owners[j]);
		if (siobj.IsA(siGroupID)) {
			SIObject objparent = siobj.GetParent();
			if (objparent.GetType() == L"light") {
				Light xsilight(objparent.GetParent());
				bool inclusive = xsilight.GetParameterValue(CString(L"SelectiveInclusive"));
				CString lgtname = xsilight.GetFullName();
				if (inclusive) lightSwitch.push_back(lgtname);
			}
		}
	}

	// get the children
	CRefArray refChildrenArray(x3dobject.GetChildren());
	long numkids = refChildrenArray.GetCount();
	for(long i = 0; i <= numkids; i++){
		if ((i == numkids) && in_obj.IsA(siModelID)) {
			nodeData.insert(in_obj, g_CurrentFrame, 0, EndModelObject, clusterName);
			break;
		}
		SceneItem sceneItem(refChildrenArray[i]);
		scanScene(sceneItem, L"");
	}

	return CStatus::OK;
}

CStatus xsiRibTranslator::ribPrologue()
{
	if (g_ExportAsArchive == true) return CStatus::Fail;

	if (job->jobType != PhotonMapJob) {
		RiOption("limits", "bucketsize", (RtPointer)&bucketSize, RI_NULL);
		if (g_Renderer != L"Air") RiOption("limits", "gridsize", (RtPointer)&gridSize, RI_NULL);
		RiOption("limits", "texturememory", (RtPointer)&textureMemory, RI_NULL);
		RiOption("limits", "eyesplits", (RtPointer)&eyeSplits, RI_NULL);
		if (g_Renderer == L"PRMan" || g_Renderer == L"3Delight") {
			RtColor zthres = {g_OpacityThresholdR, g_OpacityThresholdG, g_OpacityThresholdB};
			RiOption("limits", "zthreshold", (RtPointer)zthres, RI_NULL);
		}
		if (g_Renderer == L"PRMan") {
			RtColor othres = {g_OpacityCullingR, g_OpacityCullingG, g_OpacityCullingB};
			RiOption("limits", "othreshold", (RtPointer)othres, RI_NULL);
		}
	}

	RiOption("statistics", "endofframe", (RtPointer)&g_RenderStatistics, RI_NULL);
	if (g_StatisticsFileName != L"") {
		char* statisticsFileName = NULL;
		XSIW2A(&statisticsFileName, g_StatisticsFileName.GetWideString());
		RiOption("statistics", "filename", (RtPointer)&statisticsFileName, RI_NULL);
	}
	if (g_StatisticsXMLFileName != L"") {
		char* statisticsXMLFileName = NULL;
		XSIW2A(&statisticsXMLFileName, g_StatisticsXMLFileName.GetWideString());
		//RiOption("statistics", "xmlfilename", (RtPointer)&statisticsXMLFileName, RI_NULL);
		RiArchiveRecord(RI_VERBATIM, "Option \"statistics\" \"xmlfilename\" [\"%s\"]\n", statisticsXMLFileName);
	}

	// add project shader path 
	g_SloPath += L":" + g_ShaderPath;

	// replace back slashes
	g_SloPath = replaceSubString(g_SloPath, L"\\", L"/");
	g_ImgPath = replaceSubString(g_ImgPath, L"\\", L"/");
	g_DsoPath = replaceSubString(g_DsoPath, L"\\", L"/");

	// define resource directories
	char* shaderPath = NULL;
	XSIW2A(&shaderPath, g_SloPath.GetWideString());
	char* texturePath = NULL;
	XSIW2A(&texturePath, g_ImgPath.GetWideString());
	char* dsoPath = NULL;
	XSIW2A(&dsoPath, g_DsoPath.GetWideString());

	RiOption("searchpath", "shader", (RtPointer)&shaderPath, RI_NULL);
	RiOption("searchpath", "texture", (RtPointer)&texturePath, RI_NULL);
	RiOption("searchpath", "procedural", (RtPointer)&dsoPath, RI_NULL);

	CRef propRef = m_sceneRoot.GetProperties().GetItem(L"XSIManOptions");
	Property prop(propRef);
	Parameter DirMap = prop.GetParameters().GetItem(L"DirMap");
	GridData dirMaps = DirMap.GetValue();
	CString w_maps, zone, from, to;
	w_maps = L"[ ";
	long rowCount = 0;
	for (long i = 0; i < dirMaps.GetRowCount(); i++) {
		zone = (dirMaps.GetCell(0, i) ? L"NFS" : L"UNC");
		from = dirMaps.GetCell(1, i);
		to = dirMaps.GetCell(2, i);
		if (from == L"" || to == L"") continue;
		rowCount += 1;
		w_maps += L"\"[ ";
		w_maps += L"\\\"";
		w_maps += zone;
		w_maps += L"\\\"";
		w_maps += L" ";
		w_maps += L"\\\"";
		w_maps += from;
		w_maps += L"\\\"";
		w_maps += L" ";
		w_maps += L"\\\"";
		w_maps += to;
		w_maps += L"\\\"";
		w_maps += L" ";
		w_maps += L"]\"";
		w_maps += L" ";
	}
	w_maps += L"]";
	char* maps = NULL;
	XSIW2A(&maps, w_maps.GetWideString());
	if (rowCount > 0) RiArchiveRecord(RI_VERBATIM, "Option \"searchpath\" \"dirmap\" %s\n", maps);

	if (job->jobType == ShadowJob) {
		RiPixelSamples((RtFloat)job->shadowPixelSamples, (RtFloat)job->shadowPixelSamples);
		RiShadingRate(1);
		RiPixelFilter(RiBoxFilter, 1, 1);
    } 
	else if (job->jobType != PhotonMapJob) {
		if (g_Renderer != L"Air") {
			RiOption("shadow", "bias0", (RtPointer)&shadowBias, RI_NULL);
			RiOption("shadow", "bias1", (RtPointer)&shadowBias, RI_NULL);
		} 

		RiPixelSamples((RtFloat)pixelSamples[0], (RtFloat)pixelSamples[1]);
		RiShadingRate(shadingRate);
		writePixelFilter(pixelFilter, filterSize[0], filterSize[1]);
	}

	return CStatus::OK;
}

void xsiRibTranslator::writePixelFilter(PixelFilterType pixelFilter, float sizex, float sizey) 
{
	if (filterSize[0] > 1 || filterSize[1] > 1) {
		switch (pixelFilter) {
			case BoxFilter:
				RiPixelFilter(RiBoxFilter, sizex, sizey);
				break;
			case TriangleFilter:
				RiPixelFilter(RiTriangleFilter, sizex, sizey);
				break;
			case CatmullRomFilter:
				RiPixelFilter(RiCatmullRomFilter, sizex, sizey);
				break;
			case GaussianFilter:
				RiPixelFilter(RiGaussianFilter, sizex, sizey);
				break;
			case SincFilter:
				RiPixelFilter(RiSincFilter, sizex, sizey);
				break;
			case BlackmanHarrisFilter:
				RiArchiveRecord(RI_VERBATIM, "PixelFilter \"blackman-harris\" %g %g\n", sizex, sizey);
				break;
			case MitchellFilter:
				RiArchiveRecord(RI_VERBATIM, "PixelFilter \"mitchell\" %g %g\n", sizex, sizey);
				break;
			case SepCatmullRomFilter:
				RiArchiveRecord(RI_VERBATIM, "PixelFilter \"separable-catmull-rom\" %g %g\n", sizex, sizey);
				break;
			case BesselFilter:
				RiArchiveRecord(RI_VERBATIM, "PixelFilter \"bessel\" %g %g\n", sizex, sizey);
				break;
			default:
				RiArchiveRecord(RI_COMMENT, "Unknown filter type select");
				break;
		}
	}
}

CStatus xsiRibTranslator::ribEpilogue()
{
	return CStatus::OK;
}

CStatus xsiRibTranslator::instanceGeometry()
{	
	float sampleIncrement = job->camera[0].shutter / (g_MotionSamples - 1);

	// force RiDeclare to ouput all declares
	if (g_InstanceGeometry == true) {
		int alldeclares = 1;
		RiOption((RtToken) "rib", (RtToken) "outputalldeclares", (RtPointer)&alldeclares, RI_NULL);
	}

	// placed instanced geometry here
	vector<xsiRibNode>::iterator iter;
	for(iter=nodeData.nodeMap.begin(); iter!=nodeData.nodeMap.end(); ++iter) {
		xsiRibNode ribNode = (*iter);
		nodeStruct node = ribNode.get();

		//if ((node.objectType == NullObject) || (node.objectType == LightObject) || (node.objectType == CameraObject)) continue;
		if ((node.objectType == LightObject) || (node.objectType == CameraObject)) continue;

		// check if visible
		if (!node.renderVisibility || (!node.shadowVisibility && job->jobType==ShadowJob)) continue;

		SceneItem sceneItem = node.sceneItem;
		X3DObject x3dobject(sceneItem);

		if (node.instanceGeometry) {
			char* fullName = NULL;
			XSIW2A(&fullName, node.fullName.GetWideString());
			RiArchiveRecord(RI_COMMENT, "Archive Instance for %s", fullName);

			// basis function definition is a little screwy so it also needs to be defined here in archive file
			if (node.objectType == HairObject || node.objectType == CurveObject) {
				RiBasis(RiBSplineBasis, 1, RiBSplineBasis, 1);
			}

			long motionSamples = 1;
			if (g_DeformBlur) motionSamples = node.motionSamples + 1;
			
			float sampleTime = g_CurrentFrame;
			if (g_ShutterTiming == L"OpenOnFrame") sampleTime = g_CurrentFrame;
			else if (g_ShutterTiming == L"CloseOnFrame") sampleTime = g_CurrentFrame - job->camera[0].shutter;
			else if (g_ShutterTiming == L"CenterOnFrame") sampleTime = g_CurrentFrame - 0.5f * job->camera[0].shutter;

			for (long i=0; i<motionSamples; i++) {
				char* instanceTag = "motion_sample";
				RiArchiveRecord(RI_VERBATIM, "ArchiveBegin \"%s_%s_%d\"\n", fullName, instanceTag, i);
				node.handle[i] = fullName;

				if (node.objectType == MeshObject) {
					xsiRibMeshData meshData(x3dobject, sampleTime + i*sampleIncrement, node.clusterName);
					meshData.write();
				}
				else if (node.objectType == SubdivisionObject) {
					xsiRibSubdivisionData subdivData(x3dobject, sampleTime + i*sampleIncrement);
					subdivData.write();
				}
				else if (node.objectType == NurbsObject) {
					xsiRibSurfaceData surfaceData(x3dobject, sampleTime + i*sampleIncrement);
					if (surfaceData.hasTrimCurves()) surfaceData.writeTrims();
					surfaceData.write();
				}
				else if (node.objectType == ParticlesObject) {
					xsiRibParticlesData particlesData(x3dobject, sampleTime + i*sampleIncrement, job);
					particlesData.write(&node);
				}
				else if (node.objectType == HairObject) {	
					xsiRibHairData hairData(x3dobject, sampleTime + i*sampleIncrement);
					hairData.write();
				}

				RiArchiveRecord(RI_VERBATIM, "ArchiveEnd\n");
			}

			// curve objects need special handling
			// limit curve objects to one curve (this is to deal with adding prim vars)
			if (node.objectType == CurveObject) {
				Geometry curvegeom(x3dobject.GetActivePrimitive().GetGeometry(sampleTime));
				NurbsCurveList nurbscurvelist(curvegeom);
				CNurbsCurveDataArray nurbsCurveDataArray;
				nurbscurvelist.Get(siIGESNurbs, nurbsCurveDataArray);
				long ncurves = nurbsCurveDataArray.GetCount();

				for (long ci=0; ci<ncurves; ci++) {
					for (long i=0; i<motionSamples; i++) {
						char* instanceTag = "motion_sample";
						RiArchiveRecord(RI_VERBATIM, "ArchiveBegin \"%s_%s_%d_%d\"\n", fullName, instanceTag, ci, i);
						node.handle[i] = fullName;

						xsiRibCurveData curveData(x3dobject, sampleTime + i*sampleIncrement, ci);
						curveData.write();

						RiArchiveRecord(RI_VERBATIM, "ArchiveEnd\n");
					}
				}
			}
			
			// set object handles
			xsiRibNode nuRibNode;
			nuRibNode.put(node);
			(*iter) = nuRibNode;
		}
	}

	return CStatus::OK;
}

CStatus xsiRibTranslator::framePrologue()
{
	// if we are creating an instance archive then this has already been called
	if (g_InstanceGeometry == false) {
		instanceGeometry();
	}
	else {	
		// get archive file name
		wchar_t padframe[10];
		swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);
		CString fileName = g_RibPath + job->ribFileName + L"." + g_CameraName + L".INST." + padframe + L".rib";
			
		// we seem to need to replace back slashes
		CString nuFileName = replaceSubString(fileName, L"\\", L"/");
		char* instanceFileName = NULL;
		XSIW2A(&instanceFileName, nuFileName.GetWideString());

		RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s\"\n", instanceFileName);
	}

	float sampleIncrement = job->camera[0].shutter / (g_MotionSamples - 1);

	// generate environment maps here
	vector<xsiRibNode>::iterator iter;
	if (job->jobType != EnvmapJob) {
		for(iter=nodeData.nodeMap.begin(); iter!=nodeData.nodeMap.end(); ++iter) {
			xsiRibNode ribNode = (*iter);
			nodeStruct node = ribNode.get();

			//if ((node.objectType == NullObject) || (node.objectType == LightObject) || (node.objectType == CameraObject)) continue;
			if ((node.objectType == LightObject) || (node.objectType == CameraObject)) continue;

			SceneItem sceneItem = node.sceneItem;
			X3DObject x3dobject(sceneItem);

			if (node.generateEnvmap) {
				jobStruct* envmapJob = new jobStruct;
				envmapJob->imageName = job->imageName;
				envmapJob->writeFrequency = job->writeFrequency;
				envmapJob->name = node.fullName;
				envmapJob->jobType = EnvmapJob; 
				envmapJob->fileExt = L".tex";
				envmapJob->isPointLight = false;
				CString imageName = constructFileName(false, envmapJob);
				imageName = replaceSubString(imageName, L"\\", L"/");
				char* envmap;
				XSIW2A(&envmap, imageName.GetWideString());
				envmapJob->isEnvmap = true; 
				envmapJob->fileExt = L".tif";
				char* mapface[6];
				for (int faceView = 0; faceView < 6; faceView++) {
					envmapJob->cubeMapDir = faceView;
					imageName = constructFileName(false, envmapJob);
					imageName = replaceSubString(imageName, L"\\", L"/");
					XSIW2A(&mapface[faceView], imageName.GetWideString());
				}

				if (!job->reuseData || (job->reuseData && access(envmap, 0))) {
					RiMakeCubeFaceEnvironment(mapface[0], mapface[1], mapface[2], mapface[3], mapface[4], mapface[5], 
						envmap, 90.0f, RiGaussianFilter, 2.0, 2.0, RI_NULL);
				}
			}
		}
	}

	RiFrameBegin((RtInt)g_CurrentFrame);

	// dicing options
	if (g_Renderer == L"PRMan") {	
		char* dicingStrategy = NULL; char* dicingCamera = NULL;
		XSIW2A(&dicingStrategy, g_DicingStrategy.GetWideString());
		XSIW2A(&dicingCamera, g_DicingCamera.GetWideString());
		RiAttribute("dice", "strategy", &dicingStrategy, RI_NULL);
		//RiAttribute("dice", "referencecamera", &dicingCamera, RI_NULL);
		RiArchiveRecord(RI_VERBATIM, "Attribute \"dice\" \"referencecamera\" [\"%s\"]\n", dicingCamera);
	}
	if (g_Renderer == L"PRMan" || g_Renderer == L"3Delight") {
		RiAttribute("dice", "rasterorient", &g_RasterOriented, RI_NULL);
	}

	// raytrace options
    if (g_EnableRayTracing && job->jobType != ShadowJob && job->jobType != PhotonMapJob) {
		RiOption("user", "float tracebreadthfactor", (RtPointer)&g_RayTraceBreadthFactor, "float tracedepthfactor", (RtPointer)&g_RayTraceDepthFactor, RI_NULL);
		RiOption("trace", "int maxdepth", (RtPointer)&g_MaxRayDepth, RI_NULL);
	}

	
	if (job->jobType == PhotonMapJob) {
		RtInt photons = job->photons;
		RiHider("photon", "emit", &photons, RI_NULL);
	}
	else {
		RiShadingInterpolation("smooth");

		if (quantValue == 32) RiQuantize(RI_RGBA, 0, 0, 0, 0);
		else RiQuantize(RI_RGBA, quantizeOne, quantizeMin, quantizeMax, dither);

		if (gainValue != 1 || gammaValue != 1) {
			RiExposure(gainValue, gammaValue);
		}

		// check the shadow type
		if ((job->jobType == ShadowJob) && (job->shadowMapType != L"deepshad")) {
			RtInt zero = 0;
			RiHider("hidden", "jitter", &zero, RI_NULL);
		}
		else if ((job->jobType == ShadowJob) && (job->shadowMapType == L"deepshad")) {
			RtInt one = 1;
			RiHider("hidden", "jitter", &one, RI_NULL);
		}

		if (job->shadowMapType == L"midpoint") {
			RtString midpoint = "midpoint";
			RiHider("hidden", "depthfilter", &midpoint, RI_NULL);
		}

		if ((cropWindowX1 != 0) || (cropWindowY1 != 0) || (cropWindowX2 != 1) || (cropWindowY2 != 1)) {
			RiCropWindow(cropWindowX1, cropWindowX2, cropWindowY1, cropWindowY2);
		}
	}

	// frame padding
	wchar_t padframe[10];
    swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);

	// construct output image file name 
	CString fileName = constructFileName(false, job);

	// do we cleanup generated maps?
	if (job->jobType == ShadowJob || job->jobType == EnvmapJob || job->jobType == PhotonMapJob) {
		if (g_Cleanup == 16 || g_Cleanup == 17 || g_Cleanup == 20 || g_Cleanup == 21) 
			cleanupList.push_back(fileName);
	}

	// we seem to need to replace back slashes with forward slashes?
	CString nuFileName = replaceSubString(fileName, L"\\", L"/");

	char* format = NULL; char* imageMode = NULL; char* imageName = NULL;
	XSIW2A(&format, job->format.GetWideString());
	XSIW2A(&imageMode, job->imageMode.GetWideString());

	// check what kind of shadow map this might be
	if (job->jobType == ShadowJob) {
		if (job->shadowMapType == L"deepshad" && g_Renderer != L"Air") {
			char* shadowVolInterp = NULL;
			RiDeclare("volumeinterpretation", "string");
			CString fname = L"+" + nuFileName;
			XSIW2A(&imageName, fname.GetWideString());
			XSIW2A(&shadowVolInterp, job->shadowVolInterp.GetWideString());
			RiDisplay("null", "null", "z", RI_NULL);
			RiDisplay(imageName, format, imageMode, "volumeinterpretation", &shadowVolInterp, RI_NULL);
		}
		else if (job->shadowMapType == L"minmax") {
			XSIW2A(&imageName, nuFileName.GetWideString());
			RiArchiveRecord(RI_COMMENT, "\nDisplay \"%s\" \"%s\" \"%s\" \"minmax\" [ 1 ]", imageName, format, imageMode);
		}
		else {
			XSIW2A(&imageName, nuFileName.GetWideString());
			RiDisplay(imageName, format, imageMode, RI_NULL);
		}
	}
	else if (job->jobType != PhotonMapJob) {
		XSIW2A(&imageName, nuFileName.GetWideString());
		RiDisplay(imageName, format, imageMode, RI_NULL);

		// AOVs
		if (!g_PrimaryDisplay) {

			// get contents of AOVList
			long rowCount = g_AOVList.GetRowCount();

			for (long j = 0; j < rowCount; j++) {
				CString aovname = g_AOVList.GetCell(0, j);
				// skip if no name
				if (aovname != L"") {
					CString aovtype = g_AOVList.GetCell(1, j);
					CString aovmode = g_AOVList.GetCell(2, j);
					CString aovfilter = g_AOVList.GetCell(3, j);
					CString aovsizex = g_AOVList.GetCell(4, j);
					CString aovsizey = g_AOVList.GetCell(5, j);
					CString aovquantzero = g_AOVList.GetCell(6, j);
					CString aovquantone = g_AOVList.GetCell(7, j);
					CString aovquantmin = g_AOVList.GetCell(8, j);
					CString aovquantmax = g_AOVList.GetCell(9, j);
					CString aovquantdither = g_AOVList.GetCell(10, j);

					// determine file extension
					CString fileExt = getFileExt(aovtype);
					fileName = L"+" + g_PixPath + job->imageName + L"." + job->camera[0].name + L"." + aovname + L"." + padframe + fileExt;
					fileName = replaceSubString(fileName, L"\\", L"/");

					char* filter = NULL; XSIW2A(&filter, aovfilter.GetWideString());
					char* sizex = NULL; XSIW2A(&sizex, aovsizex.GetWideString());
					char* sizey = NULL; XSIW2A(&sizey, aovsizey.GetWideString());
					char* aovName = NULL; XSIW2A(&aovName, aovname.GetWideString());
					char* imageName = NULL; XSIW2A(&imageName, fileName.GetWideString());
					char* format = NULL; XSIW2A(&format, aovtype.GetWideString());
					char* variable = NULL; XSIW2A(&variable, aovmode.GetWideString());
					char* quantzero = NULL; XSIW2A(&quantzero, aovquantzero.GetWideString());
					char* quantone = NULL; XSIW2A(&quantone, aovquantone.GetWideString());
					char* quantmin = NULL; XSIW2A(&quantmin, aovquantmin.GetWideString());
					char* quantmax = NULL; XSIW2A(&quantmax, aovquantmax.GetWideString());
					char* quantdither = NULL; XSIW2A(&quantdither, aovquantdither.GetWideString());

					RiArchiveRecord(RI_COMMENT, "Arbitrary Output Variable %s", aovName);
					writePixelFilter((PixelFilterType)atoi(filter), (float)atof(sizex), (float)atof(sizey));
					RiArchiveRecord(RI_VERBATIM, "Display \"%s\" \"%s\" \"%s\" \"quantize\" [ %s %s %s %s ] \"dither\" [ %s ]\n", 
						imageName, format, variable, quantzero, quantone, quantmin, quantmax, quantdither);
				}
			}
		}
	}

	if (job->jobType != PhotonMapJob) {
		RiFormat(job->width, job->height, job->aspectRatio);
		RiSides(2);     

		if (job->camera[0].orthogonal) {
			RiProjection("orthographic", RI_NULL);
			RtFloat frameWidth, frameHeight;
			RtFloat frameAspectRatio = job->aspectRatio * (float)job->width / (float)job->height;
			frameWidth = job->camera[0].orthoWidth;
			frameHeight = job->camera[0].orthoHeight;
			if (frameAspectRatio >= 1) RiScreenWindow(-frameWidth, frameWidth, -frameHeight/frameAspectRatio, frameHeight/frameAspectRatio);
			else RiScreenWindow(-frameAspectRatio*frameWidth, frameAspectRatio*frameWidth, -frameHeight, frameHeight);
		}
		else {
			RtFloat fieldOfView = job->camera[0].horizFOV;
			RiProjection("perspective", RI_FOV, &fieldOfView, RI_NULL);
			RtFloat frameAspectRatio = job->aspectRatio * (float)job->width / (float)job->height;
			if (frameAspectRatio >= 1) RiScreenWindow(-1, 1, -1/frameAspectRatio, 1/frameAspectRatio);
			else RiScreenWindow(-frameAspectRatio, frameAspectRatio, -1, 1);
		}

		RiClipping(job->camera[0].nearClip, job->camera[0].farClip);

		// depth of field
		if ((job->camera[0].fStop && job->camera[0].focalDistance)  && job->jobType != ShadowJob) {
			RiDepthOfField(job->camera[0].fStop, job->camera[0].focalLength, job->camera[0].focalDistance);
		}
 
		// get shutter timing
		float sampleTime = g_CurrentFrame;
		if (g_ShutterTiming == L"OpenOnFrame") sampleTime = g_CurrentFrame;
		else if (g_ShutterTiming == L"CloseOnFrame") sampleTime = g_CurrentFrame - job->camera[0].shutter;
		else if (g_ShutterTiming == L"CenterOnFrame") sampleTime = g_CurrentFrame - 0.5f * job->camera[0].shutter;

		if ((g_MotionBlur || g_DeformBlur || g_CameraBlur) && (job->jobType != ShadowJob || job->shadowBlur == true)) {
			RiShutter(sampleTime, sampleTime + job->camera[0].shutter);
		}
		else {
			RiShutter(g_CurrentFrame, g_CurrentFrame);
		}

		// camera motion blur
		if (g_CameraBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
			RiMotionBegin(2, sampleTime, sampleTime + sampleIncrement);
		}

		// cameras in XSI seem to be flipped
		double flipCamera[] = {1.0, 1.0, -1.0, 1.0};

		RtMatrix cameraMatrix;
		MATH::CMatrix4 nucam;
		nucam.Invert(job->camera[0].matrix4);
		for (unsigned i = 0; i < 4 ; i++) {
			for (unsigned j = 0; j < 4 ; j++) {
				cameraMatrix[i][j] = (float)(nucam.GetValue(i,j) * flipCamera[j]);
			}
		}

		// camera transform
		// point lights and cubic environment maps need special attention
		RtMatrix nuCameraMatrix;
		if (job->jobType == EnvmapJob) {
			float angle = (float)(PI / 2.);

			if (job->cubeMapDir == PX) rotateMatrix(nuCameraMatrix, cameraMatrix, angle, 1);
			else if (job->cubeMapDir == NX) rotateMatrix(nuCameraMatrix, cameraMatrix, -angle, 1);
			else if (job->cubeMapDir == PY) rotateMatrix(nuCameraMatrix, cameraMatrix, -angle, 0);
			else if (job->cubeMapDir == NY) rotateMatrix(nuCameraMatrix, cameraMatrix, angle, 0);
			else if (job->cubeMapDir == PZ) rotateMatrix(nuCameraMatrix, cameraMatrix, 0, 2); 
			else if (job->cubeMapDir == NZ) rotateMatrix(nuCameraMatrix, cameraMatrix, -(float)PI, 1);

			RiTransform(nuCameraMatrix);
		}
		else if ((job->jobType == ShadowJob) && job->isPointLight) {
			float angle = (float)(PI / 2.0);

			if (job->cubeMapDir == PX) rotateMatrix(nuCameraMatrix, cameraMatrix, angle, 1);
			else if (job->cubeMapDir == NX) rotateMatrix(nuCameraMatrix, cameraMatrix, -angle, 1);
			else if (job->cubeMapDir == PY) rotateMatrix(nuCameraMatrix, cameraMatrix, -angle, 0);
			else if (job->cubeMapDir == NY) rotateMatrix(nuCameraMatrix, cameraMatrix, angle, 0);
			// transforms on cameras and lights in XSI are flipped in the z sense from PRMan
			// I think this is why I have to treat these rotations differently than for env maps.
			else if (job->cubeMapDir == PZ) rotateMatrix(nuCameraMatrix, cameraMatrix, (float)PI, 1); 
			else if (job->cubeMapDir == NZ) rotateMatrix(nuCameraMatrix, cameraMatrix, 0, 1);

			RiTransform(nuCameraMatrix);
		}
		else {
			RiTransform(cameraMatrix);
		}

		// camera motion blur
		if (g_CameraBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
			nucam.Invert(job->camera[1].matrix4);
			for (unsigned i = 0; i < 4 ; i++) {
				for (unsigned j = 0; j < 4 ; j++) {
					cameraMatrix[i][j] = (float)(nucam.GetValue(i,j) * flipCamera[j]);
				}
			}
			RiTransform(cameraMatrix);

			RiMotionEnd();
		}
	}

	// motion factor
	if (g_MotionFactor > 1.0) 
		RiGeometricApproximation("motionfactor", (RtFloat)g_MotionFactor);

	return CStatus::OK;
}

CStatus xsiRibTranslator::frameBody()
{
	CStatus stat = CStatus::OK;

	if (job->jobType != ArchiveJob) {
		RiWorldBegin();

		RiTransformBegin();
		RiCoordinateSystem("worldspace");
		RiTransformEnd();
	}

	if (g_BinaryDicing == true) {
		RtInt dice = 1;
		RiAttribute("dice", "binary", (RtPointer)&dice, RI_NULL);
	}

	// raytrace global options
	if (g_EnableRayTracing && (job->jobType != ArchiveJob)) {

		// photon map stuff
		if (g_PhotonCausticMap != L"" && g_Renderer != L"3Delight") {
			char* causticMap = NULL;
			g_PhotonCausticMap = replaceSubString(g_PhotonCausticMap, L"\\", L"/");
			XSIW2A(&causticMap, g_PhotonCausticMap.GetWideString());
			RiAttribute("photon", "causticmap", &causticMap, RI_NULL);
		}
		if (g_PhotonGlobalMap != L"" && g_Renderer != L"3Delight") {
			char* globalMap = NULL;
			g_PhotonGlobalMap = replaceSubString(g_PhotonGlobalMap, L"\\", L"/");
			XSIW2A(&globalMap, g_PhotonGlobalMap.GetWideString());
			RiAttribute("photon", "globalmap", &globalMap, RI_NULL);
		}

		if (g_PhotonEstimator > 0 && g_Renderer != L"3Delight") 
			RiAttribute("photon", "estimator", &g_PhotonEstimator, RI_NULL);

		if (g_PhotonShadingModel != L"" && g_Renderer != L"3Delight") {
			char* shadingModel = NULL;
			XSIW2A(&shadingModel, g_PhotonShadingModel.GetWideString());
			RiAttribute("photon", "shadingmodel", &shadingModel, RI_NULL);
		}

		// general ray tracing controls
		RtInt camvisibility = 1;
		RtInt raytrace = 1;
		RtInt photon = 1;
		RtString transmission = "opaque";
		RiAttribute("visibility", "int camera", &camvisibility, "int trace", &raytrace, "int photon", &photon, "string transmission", &transmission, RI_NULL);
		RiAttribute("trace", "int maxspeculardepth", (RtPointer)&g_RayTraceMaxSpecularDepth, RI_NULL);
		RiAttribute("trace", "int maxdiffusedepth", (RtPointer)&g_RayTraceMaxDiffuseDepth, RI_NULL);
		RiAttribute("trace", "float bias", (RtPointer)&g_RayTraceBias, "int displacements", (RtPointer)&g_RayTraceDisplacements, "int samplemotion", (RtPointer)&g_RayTraceSampleMotion, RI_NULL);
		if(g_RayTraceMaxError != -1) {
			RiAttribute("irradiance", "float maxerror", (RtPointer)&g_RayTraceMaxError, RI_NULL);
		 }
		 if(g_RayTraceMaxPixDist != -1) {
			RiAttribute("irradiance", "float maxpixeldist", (RtPointer)&g_RayTraceMaxPixDist, RI_NULL);
		}

		 // irradiance cache stuff
		if (g_IrradianceCache != L"") {
			char* irradianceCache = NULL; char* irradianceCacheMode = "";
			XSIW2A(&irradianceCache, g_IrradianceCache.GetWideString());
			if (g_IrradianceCacheMode == L"ignore") irradianceCacheMode = "";
			else if (g_IrradianceCacheMode == L"read") irradianceCacheMode = "r";
			else if (g_IrradianceCacheMode == L"write") irradianceCacheMode = "w";
			RiArchiveRecord(RI_VERBATIM, "Attribute \"irradiance\" \"handle\" \"%s\"\n", irradianceCache);
			RiArchiveRecord(RI_VERBATIM, "Attribute \"irradiance\" \"filemode\" \"%s\"\n", irradianceCacheMode);
		}
	}

	// lights
	stat = exportLights();

	// coordinate systems
	stat = exportCoordinateSystems();	

	// are we inserting any front or back plane geometry?
	if (g_FrontplaneGeom == true) {	
		insertFrontplaneGeom();
	}
	if (g_BackplaneGeom == true) {
		insertBackplaneGeom();
	}

	// check for script box attached to scene_root
	Property sprop(m_sceneRoot.GetProperties().GetItem(L"XSIManScriptBox"));
	Parameter ScriptBox = sprop.GetParameters().GetItem(L"ScriptBox");
	CString scriptBox = ScriptBox.GetValue();
		
	long scriptBoxPass = 277;
	Parameter ScriptBoxPass = sprop.GetParameters().GetItem(L"ScriptBoxPass");
	scriptBoxPass = ScriptBoxPass.GetValue();
		
	if (scriptBox != L"") {
		bool addScriptBox = false;
			
		// check if RIBBox should be in this pass (1-beauty; 4-shadow; 16-environment; 256-photon)
		if ((job->jobType == BeautyJob) && (scriptBoxPass == 1 || scriptBoxPass == 5 || scriptBoxPass == 17 || scriptBoxPass == 21 || scriptBoxPass == 257 || scriptBoxPass == 261 || scriptBoxPass == 273 || scriptBoxPass == 277)) addScriptBox = true;
		if ((job->jobType == ShadowJob) && (scriptBoxPass == 4 || scriptBoxPass == 5 || scriptBoxPass == 20 || scriptBoxPass == 21 || scriptBoxPass == 260 || scriptBoxPass == 261 || scriptBoxPass == 276 || scriptBoxPass == 277)) addScriptBox = true;
		if ((job->jobType == EnvmapJob) && (scriptBoxPass == 16 || scriptBoxPass == 17 || scriptBoxPass == 20 || scriptBoxPass == 21 || scriptBoxPass == 272 || scriptBoxPass == 273 || scriptBoxPass == 276 || scriptBoxPass == 277)) addScriptBox = true;
		if ((job->jobType == PhotonMapJob) && (scriptBoxPass == 256 || scriptBoxPass == 257 || scriptBoxPass == 260  || scriptBoxPass == 261  || scriptBoxPass == 272 || scriptBoxPass == 273 || scriptBoxPass == 276 || scriptBoxPass == 277)) addScriptBox = true;

		if (addScriptBox) {
			CValueArray args;
			CValue retval;
			m_app.ExecuteCommand(scriptBox, args, retval);
			char* results = NULL;
			XSIW2A(&results, CString(retval).GetWideString());
			RiArchiveRecord(RI_VERBATIM, "%s\n", results);
		}
	}

	// check for RIB box attached to scene_root
	Property prop(m_sceneRoot.GetProperties().GetItem(L"XSIManRibBox"));
	Parameter RIBBox = prop.GetParameters().GetItem(L"RIBBox");
	CString ribBox = RIBBox.GetValue();

	Parameter RIBBoxReplace = prop.GetParameters().GetItem(L"RIBBoxReplace");
	bool ribBoxReplace = RIBBoxReplace.GetValue();

	// substitute global vars
	ribBox = subGlobalVars(ribBox);

	long ribBoxPass = 277;
	Parameter RIBBoxPass = prop.GetParameters().GetItem(L"RIBBoxPass");
	ribBoxPass = RIBBoxPass.GetValue();

	if (ribBox != L"") {
		bool addRibBox = false;
			
		// check if RIBBox should be in this pass (1-beauty; 4-shadow; 16-environment; 256-photon)
		if ((job->jobType == BeautyJob) && (ribBoxPass == 1 || ribBoxPass == 5 || ribBoxPass == 17 || ribBoxPass == 21 || ribBoxPass == 257 || ribBoxPass == 261 || ribBoxPass == 273 || ribBoxPass == 277)) addRibBox = true;
		if ((job->jobType == ShadowJob) && (ribBoxPass == 4 || ribBoxPass == 5 || ribBoxPass == 20 || ribBoxPass == 21 || ribBoxPass == 260 || ribBoxPass == 261 || ribBoxPass == 276 || ribBoxPass == 277)) addRibBox = true;
		if ((job->jobType == EnvmapJob) && (ribBoxPass == 16 || ribBoxPass == 17 || ribBoxPass == 20 || ribBoxPass == 21 || ribBoxPass == 272 || ribBoxPass == 273 || ribBoxPass == 276 || ribBoxPass == 277)) addRibBox = true;
		if ((job->jobType == PhotonMapJob) && (ribBoxPass == 256 || ribBoxPass == 257 || ribBoxPass == 260  || ribBoxPass == 261  || ribBoxPass == 272 || ribBoxPass == 273 || ribBoxPass == 276 || ribBoxPass == 277)) addRibBox = true;

		if (addRibBox) {
			ribBox = replaceSubString(ribBox, L"\\", L"/");
			char* ribbox = NULL;
			XSIW2A(&ribbox, ribBox.GetWideString());
			RiArchiveRecord(RI_COMMENT, "RIB Box\n%s", ribbox);
		}
	}

	// replace scene info with RIB box
	if (ribBoxReplace == true) return CStatus::OK;

	// geometry
	stat = exportGeometry();
	
	// is this an archive job?
	if (job->jobType != ArchiveJob) RiWorldEnd();

	return CStatus::OK;
}

CStatus xsiRibTranslator::frameEpilogue()
{
	RiFrameEnd();

	return CStatus::OK;
}

CStatus xsiRibTranslator::exportLights()
{
	vector<xsiRibNode>::iterator iter;

	// lights
	if ((job->jobType != ShadowJob) && (job->jobType != ArchiveJob)) {
		for(iter=nodeData.nodeMap.begin(); iter!=nodeData.nodeMap.end(); ++iter) {
			xsiRibNode ribNode = (*iter);
			nodeStruct node = ribNode.get();

			if (node.objectType == LightObject) {
				SceneItem sceneItem = node.sceneItem;

				//  is light visible?
				if(!node.renderVisibility)
					continue;

				X3DObject x3dobject(sceneItem);
				xsiRibLightData lightData(x3dobject, g_CurrentFrame);
				CString objName = node.fullName;

				// find shadow job that corresponds to this light
				multimap<JobType, jobStruct*>::iterator jter;
				for (jter=jobList.begin(); jter!=jobList.end(); ++jter) {
					if ((*jter).first == ShadowJob) {
						jobStruct* shadowJob = (*jter).second;	
						if (shadowJob->jobType == ShadowJob) {
							if(shadowJob->name == objName) { 
								CString shadowName;
								shadowName = constructFileName(false, shadowJob);

								// we seem to need to replace back slashes with forward slashes?
								CString nuShadowName = replaceSubString(shadowName, L"\\", L"/");
								lightData.light.shadowName = nuShadowName;

								// are we ray-tracing shadows?
								if (shadowJob->rayTraceShadows)
									lightData.light.shadowName = L"raytrace";
							}
						}
					}
				}

				lightData.write();

				if (job->jobType == PhotonMapJob || job->jobType == BeautyJob) {
					char* photonmap = NULL;

					// find photon map job that corresponds to this light
					multimap<JobType, jobStruct*>::iterator jter;
					for (jter=jobList.begin(); jter!=jobList.end(); ++jter) {
						if ((*jter).first == PhotonMapJob) {
							jobStruct* photonJob = (*jter).second;	
							if (photonJob->jobType == PhotonMapJob) {
								if(photonJob->name == objName) { 
									CString photonMapName;
									photonMapName = constructFileName(false, photonJob);

									// we seem to need to replace back slashes with forward slashes?
									CString nuPhotonMapName = replaceSubString(photonMapName, L"\\", L"/");
									XSIW2A(&photonmap, nuPhotonMapName.GetWideString());
									
									if (job->photonMapType == L"Caustic") RiAttribute("photon", "causticmap", &photonmap, RI_NULL);
									if (job->photonMapType == L"Global") RiAttribute("photon", "globalmap", &photonmap, RI_NULL);

									// get photon map estimator value
									CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManPhotonMap");
									if (propRef.IsValid()) {
										Property prop = propRef;
										Parameter Estimator = prop.GetParameters().GetItem(L"Estimator");
										long estimator = Estimator.GetValue();
										RiAttribute("photon", "estimator", &estimator, RI_NULL);
									}
								}
							}
						}
					}		
				}
				
				// set light handles
				char* lhandle = (char*)lightData.getLightHandle();
				wchar_t* lightHandle;
				XSIA2W(&lightHandle, lhandle);
				node.lightHandle = lightHandle;
				xsiRibNode nuRibNode;
				nuRibNode.put(node);
				(*iter) = nuRibNode;
			}
		}
	}

	return CStatus::OK;
}

CStatus xsiRibTranslator::exportCoordinateSystems()
{
	vector<xsiRibNode>::iterator iter;

	// coordinate systems (cameras included)
	if (job->jobType != ArchiveJob) {
		for(iter=nodeData.nodeMap.begin(); iter!=nodeData.nodeMap.end(); ++iter) {
			xsiRibNode ribNode = (*iter);
			nodeStruct node = ribNode.get();

			if (node.objectType == CameraObject) {
				SceneItem sceneItem = node.sceneItem;
				X3DObject x3dobject(sceneItem);

				RiAttributeBegin();
				char* namePtr = NULL;
				XSIW2A(&namePtr, node.fullName.GetWideString());
				RiAttribute("identifier", "name", &namePtr, RI_NULL);

				xsiRibCameraData cameraData(x3dobject, g_CurrentFrame);
				cameraData.write();
				
				RiAttributeEnd();
			}
			else if (node.objectType == NullObject) {
				SceneItem sceneItem = node.sceneItem;
				X3DObject x3dobject(sceneItem);

				RiAttributeBegin();
				char* namePtr = NULL;
				XSIW2A(&namePtr, node.fullName.GetWideString());
				RiAttribute("identifier", "name", &namePtr, RI_NULL);

				xsiRibNullData nullData(x3dobject, g_CurrentFrame);
				nullData.write();

				RiAttributeEnd();
			}
		}
	}

	return CStatus::OK;
}

CStatus xsiRibTranslator::insertFrontplaneGeom()
{
	if ((job->jobType != ShadowJob) && (job->jobType != ArchiveJob)) {
		RiArchiveRecord(RI_COMMENT, "frontplane geometry");
		RiAttributeBegin();
		RiResourceBegin();
		const char* namePtr = "frontplane";
		RiAttribute("identifier", "name", &namePtr, RI_NULL);
		RtToken camera = "camera";
		RiCoordSysTransform(camera);

		RiArchiveRecord(RI_VERBATIM, "Color [0 0 0]\n");
		RiArchiveRecord(RI_VERBATIM, "Opacity [0 0 0]\n");

		// get camera shaders
		addCameraShaders();

		float left, right, bottom, top, frameAspectRatio;
		frameAspectRatio = (float)job->width / (float)job->height;
		if (frameAspectRatio < 1.0) {
			left = -0.5f*frameAspectRatio*job->camera[0].nearClip;
			right = 0.5f*frameAspectRatio*job->camera[0].nearClip;
			bottom = -0.5f*job->camera[0].nearClip;
			top = 0.5f*job->camera[0].nearClip;
		}
		else {
			left = -0.5f*job->camera[0].nearClip;
			right = 0.5f*job->camera[0].nearClip;
			bottom = -0.5f*job->camera[0].nearClip/frameAspectRatio;
			top = 0.5f*job->camera[0].nearClip/frameAspectRatio;
		}

		float card[12] = {1.01f*left, 1.01f*top, 1.0001f*job->camera[0].nearClip, 1.01f*right, 1.01f*top, 1.0001f*job->camera[0].nearClip, 1.01f*left, 1.01f*bottom, 1.0001f*job->camera[0].nearClip, 1.01f*right, 1.01f*bottom, 1.0001f*job->camera[0].nearClip};
		RiPatch("bilinear", "P", &card, RI_NULL);

		RiResourceEnd();
		RiAttributeEnd();
	}

	return CStatus::OK;
}

		
CStatus xsiRibTranslator::addCameraShaders()
{	
	bool hasSurfaceShader  = false;
	bool hasDisplacementShader = false;
	bool hasVolumeShader = false;
	CString surfaceShaderName, displaceShaderName, volumeShaderName;
	RtColor surfaceColor = {1.0, 1.0, 1.0};
	RtColor surfaceOpacity = {1.0, 1.0, 1.0};
	RtFloat displacementBound = 0.0;

	Project proj = m_app.GetActiveProject();
	Scene activeScene = proj.GetActiveScene();
	X3DObject camobj = job->camera[0].x3dobject;
	CRef camref;
	camref.Set(g_CameraName);
	Camera xsicam(camref);
	CRefArray shaderArray = xsicam.GetShaders();
	for (long i=0; i<shaderArray.GetCount(); i++ ) {
		Shader shader = shaderArray[i];
		bool rmanShader = shader.HasRenderer(L"prman") || shader.HasRenderer(L"3delight") || shader.HasRenderer(L"Air");
		if (rmanShader) {
			CString renderer;
			if (shader.HasRenderer(L"prman")) renderer = L"prman";
			else if (shader.HasRenderer(L"3delight")) renderer = L"3delight";
			else if (shader.HasRenderer(L"Air")) renderer = L"Air";

			CValue shaderType = shader.GetRendererOption(renderer, L"Type");
			if (shaderType == L"material") {
				hasSurfaceShader = true;
				surfaceShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"displace") {
				hasDisplacementShader = true;
				displaceShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"volume") {
				hasVolumeShader = true;
				volumeShaderName = shader.GetCodePath(renderer);
			}
			
			//rmanShaderParams(shader, surfaceColor, surfaceOpacity, displacementBound);
		}
	}
		
	// update texture maps
	updateSceneTextureMaps(activeScene);

	// shaders
	if (hasSurfaceShader) {
		unsigned numTokens = ribTokensSurf.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
		char* surfaceName = NULL;
		XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
		RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
		ribTokensSurf.clear();
	}
	
	return CStatus::OK;
}

CStatus xsiRibTranslator::insertBackplaneGeom()
{
	if ((job->jobType != ShadowJob) && (job->jobType != ArchiveJob)) {
		RiArchiveRecord(RI_COMMENT, "backplane geometry");
		RiAttributeBegin();
		RiResourceBegin();
		const char* namePtr = "backplane";
		RiAttribute("identifier", "name", &namePtr, RI_NULL);
		RtToken camera = "camera";
		RiCoordSysTransform(camera);
		
		RiArchiveRecord(RI_VERBATIM, "Color [0 0 0]\n");
		RiArchiveRecord(RI_VERBATIM, "Opacity [0 0 0]\n");

		// get render pass shaders
		addPassShaders();

		float left, right, bottom, top, frameAspectRatio;
		frameAspectRatio = (float)job->width / (float)job->height;
		if (frameAspectRatio < 1.0) {
			left = -0.5f*frameAspectRatio*job->camera[0].farClip;
			right = 0.5f*frameAspectRatio*job->camera[0].farClip;
			bottom = -0.5f*job->camera[0].farClip;
			top = 0.5f*job->camera[0].farClip;
		}
		else {
			left = -0.5f*job->camera[0].farClip;
			right = 0.5f*job->camera[0].farClip;
			bottom = -0.5f*job->camera[0].farClip/frameAspectRatio;
			top = 0.5f*job->camera[0].farClip/frameAspectRatio;
		}

		float card[12] = {left, top, 0.99f*job->camera[0].farClip, right, top, 0.99f*job->camera[0].farClip, left, bottom, 0.99f*job->camera[0].farClip, right, bottom, 0.99f*job->camera[0].farClip};
		RiPatch("bilinear", "P", &card, RI_NULL);

		RiResourceEnd();
		RiAttributeEnd();
	}

	return CStatus::OK;
}

CStatus xsiRibTranslator::addPassShaders()
{
	bool hasSurfaceShader  = false;
	bool hasDisplacementShader = false;
	bool hasVolumeShader = false;
	CString surfaceShaderName, displaceShaderName, volumeShaderName;
	RtColor surfaceColor = {1.0, 1.0, 1.0};
	RtColor surfaceOpacity = {1.0, 1.0, 1.0};
	RtFloat displacementBound = 0.0;

	Project proj = m_app.GetActiveProject();
	Scene activeScene = proj.GetActiveScene();
	Pass activePass = activeScene.GetActivePass();
	Parameter stack = activePass.GetNestedObjects().GetItem(L"Output Shader Stack");
	CRefArray shaderArray = stack.GetNestedObjects();
	for (long i=0; i<shaderArray.GetCount(); i++ ) {
		Shader shader = shaderArray[i];

		bool rmanShader = shader.HasRenderer(L"prman") || shader.HasRenderer(L"3delight") || shader.HasRenderer(L"Air");
		if (rmanShader) {
			CString renderer;
			if (shader.HasRenderer(L"prman")) renderer = L"prman";
			else if (shader.HasRenderer(L"3delight")) renderer = L"3delight";
			else if (shader.HasRenderer(L"Air")) renderer = L"Air";

			CValue shaderType = shader.GetRendererOption(renderer, L"Type");
			if (shaderType == L"material") {
				hasSurfaceShader = true;
				surfaceShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"displace") {
				hasDisplacementShader = true;
				displaceShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"volume") {
				hasVolumeShader = true;
				volumeShaderName = shader.GetCodePath(renderer);
			}
			
			rmanShaderParams(shader, surfaceColor, surfaceOpacity, displacementBound);
		}
	}
		
	// update texture maps
	//updateSceneTextureMaps(activeScene);

	// shaders
	if (hasSurfaceShader) {
		unsigned numTokens = ribTokensSurf.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
		char* surfaceName = NULL;
		XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
		RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
		ribTokensSurf.clear();
	}
	else {
		if (g_RenderTreeSupport == true) {
			if (getPassShader(activePass, surfaceShaderName) == CStatus::OK) {						
				unsigned numTokens = ribTokensSurf.size();
				RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
				RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
				assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
				char* surfaceName = NULL;
				XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
				RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
				ribTokensSurf.clear();
			}
		}
		else RiSurface("matte", RI_NULL);
	} 
	
	return CStatus::OK;
}

XSI::CStatus xsiRibTranslator::updateSceneTextureMaps(XSI::Scene& scene) 
{
	CStatus stat = CStatus::OK;
	CString sourceFile = L"";

	CRefArray clipArray = scene.GetImageClips();
	for (long k=0; k<clipArray.GetCount(); k++ ) {
		ImageClip2 imageclip = clipArray[k];	
			
		// get file name directly
		sourceFile = imageclip.GetFileName();
		sourceFile = replaceSubString(sourceFile, L"\\", L"/");
						
		// convert texture
		if (sourceFile != L"") { 
			CString destFile;
			CString fileName = stringAfterLastAppearance(sourceFile, L"/");
			if (g_Renderer == L"PRMan") destFile = g_TexPath + fileName + L".tex";
			else if (g_Renderer == L"3Delight") destFile = g_TexPath + fileName + L".tdl";
			else if (g_Renderer == L"Air") destFile = g_TexPath + fileName + L".tx";;

			// do we cleanup textures?
			if (g_Cleanup == 4 || g_Cleanup == 5 || g_Cleanup == 20 || g_Cleanup == 21) 
				cleanupList.push_back(destFile);

			unsigned int strlen = destFile.Length();
			destFile = replaceSubString(destFile, L"\\", L"/");
			char* destfile = NULL;
			XSIW2A(&destfile, destFile.GetWideString());

			// get file timestamps
			struct _stat buffer;
			int status;
			char* sourcefile = NULL;
			XSIW2A(&sourcefile, sourceFile.GetWideString());
			status = _stat(sourcefile, &buffer);
			time_t stime = buffer.st_mtime;
			status = _stat(destfile, &buffer);
			time_t dtime = buffer.st_mtime;

			if (access(destfile, 0) || stime > dtime) {
				CString texmake = textureCommand(sourceFile, destFile);
					preframeList.push_back(texmake);
			}
		}
	}

	return stat;
}

CStatus xsiRibTranslator::exportGeometry()
{
	vector<xsiRibNode>::iterator iter;
	bool lod = false; bool csgprim = false; bool csgdiff = false; bool csgint = false; bool csgunion = false;

	for(iter=nodeData.nodeMap.begin(); iter!=nodeData.nodeMap.end(); ++iter) {
		xsiRibNode ribNode = (*iter);
		nodeStruct node = ribNode.get();
		g_CurrentNode = node.fullName;
		g_CurrentNode = replaceSubString(g_CurrentNode, L".", L"_");

		//if ((node.objectType == NullObject) || (node.objectType == LightObject) || (node.objectType == CameraObject)) continue;
		if ((node.objectType == LightObject) || (node.objectType == CameraObject)) continue;

		SceneItem sceneItem = node.sceneItem;
		X3DObject x3dobject(sceneItem);

		// check if visible
		if (!node.renderVisibility || (!node.shadowVisibility && job->jobType==ShadowJob)) continue;
		
		// if this is null object check for Script box, otherwise don't write out
		if (node.objectType == NullObject) {
			Property prop(x3dobject.GetProperties().GetItem(L"XSIManScriptBox"));
			if (!prop.IsValid()) continue;
		}

		// if this is null object check for RIB box, otherwise don't write out
		if (node.objectType == NullObject) {
			Property prop(x3dobject.GetProperties().GetItem(L"XSIManRibBox"));
			if (!prop.IsValid()) continue;
		}

		// check for end of model object
		if ((node.objectType == EndModelObject) && (job->jobType != ArchiveJob)) {

			// check for csg and lod properties
			Property prop(x3dobject.GetProperties().GetItem(L"XSIManLODControls"));
			Parameter levelOfDetail = prop.GetParameters().GetItem(L"LevelOfDetail");
			lod = levelOfDetail.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGPrimitive");
			Parameter csgPrim = prop.GetParameters().GetItem(L"csgPrim");
			csgprim = csgPrim.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGDifference");
			Parameter csgDiff = prop.GetParameters().GetItem(L"csgDiff");
			csgdiff = csgDiff.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGIntersection");
			Parameter csgInt = prop.GetParameters().GetItem(L"csgInt");
			csgint = csgInt.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGUnion");
			Parameter csgUnion = prop.GetParameters().GetItem(L"csgUnion");
			csgunion = csgUnion.GetValue();
			
			if (csgprim || csgdiff || csgint || csgunion) {
				RiSolidEnd();
			}

			// level of detail
			if (lod) {
				RiAttributeEnd();
			}

			continue;
		}

		// check for detail range property
		RtFloat detailRange[4];
		CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManDetailRange");
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter start = prop.GetParameters().GetItem(L"DetailStart");
			detailRange[0] = start.GetValue();
			Parameter low = prop.GetParameters().GetItem(L"DetailLow");
			detailRange[1] = low.GetValue();
			Parameter high = prop.GetParameters().GetItem(L"DetailHigh");
			detailRange[2] = high.GetValue();
			Parameter end = prop.GetParameters().GetItem(L"DetailEnd");
			detailRange[3] = end.GetValue();

			if (job->jobType != ArchiveJob) {
				RiDetailRange(detailRange[0], detailRange[1], detailRange[2], detailRange[3]);
			}
		}

		// should this be a RIB archive?
		CString archiveType = L"Read";
		CString writeFrequency = L"Every Frame";
		bool ribArchive = false;
		bool reuseArchive = false;
		long archivePass = 277;
		long archiveContents = 5;
		float bboxPad = 0.0f;
		propRef = x3dobject.GetProperties().GetItem(L"XSIManArchive");
		if (propRef.IsValid()) {
			Property prop = propRef;
			ribArchive = true;
			Parameter reuse = prop.GetParameters().GetItem(L"ReuseArchive");
			reuseArchive = reuse.GetValue();
			Parameter archivetype = prop.GetParameters().GetItem(L"ArchiveType");
			archiveType = archivetype.GetValue();
			Parameter writefrequency = prop.GetParameters().GetItem(L"WriteFrequency");
			writeFrequency = writefrequency.GetValue();
			Parameter archivepass = prop.GetParameters().GetItem(L"ArchivePass");
			archivePass = archivepass.GetValue();
			Parameter archivecontents = prop.GetParameters().GetItem(L"ArchiveContents");
			archiveContents = archivecontents.GetValue();
			Parameter bboxpad = prop.GetParameters().GetItem(L"BBoxPad");
			bboxPad = bboxpad.GetValue();
		}

		// this geometry should be output as RIB archive
		char* archiveFileName = NULL;
		if (ribArchive && (job->jobType != ArchiveJob)) {
			
			// check if archive should be in this pass (1-beauty; 4-shadow; 16-environment; 256-photon)
			if ((job->jobType == BeautyJob) && !(archivePass == 1 || archivePass == 5 || archivePass == 17 || archivePass == 21 || archivePass == 257 || archivePass == 261 || archivePass == 273 || archivePass == 277)) continue;
			if ((job->jobType == ShadowJob) && !(archivePass == 4 || archivePass == 5 || archivePass == 20 || archivePass == 21 || archivePass == 260 || archivePass == 261 || archivePass == 276 || archivePass == 277)) continue;
			if ((job->jobType == EnvmapJob) && !(archivePass == 16 || archivePass == 17 || archivePass == 20 || archivePass == 21 || archivePass == 272 || archivePass == 273 || archivePass == 276 || archivePass == 277)) continue;
			if ((job->jobType == PhotonMapJob) && !(archivePass == 256 || archivePass == 257 || archivePass == 260  || archivePass == 261  || archivePass == 272 || archivePass == 273 || archivePass == 276 || archivePass == 277)) continue;
			
			char* objname = NULL;
			XSIW2A(&objname, node.fullName.GetWideString());

			// get archive file name
			wchar_t padframe[10];
			swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);
			
			// check write frequency
			CString fileName;
			if (writeFrequency == L"Every Frame")
				fileName = g_RibPath + job->ribFileName + L"." + node.fullName + L"." + padframe + L".rib";
			else if (writeFrequency == L"Once Per Job")
				fileName = g_RibPath + job->ribFileName + L"." + node.fullName + L".rib";

			// we seem to need to replace back slashes
			CString nuFileName = replaceSubString(fileName, L"\\", L"/");
			XSIW2A(&archiveFileName, nuFileName.GetWideString());

			if (archiveType == L"Read") {
				RiArchiveRecord(RI_COMMENT, "ReadArchive for object %s\nReadArchive \"%s\"", objname, archiveFileName); 
			}
			else if (archiveType == L"Delayed Read") {
				// get bounding box
				CValueArray in_args(1); CValue retval;
				in_args[0] = CValue(node.fullName);
				m_app.ExecuteCommand(L"GetBBox", in_args, retval);
				float bbox[6]; 
				swscanf(retval.GetAsText().GetWideString(), L"(%f, %f, %f, %f, %f, %f)", &bbox[0], &bbox[1], &bbox[2], 
					&bbox[3], &bbox[4], &bbox[5]);
				bbox[0] -= bboxPad; bbox[1] -= bboxPad; bbox[2] -= bboxPad;
				bbox[3] += bboxPad; bbox[4] += bboxPad; bbox[5] += bboxPad;
				RiArchiveRecord(RI_COMMENT, "DelayedReadArchive for object %s\nProcedural \"DelayedReadArchive\" [ \"%s\" ] [ %f %f %f %f %f %f ]", objname, archiveFileName, bbox[0], bbox[3], bbox[1], bbox[4], bbox[2], bbox[5]);
			}
			
			// skip this object
			continue;
		}

		// check if we are in archive job for this object
		if ((job->name != node.fullName) && (job->jobType == ArchiveJob)) continue;

		// check if we are in envmap job for this object
		if ((job->name == node.fullName) && (job->jobType == EnvmapJob)) continue;
		
		// check for model object 
		if (node.objectType == ModelObject) {

			// check for level of detail property
			RtBound detail = {-1, 1, -1, 1, -1, 1};

			Property prop(x3dobject.GetProperties().GetItem(L"XSIManLODControls"));
			Parameter levelOfDetail = prop.GetParameters().GetItem(L"LevelOfDetail");
			lod = levelOfDetail.GetValue();
			Parameter detailSizeMinX = prop.GetParameters().GetItem(L"DetailSizeMinX");
			detail[0] = detailSizeMinX.GetValue();
			Parameter detailSizeMaxX = prop.GetParameters().GetItem(L"DetailSizeMaxX");
			detail[1] = detailSizeMaxX.GetValue();
			Parameter detailSizeMinY = prop.GetParameters().GetItem(L"DetailSizeMinY");
			detail[2] = detailSizeMinY.GetValue();
			Parameter detailSizeMaxY = prop.GetParameters().GetItem(L"DetailSizeMaxY");
			detail[3] = detailSizeMaxY.GetValue();
			Parameter detailSizeMinZ = prop.GetParameters().GetItem(L"DetailSizeMinZ");
			detail[4] = detailSizeMinZ.GetValue();
			Parameter detailSizeMaxZ = prop.GetParameters().GetItem(L"DetailSizeMaxZ");
			detail[5] = detailSizeMaxZ.GetValue();
			
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGPrimitive");
			Parameter csgPrim = prop.GetParameters().GetItem(L"csgPrim");
			csgprim = csgPrim.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGDifference");
			Parameter csgDiff = prop.GetParameters().GetItem(L"csgDiff");
			csgdiff = csgDiff.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGIntersection");
			Parameter csgInt = prop.GetParameters().GetItem(L"csgInt");
			csgint = csgInt.GetValue();
				
			prop = x3dobject.GetProperties().GetItem(L"XSIManCSGUnion");
			Parameter csgUnion = prop.GetParameters().GetItem(L"csgUnion");
			csgunion = csgUnion.GetValue();

			// level of detail
			if (lod) {
				RiAttributeBegin();
				RiDetail(detail);
			}

			// csg
			if (csgprim) {
				RiSolidBegin(RI_PRIMITIVE);
			}
			else if (csgdiff) {
				RiSolidBegin(RI_DIFFERENCE);
			}
			else if (csgint) {
				RiSolidBegin(RI_INTERSECTION);
			}
			else if (csgunion) {
				RiSolidBegin(RI_UNION);
			}

			continue;
		}

		RiAttributeBegin();

		// identifier
		char* namePtr = NULL;
		XSIW2A(&namePtr, node.fullName.GetWideString());
		RiAttribute("identifier", "name", &namePtr, RI_NULL);

		// add group memberships
		CRefArray owners(x3dobject.GetOwners());
		CString groups;
		for (long j = 0; j < owners.GetCount(); ++j) {
			SIObject siobj(owners[j]);
			X3DObject x3dobj(owners[j]);
			if (siobj.IsA(siGroupID) && groups != L"") {
				groups += L"," + siobj.GetName();
			}
			else if (siobj.IsA(siGroupID)) {
				groups += siobj.GetName().GetWideString();
			}
		}
		if (groups != L"") {
			char* grouping = NULL;
			XSIW2A(&grouping, groups.GetWideString());
			RiAttribute("grouping", "membership", &grouping, RI_NULL);
		}
		
		// start resource block here
		if (node.objectType == SubdivisionObject) RiResourceBegin();

		// raytrace options
		if (g_EnableRayTracing) {
			RtInt camvisibility = node.primaryVisibility;
			RtInt raytrace = node.secondaryVisibility;
			RtString transmission = "opaque";
			if (!node.shadowVisibility) transmission = "transparent";
			RiAttribute("visibility", "int camera", &camvisibility, "int trace", &raytrace, "string transmission", &transmission, RI_NULL);
		}

		// find owners to determine if object is associated with a light
		vector<CString> inclusiveLights;
		vector<CString> exclusiveLights;
		for (long j = 0; j < owners.GetCount(); ++j) {
			SIObject siobj(owners[j]);
			if (siobj.IsA(siGroupID)) {
				SIObject objparent = siobj.GetParent();
				if (objparent.GetType() == L"light") {
					Light xsilight(objparent.GetParent());
					bool inclusive = xsilight.GetParameterValue(CString(L"SelectiveInclusive"));
					CString lgtname = xsilight.GetFullName();
					if (inclusive) inclusiveLights.push_back(lgtname);
					else exclusiveLights.push_back(lgtname);
				}
			}
		}

		if (job->jobType != ShadowJob && (archiveContents == 4 || archiveContents == 5)) {		
			vector<xsiRibNode>::iterator jter;
			for(jter=nodeData.nodeMap.begin(); jter!=nodeData.nodeMap.end(); ++jter) {
				xsiRibNode ribNode = (*jter);
				nodeStruct node = ribNode.get();
				if (node.objectType == LightObject) {
					bool illuminate = true;

					// check if this light is in lightSwitch list	
					vector<CString>::iterator kter;
					for(kter=lightSwitch.begin(); kter!=lightSwitch.end(); ++kter) {
						CString lgtname = (*kter);
						if (lgtname == node.fullName) illuminate = false;
					}

					for(kter=inclusiveLights.begin(); kter!=inclusiveLights.end(); ++kter) {
						CString lgtname = (*kter);
						if (lgtname == node.fullName) illuminate = true;
					} 
					
					for(kter=exclusiveLights.begin(); kter!=exclusiveLights.end(); ++kter) {
						CString lgtname = (*kter);
						if (lgtname == node.fullName) illuminate = false;
					}

					char* lightHandle = NULL;
					XSIW2A(&lightHandle, node.lightHandle.GetWideString());

					// archive files shouldn't have illuminate statements (light handles will get screwy)
					if (g_ExportAsArchive == false)
						RiIlluminate((RtLightHandle)lightHandle,  illuminate);
				}
			}
		}
		inclusiveLights.clear();
		exclusiveLights.clear();

		float sampleIncrement = job->camera[0].shutter / node.motionSamples;
		float sampleTime =  g_CurrentFrame;

		// get shutter timing
		if (g_ShutterTiming == L"OpenOnFrame") sampleTime = g_CurrentFrame;
		else if (g_ShutterTiming == L"CloseOnFrame") sampleTime = g_CurrentFrame - job->camera[0].shutter;
		else if (g_ShutterTiming == L"CenterOnFrame") sampleTime = g_CurrentFrame - 0.5f * job->camera[0].shutter;

		if (g_MotionBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
			RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
				sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
		}

		RtMatrix transformMatrix;
		getMatrix(x3dobject, transformMatrix, sampleTime);

		// if this is archive job then object should have ConcatTransform
		if ((job->jobType == ArchiveJob) || (g_ExportAsArchive == true))
			RiConcatTransform(transformMatrix);
		else 
			RiTransform(transformMatrix);

		if (g_MotionBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
			for (long i=0; i<node.motionSamples; i++) {
				getMatrix(x3dobject, transformMatrix, sampleTime + (i+1)*sampleIncrement);

				// if this is archive job then object should have ConcatTransform
				if ((job->jobType == ArchiveJob) || (g_ExportAsArchive == true))
					RiConcatTransform(transformMatrix);
				else 
					RiTransform(transformMatrix);
			}
			RiMotionEnd();
		} 
		
		// per-object attributes
		propRef = x3dobject.GetProperties().GetItem(L"XSIManMiscOptions");
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter shadingrate_flag = prop.GetParameters().GetItem(L"shadingrate_flag");
			if (shadingrate_flag.GetValue()) {
				Parameter shadingrate = prop.GetParameters().GetItem(L"shadingrate");
				float shadingRate = (float)shadingrate.GetValue();
				RiShadingRate(shadingRate);
			}
						
			Parameter eyesplits_flag = prop.GetParameters().GetItem(L"eyesplits_flag");
			if (eyesplits_flag.GetValue()) {
				Parameter eyesplits = prop.GetParameters().GetItem(L"eyesplits");
				long eyeSplits = (long)eyesplits.GetValue();
				RiAttribute("limits", "eyesplits", (RtPointer)&eyeSplits, RI_NULL);
			}
			
			Parameter nonbackfacecull = prop.GetParameters().GetItem(L"nonbackfacecull");
			if (nonbackfacecull.GetValue()) {
				long backfacecull = 0;
						RiAttribute("cull", "backfacing", (RtPointer)&backfacecull, RI_NULL);
			}

			Parameter nonhiddencull = prop.GetParameters().GetItem(L"nonhiddencull");
			if (nonhiddencull.GetValue()) {
				long hiddencull = 0;
				RiAttribute("cull", "hidden", (RtPointer)&hiddencull, RI_NULL);
			}

			Parameter nonrasterorient = prop.GetParameters().GetItem(L"nonrasterorient");
			if (nonrasterorient.GetValue()) {
				long rasterorient = 0;
				RiAttribute("dice", "rasterorient", (RtPointer)&rasterorient, RI_NULL);
			}
		}

		// cleanup
		ribTokensSurf.clear();
		ribTokensDisp.clear();
		ribTokensVol.clear();
				
		// check for trace set attached
		Property tprop(x3dobject.GetProperties().GetItem(L"XSIManTraceSet"));
		Parameter TraceSet = tprop.GetParameters().GetItem(L"TraceSet");
		CString traceSet = TraceSet.GetValue();
		if (traceSet != L"") {
			char* traceset = NULL;
			XSIW2A(&traceset, traceSet.GetWideString());
			unsigned int strlen = traceSet.Length();
			xsiRibTokens tokenDataString;
			tokenDataString.put("traceset", StringType, false, false, false, 0);
			tokenDataString.putDetailType(UniformDetail);
			tokenDataString.assignString(traceset, strlen);
			ribTokensSurf.push_back(tokenDataString);
		}

		// check if this should be matte object
		bool isMatteObject = false;
		propRef = node.sceneItem.GetProperties().GetItem(L"XSIManMatte");
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter matte = prop.GetParameters().GetItem(L"matte");
			isMatteObject = matte.GetValue();
		}
		if (isMatteObject && job->jobType == BeautyJob) RiMatte(1);

		// check if we want material information in RIB
		if(archiveContents == 1 || archiveContents == 5) {
			addDefaultMaterial(x3dobject, node);
			if (node.objectType == SubdivisionObject)
				addClusterMaterials(x3dobject, node);
		}

		// check for script box attached to scene_root
		Property sprop(node.sceneItem.GetProperties().GetItem(L"XSIManScriptBox"));\
		Parameter ScriptBox = sprop.GetParameters().GetItem(L"ScriptBox");
		CString scriptBox = ScriptBox.GetValue();
		
		long scriptBoxPass = 277;
		Parameter ScriptBoxPass = sprop.GetParameters().GetItem(L"ScriptBoxPass");
		scriptBoxPass = ScriptBoxPass.GetValue();
		
		if (scriptBox != L"") {
			bool addScriptBox = false;
			
			// check if RIBBox should be in this pass (1-beauty; 4-shadow; 16-environment; 256-photon)
			if ((job->jobType == BeautyJob) && (scriptBoxPass == 1 || scriptBoxPass == 5 || scriptBoxPass == 17 || scriptBoxPass == 21 || scriptBoxPass == 257 || scriptBoxPass == 261 || scriptBoxPass == 273 || scriptBoxPass == 277)) addScriptBox = true;
			if ((job->jobType == ShadowJob) && (scriptBoxPass == 4 || scriptBoxPass == 5 || scriptBoxPass == 20 || scriptBoxPass == 21 || scriptBoxPass == 260 || scriptBoxPass == 261 || scriptBoxPass == 276 || scriptBoxPass == 277)) addScriptBox = true;
			if ((job->jobType == EnvmapJob) && (scriptBoxPass == 16 || scriptBoxPass == 17 || scriptBoxPass == 20 || scriptBoxPass == 21 || scriptBoxPass == 272 || scriptBoxPass == 273 || scriptBoxPass == 276 || scriptBoxPass == 277)) addScriptBox = true;
			if ((job->jobType == PhotonMapJob) && (scriptBoxPass == 256 || scriptBoxPass == 257 || scriptBoxPass == 260  || scriptBoxPass == 261  || scriptBoxPass == 272 || scriptBoxPass == 273 || scriptBoxPass == 276 || scriptBoxPass == 277)) addScriptBox = true;

			if (addScriptBox) {
				CValueArray args;
				CValue retval;
				m_app.ExecuteCommand(scriptBox, args, retval);
				char* results = NULL;
				XSIW2A(&results, CString(retval).GetWideString());
				RiArchiveRecord(RI_VERBATIM, "%s\n", results);
			}
		}

		// check for RIB box
		Property prop(x3dobject.GetProperties().GetItem(L"XSIManRibBox"));
		Parameter RIBBox = prop.GetParameters().GetItem(L"RIBBox");
		CString ribBox = RIBBox.GetValue();
		
		Parameter RIBBoxReplace = prop.GetParameters().GetItem(L"RIBBoxReplace");
		bool ribBoxReplace = RIBBoxReplace.GetValue();

		// substitute global vars
		ribBox = subGlobalVars(ribBox);

		// replace object name as well	
		if (stringHasChar(ribBox, L"$OBJ")) {
			ribBox = replaceSubString(ribBox, L"$OBJ", g_CurrentNode);
		}

		long ribBoxPass = 277;
		Parameter RIBBoxPass = prop.GetParameters().GetItem(L"RIBBoxPass");
		ribBoxPass = RIBBoxPass.GetValue();

		if (ribBox != L"") {
			bool addRibBox = false;
			
			// check if RIBBox should be in this pass (1-beauty; 4-shadow; 16-environment; 256-photon)
			if ((job->jobType == BeautyJob) && (ribBoxPass == 1 || ribBoxPass == 5 || ribBoxPass == 17 || ribBoxPass == 21 || ribBoxPass == 257 || ribBoxPass == 261 || ribBoxPass == 273 || ribBoxPass == 277)) addRibBox = true;
			if ((job->jobType == ShadowJob) && (ribBoxPass == 4 || ribBoxPass == 5 || ribBoxPass == 20 || ribBoxPass == 21 || ribBoxPass == 260 || ribBoxPass == 261 || ribBoxPass == 276 || ribBoxPass == 277)) addRibBox = true;
			if ((job->jobType == EnvmapJob) && (ribBoxPass == 16 || ribBoxPass == 17 || ribBoxPass == 20 || ribBoxPass == 21 || ribBoxPass == 272 || ribBoxPass == 273 || ribBoxPass == 276 || ribBoxPass == 277)) addRibBox = true;
			if ((job->jobType == PhotonMapJob) && (ribBoxPass == 256 || ribBoxPass == 257 || ribBoxPass == 260  || ribBoxPass == 261  || ribBoxPass == 272 || ribBoxPass == 273 || ribBoxPass == 276 || ribBoxPass == 277)) addRibBox = true;

			if (addRibBox) {
				ribBox = replaceSubString(ribBox, L"\\", L"/");
				char* ribbox = NULL;
				XSIW2A(&ribbox, ribBox.GetWideString());
				RiArchiveRecord(RI_COMMENT, "RIB Box\n%s", ribbox);
			}
		}
		
		// replace scene info with RIB box
		if (ribBoxReplace == true) {
			RiAttributeEnd();
			continue;
		}

		// is this object is CSG primitive, it should be instanced
		prop = x3dobject.GetProperties().GetItem(L"XSIManCSGPrimitive");
		bool isCsgPrim = false;
		Parameter csgPrim = prop.GetParameters().GetItem(L"csgPrim");
		isCsgPrim = csgPrim.GetValue();

		// check if CSG block
		if (isCsgPrim) {
			RiSolidBegin(RI_PRIMITIVE);
		}

		// inline archive name
		char* archiveName = NULL;
		XSIW2A(&archiveName, node.fullName.GetWideString());

		// inline archive namespace (to prevent name clashes)
		char* instanceTag = "motion_sample";

		if (node.objectType == MeshObject) {
			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
				RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
					sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
			}
	
			if (node.handle[0] == NULL) {
				xsiRibMeshData meshData(x3dobject, sampleTime, node.clusterName);
				meshData.write();
			}
			else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_0\"\n", archiveName, instanceTag);

			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
				for (long i=0; i<node.motionSamples; i++) {
					if (node.handle[i+1] == NULL) {
						xsiRibMeshData meshData(x3dobject, sampleTime + (i+1)*sampleIncrement, node.clusterName);
						meshData.write();
					}
					else  
						RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d\"\n", archiveName, instanceTag, i+1);
				}

				RiMotionEnd();
			}
		}
		else if (node.objectType == SubdivisionObject) {
			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
				RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
					sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
			}

			if (node.handle[0] == NULL) {
				xsiRibSubdivisionData subdivData(x3dobject, sampleTime);
				subdivData.write();
			}
			else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_0\"\n", archiveName, instanceTag);

			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {

				for (long i=0; i<node.motionSamples; i++) {
					if (node.handle[i+1] == NULL) {
						xsiRibSubdivisionData subdivData(x3dobject, sampleTime + (i+1)*sampleIncrement);
						subdivData.write();
					}
					else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d\"\n", archiveName, instanceTag, i+1);
				}

				RiMotionEnd();
			}
		}
		else if (node.objectType == NurbsObject) {
			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
				RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
					sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
			}

			if (node.handle[0] == NULL) {
				xsiRibSurfaceData surfaceData(x3dobject, sampleTime);
				if (surfaceData.hasTrimCurves()) surfaceData.writeTrims();
				surfaceData.write();
			}
			else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_0\"\n", archiveName, instanceTag);

			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {

				for (long i=0; i<node.motionSamples; i++) {
					if (node.handle[i+1] == NULL) {
						xsiRibSurfaceData surfaceData(x3dobject, sampleTime + (i+1)*sampleIncrement);
						if (surfaceData.hasTrimCurves()) surfaceData.writeTrims();
						surfaceData.write();
					}
					else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d\"\n", archiveName, instanceTag, i+1);
				}

				RiMotionEnd();
			}
		}
		else if (node.objectType == CurveObject) {
			// this must be outside Motion blocks
			RiBasis(RiBSplineBasis, 1, RiBSplineBasis, 1);

			// limit curve objects to one curve (this is to deal with adding prim vars)
			Geometry curvegeom(x3dobject.GetActivePrimitive().GetGeometry(sampleTime));
			NurbsCurveList nurbscurvelist(curvegeom);
			CNurbsCurveDataArray nurbsCurveDataArray;
			nurbscurvelist.Get(siIGESNurbs, nurbsCurveDataArray);
			long ncurves = nurbsCurveDataArray.GetCount();
				
			for (long ci=0; ci<ncurves; ci++) {

				if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
					RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
						sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
				}

				if (node.handle[0] == NULL) {
					xsiRibCurveData curveData(x3dobject, sampleTime, ci);
					curveData.write();
				}
				else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d_0\"\n", archiveName, instanceTag, ci);

				if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {

					for (long i=0; i<node.motionSamples; i++) {
						if (node.handle[i+1] == NULL) {
							xsiRibCurveData curveData(x3dobject, sampleTime + (i+1)*sampleIncrement, ci);
							curveData.write();
						}
						else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d_%d\"\n", archiveName, instanceTag, ci, i+1);
					}
				
					RiMotionEnd();
				}
			}
		}
		else if (node.objectType == ParticlesObject) {		
			// determine particle type, motion blur for spheres and sprites must be handled inside xsiRibParticlesData
			ParticleType particleType = PointParticle;
			CRef propRef = node.sceneItem.GetProperties().GetItem(L"XSIManParticles");
			if (propRef.IsValid()) {
				Property prop = propRef;
				Parameter pType = prop.GetParameters().GetItem(L"ParticleType");
				if ((long)pType.GetValue() == SphereParticle) particleType = SphereParticle;
				else if ((long)pType.GetValue() == SpriteParticle) particleType = SpriteParticle;
			}

			if (particleType == SphereParticle || particleType == SpriteParticle) {
				if (node.handle[0] == NULL) {
					xsiRibParticlesData particlesData(x3dobject, sampleTime, job);
					particlesData.write(&node);
				}
				else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_0\"\n", archiveName, instanceTag);
			}
			else {
				if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
					RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
						sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
				}
			
				if (node.handle[0] == NULL) {
					xsiRibParticlesData particlesData(x3dobject, sampleTime, job);
					particlesData.write(&node);
				}
				else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_0\"\n", archiveName, instanceTag);

				if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {

					for (long i=0; i<node.motionSamples; i++) {
						if (node.handle[i+1] == NULL) {
							xsiRibParticlesData particlesData(x3dobject, sampleTime + (i+1)*sampleIncrement, job);
							particlesData.write(&node);
						}
						else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d\"\n", archiveName, instanceTag, i+1);
					}

					RiMotionEnd();
				}
			}
		}
		else if (node.objectType == HairObject) {	
			// this must be outside Motion blocks
			RiBasis(RiBSplineBasis, 1, RiBSplineBasis, 1);

			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {
				RiMotionBegin(node.motionSamples+1, sampleTime, sampleTime+sampleIncrement, sampleTime+2.f*sampleIncrement, 
					sampleTime+3.f*sampleIncrement, sampleTime+4.f*sampleIncrement, sampleTime+5.f*sampleIncrement);
			}

			if (node.handle[0] == NULL) {
				xsiRibHairData hairData(x3dobject, sampleTime);
				hairData.write();
			}
			else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_0\"\n", archiveName, instanceTag);

			if (g_DeformBlur && (job->jobType != ShadowJob || job->shadowBlur == true)) {

				for (long i=0; i<node.motionSamples; i++) {
					if (node.handle[i+1] == NULL) {
						xsiRibHairData hairData(x3dobject, sampleTime + (i+1)*sampleIncrement);
						hairData.write();
					}
					else  RiArchiveRecord(RI_VERBATIM, "ReadArchive \"%s_%s_%d\"\n", archiveName, instanceTag, i+1);
				}

				RiMotionEnd();
			}
		}

		// check if inside CSG block
		if (isCsgPrim) {
			RiSolidEnd();
		}

		// end resource block here
		if (node.objectType == SubdivisionObject) RiResourceEnd();

		RiAttributeEnd();
	}

	return CStatus::OK;
}

void xsiRibTranslator::getMatrix(X3DObject in_obj, RtMatrix transformMatrix, float cframe)
{
	MATH::CTransformation objTransform(in_obj.GetKinematics().GetGlobal().GetTransform(cframe));
	MATH::CMatrix4 matrix4(objTransform.GetMatrix4());
	for(int i = 0; i < 4 ; i++) {
		for(int j = 0; j < 4 ; j++) {
			transformMatrix[i][j] = (float)matrix4.GetValue(i,j);
		}
	}
}

CStatus xsiRibTranslator::appendBeautyJob(XSI::X3DObject& x3dobject)
{
	CString objName = x3dobject.GetFullName();
	jobStruct* beautyPass = new jobStruct;
	beautyPass->camera[0].x3dobject = x3dobject;
	beautyPass->name = objName;
	beautyPass->width = job->width;
	beautyPass->height = job->height;
	beautyPass->aspectRatio = job->aspectRatio;
	beautyPass->format = job->format;
	beautyPass->imageMode = job->imageMode;
	beautyPass->imageName = job->imageName;
	beautyPass->ribFileName = job->ribFileName;
	beautyPass->jobType = BeautyJob;
	beautyPass->writeFrequency = L"Every Frame";
	beautyPass->rewriteData = true;

	// determine file extension
	beautyPass->fileExt = getFileExt(beautyPass->format);

	// get shutter timing
	float sampleTime = g_CurrentFrame;
	if (g_ShutterTiming == L"OpenOnFrame") sampleTime = g_CurrentFrame;
	else if (g_ShutterTiming == L"CloseOnFrame") sampleTime = g_CurrentFrame - job->camera[0].shutter;
	else if (g_ShutterTiming == L"CenterOnFrame") sampleTime = g_CurrentFrame - 0.5f * job->camera[0].shutter;
	float sampleIncrement = job->camera[0].shutter / (g_MotionSamples - 1);

	for (signed i = 0; i < g_MotionSamples; i++) {
		xsiRibCameraData cam(x3dobject, sampleTime);
		sampleTime += sampleIncrement;
		cameraStruct camData = cam.get();
		beautyPass->camera[i].name = camData.name;
		beautyPass->camera[i].orthogonal = camData.orthogonal;
		beautyPass->camera[i].nearClip = camData.nearClip;
		beautyPass->camera[i].farClip = camData.farClip;
		beautyPass->camera[i].orthoWidth = camData.orthoWidth;
		beautyPass->camera[i].orthoHeight = camData.orthoHeight;
		beautyPass->camera[i].horizFOV = camData.horizFOV;
		beautyPass->camera[i].focalLength = camData.focalLength;
		beautyPass->camera[i].matrix4 = camData.matrix4;
	}

	beautyPass->camera[0].motionBlur = g_MotionBlur;
	//beautyPass->camera[0].orthogonal = false;
	beautyPass->camera[0].name = objName;

	jobList.insert(pair<JobType, jobStruct*>(BeautyJob, beautyPass));
		
	return CStatus::OK;
}

CStatus xsiRibTranslator::appendShadowJob(XSI::X3DObject& x3dobject)
{
	CString objName = x3dobject.GetFullName();
	jobStruct* shadowPass = new jobStruct;
	shadowPass->camera[0].x3dobject = x3dobject;
	shadowPass->name = objName;
	Light xsilight(x3dobject);
	float width = xsilight.GetParameterValue(CString(L"ShadowMapRes"));
	shadowPass->width = (int)width;
	shadowPass->height = shadowPass->width;
	shadowPass->aspectRatio = 1.;
	shadowPass->format = L"shadow";
	shadowPass->imageMode = L"z";
	shadowPass->fileExt = L".tex";
	shadowPass->imageName = job->imageName;
	shadowPass->ribFileName = job->ribFileName;
	bool isShadowJob = xsilight.GetParameterValue(CString(L"ShadowMap"));
	if (isShadowJob) shadowPass->jobType = ShadowJob;
	shadowPass->reuseData = false;
	shadowPass->shadowPixelSamples = 1;
	shadowPass->isPointLight = false;
	shadowPass->isEnvmap = false;
	shadowPass->writeFrequency = L"Every Frame";
	shadowPass->rewriteData = true;
	shadowPass->shadowBlur = false;
	shadowPass->rayTraceShadows = false;

	// get shadow map controls?
	CString shadowCamera = L"";
	CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManShadows");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter reuse = prop.GetParameters().GetItem(L"ReuseShadowMap");
		shadowPass->reuseData = reuse.GetValue();
		Parameter shadowcamera = prop.GetParameters().GetItem(L"ShadowCamera");
		shadowCamera = shadowcamera.GetValue();
		Parameter writefrequency = prop.GetParameters().GetItem(L"WriteFrequency");
		shadowPass->writeFrequency = writefrequency.GetValue();
		Parameter shadtype = prop.GetParameters().GetItem(L"ShadowMapType");
		shadowPass->shadowMapType = shadtype.GetValue();
		
		Parameter shadowBlur = prop.GetParameters().GetItem(L"ShadowMapBlur");
		shadowPass->shadowBlur = shadowBlur.GetValue();

		Parameter rayTraceShadows = prop.GetParameters().GetItem(L"RayTraceShadows");
		shadowPass->rayTraceShadows = rayTraceShadows.GetValue();

		if (shadowPass->shadowMapType  == L"deepshad" && g_Renderer != L"Air") {
			shadowPass->format = L"deepshad";
			shadowPass->imageMode = L"deepopacity";
			shadowPass->fileExt = L".shd";

			if (g_Renderer == L"3Delight") {
				shadowPass->format = L"dsm";
				shadowPass->imageMode = L"rgbaz";
				shadowPass->fileExt = L".dsm";
			}
		}
		Parameter interp = prop.GetParameters().GetItem(L"ShadowVolInterp");
		shadowPass->shadowVolInterp = interp.GetValue();
		Parameter samples = prop.GetParameters().GetItem(L"ShadowPixelSamples");
		shadowPass->shadowPixelSamples = (long)samples.GetValue();
	}

	shadowPass->camera[0].nearClip = 0.001f;
	shadowPass->camera[0].farClip = INF;
	shadowPass->camera[0].orthoWidth = 1.;
	shadowPass->camera[0].orthoHeight = 1.;
	shadowPass->camera[0].horizFOV = xsilight.GetParameterValue(CString(L"LightCone"));
	shadowPass->camera[0].motionBlur = false;

	short lightType = xsilight.GetParameterValue(CString(L"Type"));

	// distant light should have orthogonal camera
	if (lightType == DistantLight) shadowPass->camera[0].orthogonal = true;
	else shadowPass->camera[0].orthogonal = false;

	// point lights need FOV of 90 degrees?
	if (lightType == PointLight) {
		shadowPass->isPointLight = true;
		shadowPass->camera[0].horizFOV = 90;
	}

	// camera name
	shadowPass->camera[0].name = objName;

	// camera transform
	MATH::CTransformation lightTransform(x3dobject.GetKinematics().GetGlobal().GetTransform(g_CurrentFrame));
	MATH::CMatrix4 matrix4(lightTransform.GetMatrix4());
	shadowPass->camera[0].matrix4 = matrix4;

	// check if there is a shadow camera
	if (shadowCamera != L"") {
		CRef shadcamref;
		if (shadcamref.Set(shadowCamera) == CStatus::OK) {
			shadowPass->camera[0].name = shadowCamera;
			X3DObject shadcam(shadcamref);
			MATH::CTransformation lightTransform(shadcam.GetKinematics().GetGlobal().GetTransform(g_CurrentFrame));
			MATH::CMatrix4 matrix4(lightTransform.GetMatrix4());
			shadowPass->camera[0].matrix4 = matrix4;
			Camera camData(shadcam);
			shadowPass->camera[0].horizFOV = camData.GetParameterValue(CString(L"fov"), g_CurrentFrame);
			shadowPass->camera[0].nearClip = camData.GetParameterValue(CString(L"near"), g_CurrentFrame);
			shadowPass->camera[0].farClip = camData.GetParameterValue(CString(L"far"), g_CurrentFrame);
			shadowPass->camera[0].orthoWidth = camData.GetParameterValue(CString(L"orthoheight"), g_CurrentFrame);
			shadowPass->camera[0].orthoHeight = camData.GetParameterValue(CString(L"orthoheight"), g_CurrentFrame);
		}
	}

	jobList.insert(pair<JobType, jobStruct*>(ShadowJob, shadowPass));
		
	return CStatus::OK;
}

CStatus xsiRibTranslator::appendArchiveJob(XSI::X3DObject& x3dobject)
{
	CString objName = x3dobject.GetFullName();
	jobStruct* archivePass = new jobStruct;
	archivePass->camera[0].x3dobject = x3dobject;
	archivePass->name = objName;
	archivePass->width = job->width;
	archivePass->height = job->height;
	archivePass->aspectRatio = job->aspectRatio;
	archivePass->format = job->format;
	archivePass->imageMode = job->imageMode;
	archivePass->imageName = job->imageName;
	archivePass->ribFileName = job->ribFileName;
	archivePass->writeFrequency = L"Every Frame";
	archivePass->rewriteData = true;
	archivePass->camera[0].motionBlur = g_MotionBlur; 
	archivePass->reuseData = false;
	archivePass->jobType = ArchiveJob;

	// get archive controls?
	CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManArchive");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter reuse = prop.GetParameters().GetItem(L"ReuseArchive");
		archivePass->reuseData = reuse.GetValue();
		Parameter writefrequency = prop.GetParameters().GetItem(L"WriteFrequency");
		archivePass->writeFrequency = writefrequency.GetValue();
	}

	jobList.insert(pair<JobType, jobStruct*>(ArchiveJob, archivePass));
		
	return CStatus::OK;
}
 
CStatus xsiRibTranslator::appendEnvmapJob(XSI::X3DObject& x3dobject)
{
	CString objName = x3dobject.GetFullName();
	jobStruct* envmapPass = new jobStruct;
	envmapPass->name = objName;
	envmapPass->camera[0].x3dobject = x3dobject;
	envmapPass->width = 256;
	envmapPass->height = 256;
	envmapPass->aspectRatio = 1;
	envmapPass->format = L"tiff";
	envmapPass->imageMode = L"rgba";
	envmapPass->fileExt = L".tif";
	envmapPass->imageName = job->imageName;
	envmapPass->ribFileName = job->ribFileName;
	envmapPass->writeFrequency = L"Every Frame";
	envmapPass->rewriteData = true;
	envmapPass->reuseData = false;
	envmapPass->jobType = EnvmapJob;
	envmapPass->isEnvmap = true;	
	envmapPass->camera[0].nearClip = 0.001f;
	envmapPass->camera[0].farClip = INF;
	envmapPass->camera[0].horizFOV = 90;
	envmapPass->camera[0].motionBlur = false; 
	envmapPass->camera[0].name = objName;
	
	// camera transform
	MATH::CTransformation objectTransform(x3dobject.GetKinematics().GetGlobal().GetTransform(g_CurrentFrame));
	MATH::CMatrix4 matrix4(objectTransform.GetMatrix4());
	envmapPass->camera[0].matrix4 = matrix4;


	// get envmap controls?
	CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManEnvmap");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter reuse = prop.GetParameters().GetItem(L"ReuseEnvmap");
		envmapPass->reuseData = reuse.GetValue();
		Parameter writefrequency = prop.GetParameters().GetItem(L"WriteFrequency");
		envmapPass->writeFrequency = writefrequency.GetValue();
		Parameter envmapSize = prop.GetParameters().GetItem(L"EnvmapSize");
		envmapPass->width = (long)envmapSize.GetValue();
		envmapPass->height = (long)envmapSize.GetValue();
	}

	jobList.insert(pair<JobType, jobStruct*>(EnvmapJob, envmapPass));
		
	return CStatus::OK;
}

CStatus xsiRibTranslator::appendCausticMapJob(XSI::X3DObject& x3dobject)
{
	CString objName = x3dobject.GetFullName();
	jobStruct* photonPass = new jobStruct;
	photonPass->name = objName;
	photonPass->camera[0].x3dobject = x3dobject;
	photonPass->imageName = job->imageName;
	photonPass->ribFileName = job->ribFileName;
	photonPass->writeFrequency = L"Every Frame";
	photonPass->rewriteData = true;
	photonPass->reuseData = false;
	photonPass->jobType = PhotonMapJob;
	photonPass->camera[0].motionBlur = false; 
	photonPass->camera[0].name = objName;
	photonPass->isPhotonMap = true;

	Light xsilight(x3dobject);
	photonPass->photonMapType = L"Caustic";
	photonPass->photons = xsilight.GetParameterValue(CString(L"CausticPhotonNum"));
	photonPass->fileExt = L".cpm";
	
	// camera transform
	MATH::CTransformation objectTransform(x3dobject.GetKinematics().GetGlobal().GetTransform(g_CurrentFrame));
	MATH::CMatrix4 matrix4(objectTransform.GetMatrix4());
	photonPass->camera[0].matrix4 = matrix4;

	// get photon map controls?
	CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManPhotonMap");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter reuse = prop.GetParameters().GetItem(L"ReusePhotonMap");
		photonPass->reuseData = reuse.GetValue();
		Parameter writefrequency = prop.GetParameters().GetItem(L"WriteFrequency");
		photonPass->writeFrequency = writefrequency.GetValue();
	}

	jobList.insert(pair<JobType, jobStruct*>(PhotonMapJob, photonPass));
		
	return CStatus::OK;
}

CStatus xsiRibTranslator::appendGlobalMapJob(XSI::X3DObject& x3dobject)
{
	CString objName = x3dobject.GetFullName();
	jobStruct* photonPass = new jobStruct;
	photonPass->name = objName;
	photonPass->camera[0].x3dobject = x3dobject;
	photonPass->imageName = job->imageName;
	photonPass->ribFileName = job->ribFileName;
	photonPass->writeFrequency = L"Every Frame";
	photonPass->rewriteData = true;
	photonPass->reuseData = false;
	photonPass->jobType = PhotonMapJob;
	photonPass->camera[0].motionBlur = false; 
	photonPass->camera[0].name = objName;
	photonPass->isPhotonMap = true;	

	Light xsilight(x3dobject);
	photonPass->photonMapType = L"Global";
	photonPass->photons = xsilight.GetParameterValue(CString(L"GlobalPhotonsNum"));
	photonPass->fileExt = L".gpm";
	
	// camera transform
	MATH::CTransformation objectTransform(x3dobject.GetKinematics().GetGlobal().GetTransform(g_CurrentFrame));
	MATH::CMatrix4 matrix4(objectTransform.GetMatrix4());
	photonPass->camera[0].matrix4 = matrix4;

	// get photon map controls?
	CRef propRef = x3dobject.GetProperties().GetItem(L"XSIManPhotonMap");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter reuse = prop.GetParameters().GetItem(L"ReusePhotonMap");
		photonPass->reuseData = reuse.GetValue();
		Parameter writefrequency = prop.GetParameters().GetItem(L"WriteFrequency");
		photonPass->writeFrequency = writefrequency.GetValue();
	}

	jobList.insert(pair<JobType, jobStruct*>(PhotonMapJob, photonPass));
		
	return CStatus::OK;
}

CStatus xsiRibTranslator::appendInstanceJob()
{
	CString objName = g_CameraName;
	jobStruct* instancePass = new jobStruct;
	instancePass->name = objName;
	instancePass->ribFileName = job->ribFileName;
	instancePass->jobType = InstanceJob;
	instancePass->writeFrequency = L"Every Frame";
	instancePass->reuseData = g_ReuseInstanceGeometry;
	instancePass->rewriteData = true;
	instancePass->camera[0].name = objName;

	jobList.insert(pair<JobType, jobStruct*>(InstanceJob, instancePass));
		
	return CStatus::OK;
}

#if defined(LINUX)

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

CStatus xsiRibTranslator::execute(CString renderCommand)
{
	char* rendercommand = NULL;
	XSIW2A(&rendercommand, renderCommand.GetWideString());
	int returnCode = system((char*)rendercommand);
	
  	return CStatus::OK;
}

#endif 


#if defined(WIN32)

#include <windows.h>
#include <process.h>

CStatus xsiRibTranslator::execute(CString renderCommand)
{
	char* rendercommand = NULL;
	XSIW2A(&rendercommand, renderCommand.GetWideString());
	int returnCode = system((char*)rendercommand);
	
  	return CStatus::OK;
}

CStatus xsiRibTranslator::createProcess(CString renderCommand) 
{
	char* rendercommand = NULL;
	XSIW2A(&rendercommand, renderCommand.GetWideString());

	//_spawnlp(_P_NOWAIT, rendercommand, rendercommand, NULL);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	GetStartupInfo(&si);

	CreateProcess(NULL, rendercommand,  // Name of app to launch
		NULL,		// Default process security attributes
		NULL,		// Default thread security attributes
		FALSE,		// Don't inherit handles from the parent
		0,			// Normal priority
		NULL,		// Use the same environment as the parent
		NULL,		// Launch in the current directory
		&si,		// Startup Information
		&pi);		// Process information stored upon return

	
  	return CStatus::OK;
}

#endif 

#if defined(LINUX)

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

CStatus xsiRibTranslator::makedir(CString path)
{
	char* args = NULL;
	char* command = NULL;
	int returnCode = -1;

	// master resource directory
	CString xmpath = path + L"/xsiman_" + g_Renderer;
	XSIW2A(&args, xmpath.GetWideString());
	CString mkdir = L"mkdir " + xmpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command);

	// rib files
	CString ribpath = xmpath + L"/rib";
	XSIW2A(&args, ribpath.GetWideString());
	CString mkdir = L"mkdir " + ribpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command); 
	if (g_RibPath == L"") g_RibPath = ribpath + L"/";
	else g_RibPath += L"/";
	
	// workspace
	CString jobpath = xmpath + L"/jobs";
	XSIW2A(&args, jobpath.GetWideString());
	CString mkdir = L"mkdir " + jobpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command); 
	if (g_JobPath == L"") g_JobPath = jobpath + L"/";
	else g_JobPath += L"/";

	// rendered pictures
	CString pixpath = xmpath + L"/rmanpix";
	XSIW2A(&args, pixpath.GetWideString());
	CString mkdir = L"mkdir " + pixpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command);
	if (g_PixPath == L"") g_PixPath = pixpath + L"/";
	else g_PixPath += L"/";

	// shaders
	CString shaderpath = xmpath + L"/rmanshader";
	XSIW2A(&args, shaderpath.GetWideString());
	CString mkdir = L"mkdir " + shaderpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command);
	if (g_ShaderPath == L"") g_ShaderPath = shaderpath + L"/";
	else g_ShaderPath += L"/";

	// texture maps
	CString texpath = xmpath + L"/rmantex";
	XSIW2A(&args, texpath.GetWideString());
	CString mkdir = L"mkdir " + texpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command);
	if (g_TexPath == L"") g_TexPath = texpath + L"/";
	else g_TexPath += L"/";

	//shadow maps
	CString shdpath = texpath + L"/shd";
	XSIW2A(&args, shdpath.GetWideString());
	CString mkdir = L"mkdir " + shdpath + L"&";
	XSIW2A(&command, mkdir.GetWideString());
	if (access(args, 0)) returnCode = system(command);
	if (g_ShdPath == L"") g_ShdPath = shdpath + L"/";
	else g_ShdPath += L"/";

	//return (returnCode != -1);
	return CStatus::OK;
}
#endif 


#if defined(WIN32)

#include <windows.h>

CStatus xsiRibTranslator::makedir(CString path)
{
	char* args = NULL;

	// master resource directory
	CString xmpath = path + L"\\xsiman_" + g_Renderer;
	XSIW2A(&args, xmpath.GetWideString());
	if (access(args, 0)) mkdir(args);

	// rib files
	CString ribpath = xmpath + L"\\rib";
	XSIW2A(&args, ribpath.GetWideString());
	if (access(args, 0)) mkdir(args);
	if (g_RibPath == L"") g_RibPath = ribpath + L"\\";
	else g_RibPath += L"\\";

	// workspace
	CString jobpath = xmpath + L"\\jobs";
	XSIW2A(&args, jobpath.GetWideString());
	if (access(args, 0)) mkdir(args);
	if (g_JobPath == L"") g_JobPath = jobpath + L"\\";
	else g_JobPath += L"\\";

	// rendered pictures
	CString pixpath = xmpath + L"\\rmanpix";
	XSIW2A(&args, pixpath.GetWideString());
	if (access(args, 0)) mkdir(args);
	if (g_PixPath == L"") g_PixPath = pixpath + L"\\";
	else g_PixPath += L"\\";

	// shaders
	CString shaderpath = xmpath + L"\\rmanshader\\";
	XSIW2A(&args, shaderpath.GetWideString());
	if (access(args, 0)) mkdir(args);
	if (g_ShaderPath == L"") g_ShaderPath = shaderpath + L"\\";
	else g_ShaderPath += L"\\";

	// texture maps
	CString texpath = xmpath + L"\\rmantex";
	XSIW2A(&args, texpath.GetWideString());
	if (access(args, 0)) mkdir(args);
	if (g_TexPath == L"") g_TexPath = texpath + L"\\";
	else g_TexPath += L"\\";

	// shadow maps
	CString shdpath = texpath + L"\\shd";
	XSIW2A(&args, shdpath.GetWideString());
	if (access(args, 0)) mkdir(args);
	if (g_ShdPath == L"") g_ShdPath = shdpath + L"\\";
	else g_ShdPath += L"\\";

  	return CStatus::OK;
}
#endif 
 
XSI::CStatus xsiRibTranslator::builtInShader(XSI::Material& material, CString& shaderName)
{
	CStatus stat = CStatus::OK;

	// only look at surface shaders
	Parameter matparm = material.GetParameters().GetItem(L"surface");
	CRef paramSrc = matparm.GetSource();
	Shader shader(paramSrc);

	CString shaderType = L"";

	if (shader.GetProgID() == L"Softimage.material-phong.1" || shader.GetProgID() == L"Softimage.simple_phong.1") {
		shaderType = L"material";
		shaderName = L"xsimanPhong";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.material-lambert.1" || shader.GetProgID() == L"Softimage.simple_lambert.1") {
		shaderType = L"material";
		shaderName = L"xsimanLambert";
		stat = getShaderParams(shader);		
	}
	else if (shader.GetProgID() == L"Softimage.material-blinn.1" || shader.GetProgID() == L"Softimage.simple_blinn.1") {
		shaderType = L"material";
		shaderName = L"xsimanBlinn";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.material-cooktorr.1" || shader.GetProgID() == L"Softimage.simple_cooktorr.1") {
		shaderType = L"material";
		shaderName = L"xsimanCookTorrance";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.material-strauss.1") {
		shaderType = L"material";
		shaderName = L"xsimanStrauss";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.material-ward.1" || shader.GetProgID() == L"Softimage.simple_ward.1") {
		shaderType = L"material";
		shaderName = L"xsimanWard";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.material-constant.1") {
		shaderType = L"material";
		shaderName = L"xsimanConstant";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.pt_metashader.1") {
		shaderType = L"material";
		shaderName = L"xsimanParticle";
	}
	else if (shader.GetProgID() == L"Softimage.rh_renderer.1") {
		shaderType = L"material";
		shaderName = L"xsimanHair";
		stat = getShaderParams(shader);
	}
	else if (shader.GetProgID() == L"Softimage.sib_illum_shadowmaterial.1") {
		shaderType = L"material";
		shaderName = L"xsimanShadow";
		stat = getShaderParams(shader);
	}
	//else if (shader.GetProgID() == L"Softimage.FluidV2-mat.1") {
		//shaderType = L"material";
		//shaderName = L"xsimanFluid";
		//stat = getShaderParams(shader);
	//}
	//else if (shader.GetProgID() == L"Softimage.sib_bumpmap_gen.1") {
		//shaderType = L"material";
		//stat = getShaderParams(shader);
	//}

	if (shaderType != L"material") {
		stat = CStatus::Fail;
	}

	return stat;
}

XSI::CStatus xsiRibTranslator::getPassShader(XSI::Pass& pass, CString& shaderName)
{
	FILE* stream;
	CString shaderType;
	CStatus stat = CStatus::OK;
	vector<CString> shaderArgList;
	
	// only look at surface shaders
	Parameter stack = pass.GetNestedObjects().GetItem(L"Output Shader Stack");
	CRef paramSrc = stack.GetSource(); 
		
	// shader file name
	CString passName = pass.GetName();
	shaderName = L"srf_" +  passName;
	CString shaderFile = g_ShaderPath + L"/" + shaderName + L".sl"; 
	
	// insert in database
	shaderList.push_back(shaderName);
	Shader shader(paramSrc);

	// open file handle
	shaderFile = replaceSubString(shaderFile, L"\\", L"/");
	if ((stream = _wfopen(shaderFile.GetWideString(), L"w")) == NULL) {
		m_app.LogMessage(L"Could not create shader " + shaderName + L"!");
		return CStatus::Fail;
	}

	// write out shader
	shaderType = L"surface";

	// this may help speed things up
	shaderTemplates.reserve(1000);

	shaderTemplateItem stitem;
	stitem.subShader = L"";
	stitem.subShaderName = L"";
	stitem.param = L"";
	stitem.paramValue = L"";
	stitem.isAnimated = false;
	stitem.shader = shader.GetProgID();
	stitem.shaderName = shader.GetName();
	shaderTemplates.push_back(stitem);

	// recurse shader tree
	getShaderNodes(shader);

	if (g_Renderer == L"Air") {
		fwprintf(stream, CString(L"#include \"combined.h\"\n").GetWideString());
	}
	else {
		fwprintf(stream, CString(L"#include \"utility.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"conversion.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"imageprocessing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"texture.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"mixers.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"bump.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"switch.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"colorchannels.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"state.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"environment.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"illumination.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"hair.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"raytracing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"math.h\"\n").GetWideString());
	}

	CString newline = L"\nsurface " + shaderName + L"(\n\tDEF_GLOBAL_PARAMS\n\tDEF_OUTPUT_VARYING_PARAMS\n";
	fwprintf(stream, newline.GetWideString());

	// add animated parameters
	vector<shaderTemplateItem>::reverse_iterator iter;
	iter = shaderTemplates.rbegin();		
	long paramIndex = 1;
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			if (stitem.isAnimated == true) {
				CString paramName = CString(sfunc1) + L"_" + stitem.param;

				// add definition for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argval = stitem.paramValue;
						if (CString((*jter).second) == L"string") {
							argval = L"\"" + stitem.paramValue + L"\"";
						}
						paramName = shaderName + L"__srfparam" + CValue(paramIndex).GetAsText();
						paramIndex += 1;
						CString newline = L"\t" + CString((*jter).second) + L" " + paramName + L" = " + argval + L";\n";				
						fwprintf(stream, newline.GetWideString());
					}
				}
			}
		}
	}
		
	// add varying parameters for texture UVs
	vector<CString> varyingParams;
	iter = shaderTemplates.rbegin();	
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			vector<CString>::reverse_iterator lter;
			lter = varyingParams.rbegin();	
			bool addParam = true;
			while (lter != varyingParams.rend()) {
				if ((*lter) == stitem.paramValue) {
					addParam = false;
					break;
				}
				lter++;
			}

			if (stitem.param == L"tspace_id" && stitem.paramValue != L"" && stitem.paramValue != L"*" && addParam == true) {
				varyingParams.push_back(stitem.paramValue);
				// add definition for texture coordinate parameter
				CString paramName = L"u_" + stitem.paramValue;
				CString newline = L"\tvarying float " + paramName + L" = 0;\n";	
				fwprintf(stream, newline.GetWideString());
				paramName = L"v_" + stitem.paramValue;
				newline = L"\tvarying float " + paramName + L" = 0;\n";				
				fwprintf(stream, newline.GetWideString());
			}
		}
	}

	fwprintf(stream, L")\n");
	fwprintf(stream, L"{\n");
			
	// output parameter definitions
	fwprintf(stream, CString(L"\tcolor diffusecolor = 0;\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor ambientcolor = 0;\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor specularcolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor radiancecolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor transparencycolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor refrcolor = 0;\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor reflcolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor occlusioncolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor indirectcolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor causticcolor = 0;\n\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor shadowcolor = 0;\n").GetWideString());

	// parameter definitions	
	map<const wchar_t*, const wchar_t*> paramDefs;
	iter = shaderTemplates.rbegin();
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;				
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderArgDefs.begin(); jter!=shaderArgDefs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
		}
	}

	map<const wchar_t*, const wchar_t*>::reverse_iterator kter;
	kter = paramDefs.rbegin();
	while (kter != paramDefs.rend()) {
		CString newline = L"\t" + CString((*kter).second) + L"\n";
		fwprintf(stream, newline.GetWideString());
		kter++;
	}
	paramDefs.clear();
			
	paramIndex = 1;
	iter = shaderTemplates.rbegin();
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L""; wchar_t* sfunc3 = L""; wchar_t* sfunc4 = L"";

		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc3 = (*jter).second;
		}
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
			if (CString(stitem.shader.GetWideString()) == CString((*jter).first)) 
				sfunc4 = (*jter).second;
		}

		if (iter == shaderTemplates.rend()) {
			// reset unique parameter names to global names
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc1).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc1)) {
					// check for global environment parameters
					if (globalEnvironmentParam(CString((*jter).first)) == true) {
						newline = L"\t" + CString((*jter).first) + L" = global_environment_global;\n";
					}
					else {
						newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.shaderName + L";\n";
					}
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString newline = L"\tCi = " + CString(sfunc1) + L"(" + CString(sfunc4) + L");\n";
			fwprintf(stream, newline.GetWideString());
		}
		else if (CString(sfunc1) != L"" && CString(sfunc2) != L"") {
			if (CString(sfunc3) == L"image_clip") {
				fwprintf(stream, CString(L"\timage_clip_invert_s = invert_s;\n").GetWideString());
				fwprintf(stream, CString(L"\timage_clip_invert_t = invert_t;\n").GetWideString());
			}

			// reset unique parameter names to global names 
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc3).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc3)) {
					// check for global environment parameters
					if (globalEnvironmentParam(CString((*jter).first)) == true) {	
						newline = L"\t" + CString((*jter).first) + L" = global_environment_global;\n";
					}
					else {
						newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.subShaderName + L";\n";
					}
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString paramName = CString(sfunc1) + L"_" + stitem.param;
			CString argType = getArgType(paramName);

			// check if this parameter has already been defined
			vector<CString>::iterator lter;
			bool alreadyDefined = false;
			for (lter=shaderArgList.begin(); lter!=shaderArgList.end(); lter++) {
				if (*lter == paramName + L"_" + stitem.shaderName) {
					alreadyDefined = true;
					break;
				}
			}
		
			if (alreadyDefined == false) {
				shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

				CString newline;
				newline = L"\t" + argType + L" " + paramName + L"_" + stitem.shaderName + L" = " + CString(sfunc3) + L"(" + CString(sfunc2) + L");\n";
				fwprintf(stream, newline.GetWideString());
			}
		}
		else if (CString(sfunc1) != L"") {
			CString newline;
			CString paramName = CString(sfunc1) + L"_" + stitem.param;

			if (stitem.isAnimated == true) {
				// add rib tokens for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argType = (*jter).second;
						char* pname = NULL;
						CString nuName = shaderName + L"__srfparam" + CValue(paramIndex).GetAsText();

						// check if this parameter has already been defined
						vector<CString>::iterator kter;
						bool alreadyDefined = false;
						for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
							if (*kter == paramName + L"_" + stitem.shaderName) {
								alreadyDefined = true;
								break;
							}
						}
		
						if (alreadyDefined == false) {
							shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

							// set unique parameter names
							paramName = argType + L" " + paramName + L"_" + stitem.shaderName;
							newline = L"\t" + paramName + L" = " + nuName + L";\n";
							fwprintf(stream, newline.GetWideString());
						}

						paramIndex += 1;
						XSIW2A(&pname, nuName.GetWideString());
						char* pvalue = NULL;
						XSIW2A(&pvalue, stitem.paramValue.GetWideString());
						unsigned int strlen = stitem.paramValue.Length();

						if (argType == L"float") {		
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put(pname, FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, (float)atof(pvalue));
							ribTokensSurf.push_back(tokenDataFloat);
						}
						else if (argType == L"color") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"color (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, ColorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensSurf.push_back(tokenDataFloat);									
						}
						else if (argType == L"vector") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"vector (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, VectorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensSurf.push_back(tokenDataFloat);
						}
						else if (argType == L"string") {
							xsiRibTokens tokenDataString;
							tokenDataString.put(pname, StringType, false, false, false, 0);
							tokenDataString.putDetailType(UniformDetail);
							tokenDataString.assignString(pvalue, strlen);
							ribTokensSurf.push_back(tokenDataString);
						}

						// add shader params for shader param list as well
						shaderParamStruct spstruct;
						spstruct.shaderName = shaderName;
						spstruct.argType = argType;
						spstruct.paramName = nuName;
						spstruct.paramValue = stitem.paramValue;
						//m_app.LogMessage(L"DEBUG: " + spstruct.shaderName + L" " + spstruct.argType + L" " + spstruct.paramName + L" " + spstruct.paramValue);
						shaderParamList.push_back(spstruct);
					}
				}
			}
			else {
				// set unique parameter names
				CString argType = getArgType(paramName);

				// check if this parameter has already been defined
				vector<CString>::iterator kter;
				bool alreadyDefined = false;
				for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
					if (*kter == paramName + L"_" + stitem.shaderName) {
						alreadyDefined = true;
						break;
					}
				}
		
				if (alreadyDefined == false) {
					shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

					// do not reset global environment color parameter
					CString fullName;
					if (paramName == L"global_environment_global") 
						fullName = argType + L" " + paramName;
					else 
						fullName = argType + L" " + paramName + L"_" + stitem.shaderName;

					if (argType == L"string") 
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
					else 
						newline = L"\t" + fullName + L" = " + stitem.paramValue + L";\n";

					if (paramName == L"txt2d_image_explicit_tspace_id" || paramName == L"txt2d_scalarimage_explicit_tspace_id") {
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
						if (stitem.paramValue == L"" || stitem.paramValue == L"*") {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
						else if (stitem.paramValue != L"") {
							newline += L"\t" + paramName + L"_u = u_" + stitem.paramValue + L";\n";
							newline += L"\t" + paramName + L"_v = v_" + stitem.paramValue + L";\n";
						}
						else {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
					}

					fwprintf(stream, newline.GetWideString());
				}
			}
		}
	}
			
	// set output varying parameters
	fwprintf(stream, CString(L"\n\t__diffusecolor = diffusecolor;\n").GetWideString());
	fwprintf(stream, CString(L"\t__ambientcolor = ambientcolor;\n").GetWideString());
	fwprintf(stream, CString(L"\t__specularcolor = specularcolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__radiancecolor = radiancecolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__transparencycolor = transparencycolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__refrcolor = refrcolor;\n").GetWideString());
	fwprintf(stream, CString(L"\t__reflcolor = reflcolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__occlusioncolor = occlusioncolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__indirectcolor = indirectcolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__causticcolor = causticcolor;\n\n").GetWideString());
	fwprintf(stream, CString(L"\t__shadowcolor = shadowcolor;\n\n").GetWideString());

	fwprintf(stream, L"}\n");

	if (shaderType != L"surface") {
		stat = CStatus::Fail;
	}

	// cleanup
	fclose(stream);
	shaderTemplates.clear();
	shaderArgList.clear();	
	
	// compile shader
	if (shaderName != L"" && shaderName != L"xsimanParticle" && prejobState == true) 
		compileShader(shaderName);
		
	return stat;
}

XSI::CStatus xsiRibTranslator::getSurfaceShader(XSI::Material& material, CString nodeName, CString& shaderName)
{
	FILE* stream;
	CString shaderType;
	CStatus stat = CStatus::OK;
	vector<CString> shaderArgList;
	
	// only look at surface shaders
	Parameter matparm = material.GetParameters().GetItem(L"surface");
	CRef paramSrc = matparm.GetSource();

	if (paramSrc.IsValid() == false) {
		// also try to see if there is a "material" attached
		matparm = material.GetParameters().GetItem(L"material");
		paramSrc = matparm.GetSource();
		if (paramSrc.IsValid() == false)
			return CStatus::Fail;
	}

	// shader file name
	CString materialName = material.GetName();
	shaderName = L"srf_" + materialName;
	CString shaderFile = g_ShaderPath + L"/" + shaderName + L".sl"; 

	// check if this shader has already been created 
	//if (shaderExists(shaderName) == true) {
		//addShaderParamToTokenList(shaderName, L"surface");
		//return stat;
	//}
		
	// insert in database
	shaderList.push_back(shaderName);
	Shader shader(paramSrc);

	// open file handle
	shaderFile = replaceSubString(shaderFile, L"\\", L"/");
	if ((stream = _wfopen(shaderFile.GetWideString(), L"w")) == NULL) {
		m_app.LogMessage(L"Could not create shader " + shaderName + L"!");
		return CStatus::Fail;
	}

	// write out shader
	shaderType = L"surface";
		
	// also get data from environment shader
	matparm = material.GetParameters().GetItem(L"environment");
	CRef paramSrc2 = matparm.GetSource();
	Shader envShader;
	if (paramSrc2.IsValid()) envShader = paramSrc2; 

	// particle shader needs special attention
	if (shader.GetProgID() == L"Softimage.pt_metashader.1") {
		shaderName = L"xsimanParticle";
		return stat;
	}

	// this may help speed things up
	shaderTemplates.reserve(1000);

	shaderTemplateItem stitem;
	stitem.subShader = L"";
	stitem.subShaderName = L"";
	stitem.param = L"";
	stitem.paramValue = L"";
	stitem.isAnimated = false;
	stitem.shader = shader.GetProgID();
	stitem.shaderName = shader.GetName();
	shaderTemplates.push_back(stitem);

	// recurse shader tree
	getShaderNodes(shader);
				
	// add any nodes from environment shader
	if (paramSrc2.IsValid()) {
		stitem.shader = L"global";
		stitem.shaderName = L"global";
		stitem.subShader = envShader.GetProgID();
		stitem.subShaderName = envShader.GetName();
		stitem.param = L"environment";
		if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);
		getShaderNodes(envShader);
	}
   	else {
		stitem.shader = L"global";
		stitem.shaderName = L"global";
		stitem.param = L"environment";
		stitem.paramValue = L"color (0,0,0)";
		if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);
	}

	if (g_Renderer == L"Air") {
		fwprintf(stream, CString(L"#include \"combined.h\"\n").GetWideString());
	}
	else {
		fwprintf(stream, CString(L"#include \"utility.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"conversion.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"imageprocessing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"texture.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"mixers.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"bump.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"switch.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"colorchannels.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"state.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"environment.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"illumination.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"hair.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"raytracing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"math.h\"\n").GetWideString());
	}

	CString newline = L"\nsurface " + shaderName + L"(\n\tDEF_GLOBAL_PARAMS\n\tDEF_OUTPUT_VARYING_PARAMS\n";
	fwprintf(stream, newline.GetWideString());

	// add animated parameters
	vector<shaderTemplateItem>::reverse_iterator iter;
	iter = shaderTemplates.rbegin();		
	long paramIndex = 1;
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			if (stitem.isAnimated == true) {
				CString paramName = CString(sfunc1) + L"_" + stitem.param;

				// add definition for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argval = stitem.paramValue;
						if (CString((*jter).second) == L"string") {
							argval = L"\"" + stitem.paramValue + L"\"";
						}
						paramName = shaderName + L"__srfparam" + CValue(paramIndex).GetAsText();
						paramIndex += 1;
						CString newline = L"\t" + CString((*jter).second) + L" " + paramName + L" = " + argval + L";\n";				
						fwprintf(stream, newline.GetWideString());
					}
				}
			}
		}
	}
		
	// add varying parameters for texture UVs
	vector<CString> varyingParams;
	iter = shaderTemplates.rbegin();	
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			vector<CString>::reverse_iterator lter;
			lter = varyingParams.rbegin();	
			bool addParam = true;
			while (lter != varyingParams.rend()) {
				if ((*lter) == stitem.paramValue) {
					addParam = false;
					break;
				}
				lter++;
			}

			if (stitem.param == L"tspace_id" && stitem.paramValue != L"" && stitem.paramValue != L"*" && addParam == true) {
				varyingParams.push_back(stitem.paramValue);
				// add definition for texture coordinate parameter
				CString paramName = L"u_" + stitem.paramValue;
				CString newline = L"\tvarying float " + paramName + L" = 0;\n";	
				fwprintf(stream, newline.GetWideString());
				paramName = L"v_" + stitem.paramValue;
				newline = L"\tvarying float " + paramName + L" = 0;\n";				
				fwprintf(stream, newline.GetWideString());
			}
		}
	}

	fwprintf(stream, L")\n");
	fwprintf(stream, L"{\n");
			
	// output parameter definitions
	fwprintf(stream, CString(L"\tcolor diffusecolor = 0;\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor ambientcolor = 0;\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor specularcolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor radiancecolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor transparencycolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor refrcolor = 0;\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor reflcolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor occlusioncolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor indirectcolor = 0;\n").GetWideString()); 
	fwprintf(stream, CString(L"\tcolor causticcolor = 0;\n\n").GetWideString());
	fwprintf(stream, CString(L"\tcolor shadowcolor = 0;\n").GetWideString());

	// parameter definitions	
	map<const wchar_t*, const wchar_t*> paramDefs;
	iter = shaderTemplates.rbegin();
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;				
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderArgDefs.begin(); jter!=shaderArgDefs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
		}
	}

	map<const wchar_t*, const wchar_t*>::reverse_iterator kter;
	kter = paramDefs.rbegin();
	while (kter != paramDefs.rend()) {
		CString newline = L"\t" + CString((*kter).second) + L"\n";
		fwprintf(stream, newline.GetWideString());
		kter++;
	}
	paramDefs.clear();
			
	paramIndex = 1;
	iter = shaderTemplates.rbegin();
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L""; wchar_t* sfunc3 = L""; wchar_t* sfunc4 = L"";

		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc3 = (*jter).second;
		}
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
			if (CString(stitem.shader.GetWideString()) == CString((*jter).first)) 
				sfunc4 = (*jter).second;
		}

		if (iter == shaderTemplates.rend()) {
			// reset unique parameter names to global names
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc1).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc1)) {
					// check for global environment parameters
					if (globalEnvironmentParam(CString((*jter).first)) == true) {
						newline = L"\t" + CString((*jter).first) + L" = global_environment_global;\n";
					}
					else {
						newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.shaderName + L";\n";
					}
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString newline = L"\tCi = " + CString(sfunc1) + L"(" + CString(sfunc4) + L");\n";
			fwprintf(stream, newline.GetWideString());
		}
		else if (CString(sfunc1) != L"" && CString(sfunc2) != L"") {
			if (CString(sfunc3) == L"image_clip") {
				fwprintf(stream, CString(L"\timage_clip_invert_s = invert_s;\n").GetWideString());
				fwprintf(stream, CString(L"\timage_clip_invert_t = invert_t;\n").GetWideString());
			}

			// reset unique parameter names to global names 
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc3).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc3)) {
					// check for global environment parameters
					if (globalEnvironmentParam(CString((*jter).first)) == true) {	
						newline = L"\t" + CString((*jter).first) + L" = global_environment_global;\n";
					}
					else {
						newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.subShaderName + L";\n";
					}
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString paramName = CString(sfunc1) + L"_" + stitem.param;
			CString argType = getArgType(paramName);

			// check if this parameter has already been defined
			vector<CString>::iterator lter;
			bool alreadyDefined = false;
			for (lter=shaderArgList.begin(); lter!=shaderArgList.end(); lter++) {
				if (*lter == paramName + L"_" + stitem.shaderName) {
					alreadyDefined = true;
					break;
				}
			}
		
			if (alreadyDefined == false) {
				shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

				CString newline;
				newline = L"\t" + argType + L" " + paramName + L"_" + stitem.shaderName + L" = " + CString(sfunc3) + L"(" + CString(sfunc2) + L");\n";
				fwprintf(stream, newline.GetWideString());
			}
		}
		else if (CString(sfunc1) != L"") {
			CString newline;
			CString paramName = CString(sfunc1) + L"_" + stitem.param;

			if (stitem.isAnimated == true) {
				// add rib tokens for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argType = (*jter).second;
						char* pname = NULL;
						CString nuName = shaderName + L"__srfparam" + CValue(paramIndex).GetAsText();

						// check if this parameter has already been defined
						vector<CString>::iterator kter;
						bool alreadyDefined = false;
						for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
							if (*kter == paramName + L"_" + stitem.shaderName) {
								alreadyDefined = true;
								break;
							}
						}
		
						if (alreadyDefined == false) {
							shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

							// set unique parameter names
							paramName = argType + L" " + paramName + L"_" + stitem.shaderName;
							newline = L"\t" + paramName + L" = " + nuName + L";\n";
							fwprintf(stream, newline.GetWideString());
						}

						paramIndex += 1;
						XSIW2A(&pname, nuName.GetWideString());
						char* pvalue = NULL;
						XSIW2A(&pvalue, stitem.paramValue.GetWideString());
						unsigned int strlen = stitem.paramValue.Length();

						if (argType == L"float") {		
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put(pname, FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, (float)atof(pvalue));
							ribTokensSurf.push_back(tokenDataFloat);
						}
						else if (argType == L"color") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"color (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, ColorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensSurf.push_back(tokenDataFloat);									
						}
						else if (argType == L"vector") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"vector (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, VectorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensSurf.push_back(tokenDataFloat);
						}
						else if (argType == L"string") {
							xsiRibTokens tokenDataString;
							tokenDataString.put(pname, StringType, false, false, false, 0);
							tokenDataString.putDetailType(UniformDetail);
							tokenDataString.assignString(pvalue, strlen);
							ribTokensSurf.push_back(tokenDataString);
						}

						// add shader params for shader param list as well
						shaderParamStruct spstruct;
						spstruct.shaderName = shaderName;
						spstruct.argType = argType;
						spstruct.paramName = nuName;
						spstruct.paramValue = stitem.paramValue;
						//m_app.LogMessage(L"DEBUG: " + spstruct.shaderName + L" " + spstruct.argType + L" " + spstruct.paramName + L" " + spstruct.paramValue);
						shaderParamList.push_back(spstruct);
					}
				}
			}
			else {
				// set unique parameter names
				CString argType = getArgType(paramName);

				// check if this parameter has already been defined
				vector<CString>::iterator kter;
				bool alreadyDefined = false;
				for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
					if (*kter == paramName + L"_" + stitem.shaderName) {
						alreadyDefined = true;
						break;
					}
				}
		
				if (alreadyDefined == false) {
					shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

					// do not reset global environment color parameter
					CString fullName;
					if (paramName == L"global_environment_global") 
						fullName = argType + L" " + paramName;
					else 
						fullName = argType + L" " + paramName + L"_" + stitem.shaderName;

					if (argType == L"string") 
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
					else 
						newline = L"\t" + fullName + L" = " + stitem.paramValue + L";\n";

					if (paramName == L"txt2d_image_explicit_tspace_id" || paramName == L"txt2d_scalarimage_explicit_tspace_id") {
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
						if (stitem.paramValue == L"" || stitem.paramValue == L"*") {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
						else if (stitem.paramValue != L"") {
							newline += L"\t" + paramName + L"_u = u_" + stitem.paramValue + L";\n";
							newline += L"\t" + paramName + L"_v = v_" + stitem.paramValue + L";\n";
						}
						else {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
					}

					fwprintf(stream, newline.GetWideString());
				}
			}
		}
	}
			
	// set output varying parameters
	fwprintf(stream, CString(L"\n\t__diffusecolor = diffusecolor;\n").GetWideString());
	fwprintf(stream, CString(L"\t__ambientcolor = ambientcolor;\n").GetWideString());
	fwprintf(stream, CString(L"\t__specularcolor = specularcolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__radiancecolor = radiancecolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__transparencycolor = transparencycolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__refrcolor = refrcolor;\n").GetWideString());
	fwprintf(stream, CString(L"\t__reflcolor = reflcolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__occlusioncolor = occlusioncolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__indirectcolor = indirectcolor;\n").GetWideString()); 
	fwprintf(stream, CString(L"\t__causticcolor = causticcolor;\n\n").GetWideString());
	fwprintf(stream, CString(L"\t__shadowcolor = shadowcolor;\n\n").GetWideString());

	fwprintf(stream, L"}\n");

	if (shaderType != L"surface") {
		stat = CStatus::Fail;
	}

	// cleanup
	fclose(stream);
	shaderTemplates.clear();
	shaderArgList.clear();	
	
	// compile shader
	if (shaderName != L"" && shaderName != L"xsimanParticle" && prejobState == true) 
		compileShader(shaderName);
		
	return stat;
}

XSI::CStatus xsiRibTranslator::getDisplacementShader(XSI::Material& material, CString nodeName, CString& shaderName)
{
	FILE* stream;
	CString shaderType;
	CStatus stat = CStatus::OK;
	vector<CString> shaderArgList;
	
	// only look at displacement shaders
	Parameter matparm = material.GetParameters().GetItem(L"displacement");
	CRef paramSrc = matparm.GetSource();

	if (paramSrc.IsValid() == false) {
		return CStatus::Fail;
	}

	// open file handle
	CString materialName = material.GetName();
	shaderName = L"dsp_" + materialName;
	CString shaderFile = g_ShaderPath + L"/" + shaderName + L".sl"; 
	shaderFile = replaceSubString(shaderFile, L"\\", L"/");

	// check if this shader has already been created 
	//if (shaderExists(shaderName) == true) {
		//addShaderParamToTokenList(shaderName, L"displacement");
		//return CStatus::OK;
	//}
		
	// insert in database
	shaderList.push_back(shaderName);

	if ((stream = _wfopen(shaderFile.GetWideString(), L"w")) == NULL) {
		m_app.LogMessage(L"Could not create shader " + shaderName + L"!");
		return CStatus::Fail;
	}

	// write out shader
	shaderType = L"displace";
	Shader shader(paramSrc);

	// this may help speed things up
	shaderTemplates.reserve(1000);

	shaderTemplateItem stitem;
	stitem.subShader = L"";
	stitem.subShaderName = L"";
	stitem.param = L"";
	stitem.paramValue = L"";
	stitem.isAnimated = false;
	stitem.shader = shader.GetProgID();
	stitem.shaderName = shader.GetName();
	shaderTemplates.push_back(stitem);

	// recurse shader tree
	getShaderNodes(shader);

	if (g_Renderer == L"Air") {
		fwprintf(stream, CString(L"#include \"combined.h\"\n").GetWideString());
	}
	else {
		fwprintf(stream, CString(L"#include \"utility.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"conversion.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"imageprocessing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"texture.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"mixers.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"bump.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"switch.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"colorchannels.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"state.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"raytracing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"math.h\"\n").GetWideString());
	}

	CString newline = L"\ndisplacement " + shaderName + L"(\n\tDEF_TEXTURE_PARAMS\n";
	fwprintf(stream, newline.GetWideString());

	// add animated parameters
	vector<shaderTemplateItem>::reverse_iterator iter;
	iter = shaderTemplates.rbegin();
	long paramIndex = 1;
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			if (stitem.isAnimated == true) {
				CString paramName = CString(sfunc1) + L"_" + stitem.param;

				// add definition for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argval = stitem.paramValue;
						if (CString((*jter).second) == L"string") {
							argval = L"\"" + stitem.paramValue + L"\"";
						}
						paramName = shaderName + L"__dspparam" + CValue(paramIndex).GetAsText();
						paramIndex += 1;
						CString newline = L"\t" + CString((*jter).second) + L" " + paramName + L" = " + argval + L";\n";				
						fwprintf(stream, newline.GetWideString());
					}
				}
			}
		}
	}

	// add varying parameters for texture UVs
	vector<CString> varyingParams;
	iter = shaderTemplates.rbegin();	
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			vector<CString>::reverse_iterator lter;
			lter = varyingParams.rbegin();	
			bool addParam = true;
			while (lter != varyingParams.rend()) {
				if ((*lter) == stitem.paramValue) {
					addParam = false;
					break;
				}
				lter++;
			}

			if (stitem.param == L"tspace_id" && stitem.paramValue != L"" && stitem.paramValue != L"*" && addParam == true) {
				varyingParams.push_back(stitem.paramValue);
				// add definition for texture coordinate parameter
				CString paramName = L"u_" + stitem.paramValue;
				CString newline = L"\tvarying float " + paramName + L" = 0;\n";	
				fwprintf(stream, newline.GetWideString());
				paramName = L"v_" + stitem.paramValue;
				newline = L"\tvarying float " + paramName + L" = 0;\n";				
				fwprintf(stream, newline.GetWideString());
			}
		}
	}

	fwprintf(stream, L")\n");
	fwprintf(stream, L"{\n");

	// parameter definitions	
	map<const wchar_t*, const wchar_t*> paramDefs;
	iter = shaderTemplates.rbegin();
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;				
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderArgDefs.begin(); jter!=shaderArgDefs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
		}
	}

	map<const wchar_t*, const wchar_t*>::reverse_iterator kter;
	kter = paramDefs.rbegin();
	while (kter != paramDefs.rend()) {
		CString newline = L"\t" + CString((*kter).second) + L"\n";
		fwprintf(stream, newline.GetWideString());
		kter++;
	}
	paramDefs.clear();

	iter = shaderTemplates.rbegin();
	paramIndex = 1;
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L""; wchar_t* sfunc3 = L""; wchar_t* sfunc4 = L"";

		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc3 = (*jter).second;
		}
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
			if (CString(stitem.shader.GetWideString()) == CString((*jter).first)) 
				sfunc4 = (*jter).second;
		}

		if (iter == shaderTemplates.rend()) {
			// reset unique parameter names to global names
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc1).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc1)) {
					newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.shaderName + L";\n";
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString newline;
			newline = L"\tfloat dispvalue = " + CString(sfunc1) + L"(" + CString(sfunc4) + L");\n";
			fwprintf(stream, newline.GetWideString());
			newline = L"\tfnc_displace(dispvalue);\n";
			fwprintf(stream, newline.GetWideString());
		}
		else if (CString(sfunc1) != L"" && CString(sfunc2) != L"") {
			if (CString(sfunc3) == L"image_clip") {
				fwprintf(stream, CString(L"\timage_clip_invert_s = invert_s;\n").GetWideString());
				fwprintf(stream, CString(L"\timage_clip_invert_t = invert_t;\n").GetWideString());
			}

			// reset unique parameter names to global names 
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc3).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc3)) {
					newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.subShaderName + L";\n";
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString paramName = CString(sfunc1) + L"_" + stitem.param;
			CString argType = getArgType(paramName);

			// check if this parameter has already been defined
			vector<CString>::iterator lter;
			bool alreadyDefined = false;
			for (lter=shaderArgList.begin(); lter!=shaderArgList.end(); lter++) {
				if (*lter == paramName + L"_" + stitem.shaderName) {
					alreadyDefined = true;
					break;
				}
			}
		
			if (alreadyDefined == false) {
				shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

				CString newline;
				newline = L"\t" + argType + L" " + paramName + L"_" + stitem.shaderName + L" = " + CString(sfunc3) + L"(" + CString(sfunc2) + L");\n";	
				fwprintf(stream, newline.GetWideString());
			}
		}
		else if (CString(sfunc1) != L"") {
			CString newline;
			CString paramName = CString(sfunc1) + L"_" + stitem.param;

			if (stitem.isAnimated == true) {
				// add rib tokens for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argType = (*jter).second;
						char* pname = NULL;
						CString nuName = shaderName + L"__dspparam" + CValue(paramIndex).GetAsText();

						// check if this parameter has already been defined
						vector<CString>::iterator kter;
						bool alreadyDefined = false;
						for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
							if (*kter == paramName + L"_" + stitem.shaderName) {
								alreadyDefined = true;
								break;
							}
						}
		
						if (alreadyDefined == false) {
							shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

							// set unique parameter names
							paramName = argType + L" " + paramName + L"_" + stitem.shaderName;
							newline = L"\t" + paramName + L" = " + nuName + L";\n";
							fwprintf(stream, newline.GetWideString());
						}

						paramIndex += 1;
						XSIW2A(&pname, nuName.GetWideString());
						char* pvalue = NULL;
						XSIW2A(&pvalue, stitem.paramValue.GetWideString());
						unsigned int strlen = stitem.paramValue.Length();

						if (argType == L"float") {		
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put(pname, FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, (float)atof(pvalue));
							ribTokensDisp.push_back(tokenDataFloat);
						}
						else if (argType == L"color") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"color (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, ColorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensDisp.push_back(tokenDataFloat);									
						}
						else if (argType == L"vector") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"vector (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, VectorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensDisp.push_back(tokenDataFloat);
						}
						else if (argType == L"string") {
							xsiRibTokens tokenDataString;
							tokenDataString.put(pname, StringType, false, false, false, 0);
							tokenDataString.putDetailType(UniformDetail);
							tokenDataString.assignString(pvalue, strlen);
							ribTokensDisp.push_back(tokenDataString);
						}

						// add shader params for shader param list as well
						shaderParamStruct spstruct;
						spstruct.shaderName = shaderName;
						spstruct.argType = argType;
						spstruct.paramName = nuName;
						spstruct.paramValue = stitem.paramValue;
						shaderParamList.push_back(spstruct);
					}
				}
			}
			else {
				// set unique parameter names
				CString argType = getArgType(paramName);

				// check if this parameter has already been defined
				vector<CString>::iterator kter;
				bool alreadyDefined = false;
				for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
					if (*kter == paramName + L"_" + stitem.shaderName) {
						alreadyDefined = true;
						break;
					}
				}
		
				if (alreadyDefined == false) {
					shaderArgList.push_back(paramName + L"_" + stitem.shaderName);
					CString fullName = argType + L" " + paramName + L"_" + stitem.shaderName;
					
					if (argType == L"string") 
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
					else 
						newline = L"\t" + fullName + L" = " + stitem.paramValue + L";\n";
					
					if (paramName == L"txt2d_image_explicit_tspace_id" || paramName == L"txt2d_scalarimage_explicit_tspace_id") {
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
						if (stitem.paramValue == L"" || stitem.paramValue == L"*") {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
						else if (stitem.paramValue != L"") {
							newline += L"\t" + paramName + L"_u = u_" + stitem.paramValue + L";\n";
							newline += L"\t" + paramName + L"_v = v_" + stitem.paramValue + L";\n";
						}
						else {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
					}

					fwprintf(stream, newline.GetWideString());
				}
			}
		}
	}

	fwprintf(stream, L"}\n");

	if (shaderType != L"displace") {
		stat = CStatus::Fail;
	}

	// cleanup
	fclose(stream);
	shaderTemplates.clear();
	shaderArgList.clear();
	
	// compile shader
	if (shaderName != L"" && shaderName != L"xsimanParticle" && prejobState == true) 
		compileShader(shaderName);

	return stat;
}

XSI::CStatus xsiRibTranslator::getVolumeShader(XSI::Material& material, CString nodeName, CString& shaderName)
{
	FILE* stream;
	CString shaderType;
	CStatus stat = CStatus::OK;
	vector<CString> shaderArgList;

	// only look at volume shaders
	Parameter matparm = material.GetParameters().GetItem(L"volume");
	CRef paramSrc = matparm.GetSource();

	if (paramSrc.IsValid() == false) {
		return CStatus::Fail;
	}

	// open file handle
	CString materialName = material.GetName();
	shaderName = L"vol_" + materialName;
	CString shaderFile = g_ShaderPath + L"/" + shaderName + L".sl"; 
	shaderFile = replaceSubString(shaderFile, L"\\", L"/");

	// check if this shader has already been created 
	//if (shaderExists(shaderName) == true) {
		//addShaderParamToTokenList(shaderName, L"volume");
		//return CStatus::OK;
	//}
		
	// insert in database
	shaderList.push_back(shaderName);

	if ((stream = _wfopen(shaderFile.GetWideString(), L"w")) == NULL) {
		m_app.LogMessage(L"Could not create shader " + shaderName + L"!");
		return CStatus::Fail;
	}

	// write out shader
	shaderType = L"volume";
	Shader shader(paramSrc);

	// this may help speed things up
	shaderTemplates.reserve(1000);

	shaderTemplateItem stitem;
	stitem.subShader = L"";
	stitem.subShaderName = L"";
	stitem.param = L"";
	stitem.paramValue = L"";
	stitem.isAnimated = false;
	stitem.shader = shader.GetProgID();
	stitem.shaderName = shader.GetName();
	shaderTemplates.push_back(stitem);

	// recurse shader tree
	getShaderNodes(shader);

	if (g_Renderer == L"Air") {
		fwprintf(stream, CString(L"#include \"combined.h\"\n").GetWideString());
	}
	else {
		fwprintf(stream, CString(L"#include \"utility.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"conversion.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"imageprocessing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"texture.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"mixers.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"bump.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"switch.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"colorchannels.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"state.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"volume.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"raytracing.h\"\n").GetWideString());
		fwprintf(stream, CString(L"#include \"math.h\"\n").GetWideString());
	}

	CString newline = L"\nvolume " + shaderName + L"(\n\tDEF_TEXTURE_PARAMS\n";
	fwprintf(stream, newline.GetWideString());

	// add animated parameters
	vector<shaderTemplateItem>::reverse_iterator iter;
	iter = shaderTemplates.rbegin();
	long paramIndex = 1;
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			if (stitem.isAnimated == true) {
				CString paramName = CString(sfunc1) + L"_" + stitem.param;

				// add definition for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argval = stitem.paramValue;
						if (CString((*jter).second) == L"string") {
							argval = L"\"" + stitem.paramValue + L"\"";
						}
						paramName = shaderName + L"__volparam" + CValue(paramIndex).GetAsText();
						paramIndex += 1;
						CString newline = L"\t" + CString((*jter).second) + L" " + paramName + L" = " + argval + L";\n";				
						fwprintf(stream, newline.GetWideString());
					}
				}
			}
		}
	}
		
	// add varying parameters for texture UVs
	vector<CString> varyingParams;
	iter = shaderTemplates.rbegin();	
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;	
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L"";
								
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
		}	
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
		}

		if (CString(sfunc1) != L"" && CString(sfunc2) == L"") {
			vector<CString>::reverse_iterator lter;
			lter = varyingParams.rbegin();	
			bool addParam = true;
			while (lter != varyingParams.rend()) {
				if ((*lter) == stitem.paramValue) {
					addParam = false;
					break;
				}
				lter++;
			}

			if (stitem.param == L"tspace_id" && stitem.paramValue != L"" && stitem.paramValue != L"*" && addParam == true) {
				varyingParams.push_back(stitem.paramValue);
				// add definition for texture coordinate parameter
				CString paramName = L"u_" + stitem.paramValue;
				CString newline = L"\tvarying float " + paramName + L" = 0;\n";	
				fwprintf(stream, newline.GetWideString());
				paramName = L"v_" + stitem.paramValue;
				newline = L"\tvarying float " + paramName + L" = 0;\n";				
				fwprintf(stream, newline.GetWideString());
			}
		}
	}

	fwprintf(stream, L")\n");
	fwprintf(stream, L"{\n");

	// parameter definitions	
	map<const wchar_t*, const wchar_t*> paramDefs;
	iter = shaderTemplates.rbegin();
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;				
		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderArgDefs.begin(); jter!=shaderArgDefs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) {
				paramDefs.insert(pair<const wchar_t*, const wchar_t*>((*jter).first, (*jter).second));
			}
		}
	}

	map<const wchar_t*, const wchar_t*>::reverse_iterator kter;
	kter = paramDefs.rbegin();
	while (kter != paramDefs.rend()) {
		CString newline = L"\t" + CString((*kter).second) + L"\n";
		fwprintf(stream, newline.GetWideString());
		kter++;
	}
	paramDefs.clear();

	iter = shaderTemplates.rbegin();
	paramIndex = 1;
	while (iter != shaderTemplates.rend()) {
		shaderTemplateItem stitem = *iter; 
		iter++;
		wchar_t* sfunc1 = L""; wchar_t* sfunc2 = L""; wchar_t* sfunc3 = L""; wchar_t* sfunc4 = L"";

		map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
		for (jter=shaderFuncs.begin(); jter!=shaderFuncs.end(); ++jter) {
			if (CString(stitem.shader) == CString((*jter).first)) 
				sfunc1 = (*jter).second;
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc3 = (*jter).second;
		}
		for (jter=shaderArgs.begin(); jter!=shaderArgs.end(); ++jter) {
			if (CString(stitem.subShader.GetWideString()) == CString((*jter).first)) 
				sfunc2 = (*jter).second;
			if (CString(stitem.shader.GetWideString()) == CString((*jter).first)) 
				sfunc4 = (*jter).second;
		}

		if (iter == shaderTemplates.rend()) {
			// reset unique parameter names to global names
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc1).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc1)) {
					newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.shaderName + L";\n";
					fwprintf(stream, newline.GetWideString());
				}
			}
				
			CString newline = L"\n\tCi = " + CString(sfunc1) + L"(" + CString(sfunc4) + L");\n";
			fwprintf(stream, newline.GetWideString());
		}
		else if (CString(sfunc1) != L"" && CString(sfunc2) != L"") {
			if (CString(sfunc3) == L"image_clip") {
				fwprintf(stream, CString(L"\timage_clip_invert_s = invert_s;\n").GetWideString());
				fwprintf(stream, CString(L"\timage_clip_invert_t = invert_t;\n").GetWideString());
			}

			// reset unique parameter names to global names 
			map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
			long slen = CString(sfunc3).Length();
			for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
				CString substr = subString(CString((*jter).first), 0, slen-1);
				CString newline;
				if (substr == CString(sfunc3)) {
					newline = L"\t" + CString((*jter).first) + L" = " + CString((*jter).first) + L"_" + stitem.subShaderName + L";\n";
					fwprintf(stream, newline.GetWideString());
				}
			}

			CString paramName = CString(sfunc1) + L"_" + stitem.param;
			CString argType = getArgType(paramName);

			// check if this parameter has already been defined
			vector<CString>::iterator lter;
			bool alreadyDefined = false;
			for (lter=shaderArgList.begin(); lter!=shaderArgList.end(); lter++) {
				if (*lter == paramName + L"_" + stitem.shaderName) {
					alreadyDefined = true;
					break;
				}
			}
		
			if (alreadyDefined == false) {
				shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

				CString newline;
				newline = L"\t" + argType + L" " + paramName + L"_" + stitem.shaderName + L" = " + CString(sfunc3) + L"(" + CString(sfunc2) + L");\n";	
				fwprintf(stream, newline.GetWideString());
			}
		}
		else if (CString(sfunc1) != L"") {
			CString newline;
			CString paramName = CString(sfunc1) + L"_" + stitem.param;

			if (stitem.isAnimated == true) {
				// add rib tokens for animated parameters
				map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
				for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
					if (CString((*jter).first) == paramName) {
						CString argType = (*jter).second;
						char* pname = NULL;
						CString nuName = shaderName + L"__volparam" + CValue(paramIndex).GetAsText();

						// check if this parameter has already been defined
						vector<CString>::iterator kter;
						bool alreadyDefined = false;
						for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
							if (*kter == paramName + L"_" + stitem.shaderName) alreadyDefined = true;
						}
		
						if (alreadyDefined == false) {
							shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

							// set unique parameter names
							paramName = argType + L" " + paramName + L"_" + stitem.shaderName;
							newline = L"\t" + paramName + L" = " + nuName + L";\n";
							fwprintf(stream, newline.GetWideString());
						}

						paramIndex += 1;
						XSIW2A(&pname, nuName.GetWideString());
						char* pvalue = NULL;
						XSIW2A(&pvalue, stitem.paramValue.GetWideString());
						unsigned int strlen = stitem.paramValue.Length();

						if (argType == L"float") {		
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put(pname, FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, (float)atof(pvalue));
							ribTokensVol.push_back(tokenDataFloat);
						}
						else if (argType == L"color") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"color (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, ColorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensVol.push_back(tokenDataFloat);									
						}
						else if (argType == L"vector") {
							float floatParam[3] = {0.0, 0.0, 0.0};
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.putDetailType(UniformDetail);
							swscanf(stitem.paramValue.GetWideString(), L"vector (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
							tokenDataFloat.put(pname, VectorType, false, false, false, 0);							
							tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
							ribTokensVol.push_back(tokenDataFloat);
						}
						else if (argType == L"string") {
							xsiRibTokens tokenDataString;
							tokenDataString.put(pname, StringType, false, false, false, 0);
							tokenDataString.putDetailType(UniformDetail);
							tokenDataString.assignString(pvalue, strlen);
							ribTokensVol.push_back(tokenDataString);
						}

						// add shader params for shader param list as well
						shaderParamStruct spstruct;
						spstruct.shaderName = shaderName;
						spstruct.argType = argType;
						spstruct.paramName = nuName;
						spstruct.paramValue = stitem.paramValue;
						shaderParamList.push_back(spstruct);
					}
				}
			}
			else {
				// set unique parameter names
				CString argType = getArgType(paramName);

				// check if this parameter has already been defined
				vector<CString>::iterator kter;
				bool alreadyDefined = false;
				for (kter=shaderArgList.begin(); kter!=shaderArgList.end(); kter++) {
					if (*kter == paramName + L"_" + stitem.shaderName) {
						alreadyDefined = true;
						break;
					}
				}
		
				if (alreadyDefined == false) {
					shaderArgList.push_back(paramName + L"_" + stitem.shaderName);

					CString fullName = argType + L" " + paramName + L"_" + stitem.shaderName;

					if (argType == L"string") 
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
					else 
						newline = L"\t" + fullName + L" = " + stitem.paramValue + L";\n";
					
					if (paramName == L"txt2d_image_explicit_tspace_id" || paramName == L"txt2d_scalarimage_explicit_tspace_id") {
						newline = L"\t" + fullName + L" = \"" + stitem.paramValue + L"\";\n";
						if (stitem.paramValue == L"" || stitem.paramValue == L"*") {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
						else if (stitem.paramValue != L"") {
							newline += L"\t" + paramName + L"_u = u_" + stitem.paramValue + L";\n";
							newline += L"\t" + paramName + L"_v = v_" + stitem.paramValue + L";\n";
						}
						else {
							newline += L"\t" + paramName + L"_u = s;\n";
							newline += L"\t" + paramName + L"_v = t;\n";
						}
					}

					fwprintf(stream, newline.GetWideString());
				}
			}
		}
	}

	fwprintf(stream, L"}\n");

	if (shaderType != L"volume") {
		stat = CStatus::Fail;
	}

	// cleanup
	fclose(stream);
	shaderTemplates.clear();
	shaderArgList.clear();
	
	// compile shader
	if (shaderName != L"" && shaderName != L"xsimanParticle" && prejobState == true) 
		compileShader(shaderName);

	return stat;
}

XSI::CStatus xsiRibTranslator::updateTextureMaps(XSI::Material& material) 
{
	CStatus stat = CStatus::OK;
	CString sourceFile = L"";

	if(material.IsValid()) {
		CRefArray clipArray = material.GetImageClips();
		for (long k=0; k<clipArray.GetCount(); k++ ) {
			ImageClip2 imageclip = clipArray[k];	
			
			// get file name directly
			sourceFile = imageclip.GetFileName();
			sourceFile = replaceSubString(sourceFile, L"\\", L"/");
						
			// convert texture
			if (sourceFile != L"") { 
				CString destFile;
				CString fileName = stringAfterLastAppearance(sourceFile, L"/");
				if (g_Renderer == L"PRMan") destFile = g_TexPath + fileName + L".tex";
				else if (g_Renderer == L"3Delight") destFile = g_TexPath + fileName + L".tdl";
				else if (g_Renderer == L"Air") destFile = g_TexPath + fileName + L".tx";;

				// do we cleanup textures?
				if (g_Cleanup == 4 || g_Cleanup == 5 || g_Cleanup == 20 || g_Cleanup == 21) 
					cleanupList.push_back(destFile);

				unsigned int strlen = destFile.Length();
				destFile = replaceSubString(destFile, L"\\", L"/");
				char* destfile = NULL;
				XSIW2A(&destfile, destFile.GetWideString());

				// get file timestamps
				struct _stat buffer;
				int status;
				char* sourcefile = NULL;
				XSIW2A(&sourcefile, sourceFile.GetWideString());
				status = _stat(sourcefile, &buffer);
				time_t stime = buffer.st_mtime;
				status = _stat(destfile, &buffer);
				time_t dtime = buffer.st_mtime;

				if (access(destfile, 0) || stime > dtime) {
					CString texmake = textureCommand(sourceFile, destFile);
					preframeList.push_back(texmake);
				}
			}
		}
	}

	return stat;
}

XSI::CStatus xsiRibTranslator::getImageClip(ImageClip2 imageClip) 
{
	CStatus stat = CStatus::OK;
	shaderTemplateItem stitem;
	CString sourceFile = L"";
	CString sourceName = L"";

	// add XRes and YRes paramaters
	Source clipSource(imageClip.GetSource());
	CValue XRes = clipSource.GetParameterValue(L"XRes");
    CValue YRes = clipSource.GetParameterValue(L"YRes");

	stitem.subShader = L"";
	stitem.subShaderName = L"";
	stitem.shader = L"image_clip";
	stitem.shaderName = imageClip.GetName();
	stitem.isAnimated = false;

	stitem.param = L"XRes";
	stitem.paramValue = XRes;		
	
	// make sure this shader argument is valid
	if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);

	stitem.param = L"YRes";
	stitem.paramValue = YRes;

	// make sure this shader argument is valid
	if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);

	CRefArray paramArray = imageClip.GetParameters();
	CRefArray animParamArray = imageClip.GetAnimatedParameters(siExpressionSource);
	paramArray += animParamArray;
	for (long j = 0; j < paramArray.GetCount(); j++) {
		Parameter param(paramArray[j]);
		stitem.isAnimated = param.IsAnimated();
		
		stitem.subShader = L"";
		stitem.subShaderName = L"";
		stitem.param = param.GetName();

		// replace bad characters
		stitem.param = replaceSubString(stitem.param, L" ", L"_");
		stitem.param = replaceSubString(stitem.param, L".", L"");
		stitem.param = replaceSubString(stitem.param, L"(", L"");
		stitem.param = replaceSubString(stitem.param, L")", L"");

		stitem.paramValue = getParamValue(param);

		if (stitem.param == L"Source_File") {
			//sourceFile = param.GetValue().GetAsText();
			//sourceFile = replaceSubString(sourceFile, L"\\", L"/");
			//stitem.paramValue = sourceFile;
	
			// get file name directly
			sourceFile = imageClip.GetFileName();
			sourceFile = replaceSubString(sourceFile, L"\\", L"/");
			stitem.paramValue = sourceFile;

			// assume image clips are animated
			stitem.isAnimated = true;
		}
		else if (stitem.param == L"Name") {
			sourceName = param.GetValue().GetAsText();
			stitem.paramValue = sourceName;
		}

		stitem.shader = L"image_clip";
		
		// make sure this shader argument is valid
		if (shaderArgIsValid(stitem) == CStatus::OK) 
			shaderTemplates.push_back(stitem);
	}

	// convert texture
	if (sourceFile != L"") { 

		CString destFile;
		CString fileName = stringAfterLastAppearance(sourceFile, L"/");
		if (g_Renderer == L"PRMan") destFile = g_TexPath + fileName + L".tex";
		else if (g_Renderer == L"3Delight") destFile = g_TexPath + fileName + L".tdl";
		else if (g_Renderer == L"Air") destFile = g_TexPath + fileName + L".tx";
		//else if (g_Renderer == L"RenderDotC") destFile = g_TexPath + fileName + L".tex";

		// do we cleanup textures?
		if (g_Cleanup == 4 || g_Cleanup == 5 || g_Cleanup == 20 || g_Cleanup == 21) 
			cleanupList.push_back(destFile);

		unsigned int strlen = destFile.Length();
		destFile = replaceSubString(destFile, L"\\", L"/");
		char* destfile = NULL;
		XSIW2A(&destfile, destFile.GetWideString());

		// get file timestamps
		struct _stat buffer;
		int status;
		char* sourcefile = NULL;
		XSIW2A(&sourcefile, sourceFile.GetWideString());
		status = _stat(sourcefile, &buffer);
		time_t stime = buffer.st_mtime;
		status = _stat(destfile, &buffer);
		time_t dtime = buffer.st_mtime;

		if (access(destfile, 0) || stime > dtime) {
			CString texmake = textureCommand(sourceFile, destFile);
			prejobList.push_back(texmake);
		}
				
		// update value of shader template item
		vector<shaderTemplateItem>::iterator jter;
		for (jter=shaderTemplates.begin(); jter!=shaderTemplates.end(); jter++) {
			if ((*jter).param == L"Source_File" && (*jter).paramValue == sourceFile) {
				shaderTemplateItem stitem;
				stitem.param = (*jter).param;
				stitem.isAnimated = (*jter).isAnimated;
				stitem.paramValue = destFile;
				stitem.shader = (*jter).shader;
				stitem.subShader = (*jter).subShader;
				stitem.subShaderName = (*jter).subShaderName;
				stitem.shaderName = (*jter).shaderName;
				(*jter) = stitem;
				break;
			}
		}
	}

	return stat;
}

XSI::CStatus xsiRibTranslator::getShaderNodes(Shader shader) 
{
	CStatus stat = CStatus::OK;
	shaderTemplateItem stitem;
	
	CRefArray paramArray = shader.GetParameters();
	for (long j = 0; j < paramArray.GetCount(); j++) {
		Parameter param(paramArray[j]);
		stitem.isAnimated = param.IsAnimated();
		//m_app.LogMessage(L"DEBUG: " + shader.GetFullName() + L" " + param.GetName());

		// get connections
		CRef paramSrc = param.GetSource();
		if (paramSrc.IsValid()) {
			Shader subShader;
			if (paramSrc.GetClassIDName() == L"ImageClip") {
				ImageClip2 imageClip(paramSrc);
				stitem.subShader = L"image_clip";
				stitem.subShaderName = imageClip.GetName();
				stitem.param = param.GetName();
				stitem.param = replaceSubString(stitem.param, L" ", L"_");
				stitem.param = replaceSubString(stitem.param, L".", L"");
				stitem.param = replaceSubString(stitem.param, L"(", L"");
				stitem.param = replaceSubString(stitem.param, L")", L"");
				stitem.paramValue = L"";
				stitem.shader = shader.GetProgID();
				stitem.shaderName = shader.GetName();

				if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);	
				
				getImageClip(imageClip);
			}
			else {
				subShader = paramSrc;
				stitem.subShader = subShader.GetProgID();
				stitem.subShaderName = subShader.GetName();
				stitem.param = param.GetName();
				stitem.param = replaceSubString(stitem.param, L" ", L"_");
				stitem.param = replaceSubString(stitem.param, L".", L"");
				stitem.param = replaceSubString(stitem.param, L"(", L"");
				stitem.param = replaceSubString(stitem.param, L")", L"");
				stitem.paramValue = L"";
				stitem.shader = shader.GetProgID();
				stitem.shaderName = shader.GetName();

				// check if parameter is animated by expression 
				if (stitem.subShader == L"") {
					//if (param.IsAnimated(siExpressionSource)) {
					if (param.IsAnimated()) {
						stitem.paramValue = getParamValue(param);
					
						// make sure shader function is valid
						if (shaderFuncIsValid(stitem.shader) == CStatus::Fail) stitem.shader = L"unimplemented";
												
						// make sure shader arg is valid
						if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);
					}
				}
				else {
					// make sure shader functions are valid
					if (shaderFuncIsValid(stitem.shader) == CStatus::Fail) stitem.shader = L"unimplemented";
					if (shaderFuncIsValid(stitem.subShader) == CStatus::Fail) stitem.subShader = L"unimplemented";
				
					// make sure shader arg is valid
					if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);

					getShaderNodes(subShader);
				}
			}
		}
		else {
			stitem.subShader = L"";
			stitem.subShaderName = L"";
			stitem.param = param.GetName();
			stitem.param = replaceSubString(stitem.param, L" ", L"_");
			stitem.param = replaceSubString(stitem.param, L".", L"");
			stitem.param = replaceSubString(stitem.param, L"(", L"");
			stitem.param = replaceSubString(stitem.param, L")", L"");
			stitem.paramValue = getParamValue(param);
			stitem.shader = shader.GetProgID();
			stitem.shaderName = shader.GetName();
			
			// for some reason some color params are not always returned the usual way (ambience for example)
			if (stitem.paramValue == L"") {
				CValue::DataType ptype = param.GetValueType();
				if (ptype == siUnknown || ptype == siEmpty) {
					float rgb[3]; float alphachan;
					shader.GetColorParameterValue(stitem.param, rgb[0], rgb[1], rgb[2], alphachan, g_CurrentFrame);
					stitem.paramValue = L"color (" + CValue(rgb[0]).GetAsText() + L"," + CValue(rgb[1]).GetAsText() + L"," + CValue(rgb[2]).GetAsText() + L")";
				}

				// This isn't working, try and get the ambience directly from the ambient lighting property
				// <object>.AmbientLighting.ambience.red
			}
			
			// make sure shader functions are valid
			if (shaderFuncIsValid(stitem.shader) == CStatus::Fail) stitem.shader = L"unimplemented";

			// make sure this shader argument is valid
			if (shaderArgIsValid(stitem) == CStatus::OK) shaderTemplates.push_back(stitem);
		}
	}

	return stat;
}

XSI::CStatus xsiRibTranslator::getShaderParams(Shader shader)
{
	CStatus stat = CStatus::OK;

	CRefArray paramArray = shader.GetParameters();
	long numParam = paramArray.GetCount();
	for (long j=0; j<numParam; j++ ) {
		Parameter param = paramArray[j];
		CValue::DataType ptype = param.GetValueType();
		CString paramName = param.GetName();
		CValue paramValue = param.GetValue();

		// get connections
		CRef paramSrc = param.GetSource();
		if (paramSrc.GetAsText() != L"") {
			Shader subShader = paramSrc;
			CRefArray clipArray = subShader.GetImageClips();
			for (long k=0; k<clipArray.GetCount(); k++ ) {
				ImageClip2 imageclip = clipArray[k];
				CString sourceFile = L"";
				CString sourceName = L"";
				CParameterRefArray parray = imageclip.GetParameters();
				for (long l=0; l<parray.GetCount(); l++ ) {
					Parameter cparam = parray[l];
					CString cpname = cparam.GetName();
					CValue cpvalue = cparam.GetValue();
					if (cpname == L"Source File") {
						//sourceFile = cpvalue.GetAsText();		
							
						// get file name directly
						sourceFile = imageclip.GetFileName();
			
						// substitute global variables
						sourceFile = subGlobalVars(sourceFile);
						
						// replace object name as well	
						if (stringHasChar(sourceFile, L"$OBJ")) {
							sourceFile = replaceSubString(sourceFile, L"$OBJ", g_CurrentNode);
						}

						// we seem to need to replace back slashes
						sourceFile = replaceSubString(sourceFile, L"\\", L"/");
					}
					else if (cpname == L"Name") {
						sourceName = cpvalue.GetAsText();
					}
				}
				
				if (sourceFile != L"") {
					sourceFile = replaceSubString(sourceFile, L"\\", L"/");
					CString destFile;
					CString fileName = stringAfterLastAppearance(sourceFile, L"/");
					if (g_Renderer == L"PRMan") destFile = g_TexPath + fileName + L".tex";
					else if (g_Renderer == L"3Delight") destFile = g_TexPath + fileName + L".tdl";
					else if (g_Renderer == L"Air") destFile = g_TexPath + fileName + L".tx";
					//else if (g_Renderer == L"RenderDotC") destFile = g_TexPath + fileName + L".tex";

					// do we cleanup textures?
					if (g_Cleanup == 4 || g_Cleanup == 5 || g_Cleanup == 20 || g_Cleanup == 21) 
						cleanupList.push_back(destFile);

					unsigned int strlen = destFile.Length();
					destFile = replaceSubString(destFile, L"\\", L"/");
					char* destfile = NULL;
					XSIW2A(&destfile, destFile.GetWideString());

					// get file timestamps
					struct _stat buffer;
					int status;
					char* sourcefile = NULL;
					XSIW2A(&sourcefile, sourceFile.GetWideString());
					status = _stat(sourcefile, &buffer);
					time_t stime = buffer.st_mtime;
					status = _stat(destfile, &buffer);
					time_t dtime = buffer.st_mtime;

					if (access(destfile, 0) || stime > dtime) {
						CString texmake = textureCommand(sourceFile, destFile);
						prejobList.push_back(texmake);
					}

					xsiRibTokens tokenDataString;
					char* pname = NULL;
					CString nuname = paramName + L"map";
					XSIW2A(&pname, nuname.GetWideString());
					tokenDataString.put(pname, StringType, false, false, false, 0);
					tokenDataString.putDetailType(UniformDetail);
					tokenDataString.assignString(destfile, strlen);
					ribTokens.push_back(tokenDataString);
				}
			}
		}
		
		char* pname = NULL;
		XSIW2A(&pname, paramName.GetWideString());

		if (ptype == siFloat || ptype == siDouble || ptype == siInt2 || ptype == siInt4 || ptype == siBool) {
			xsiRibTokens tokenDataFloat;
			tokenDataFloat.put(pname, FloatType, false, false, false, 0);
			tokenDataFloat.putDetailType(UniformDetail);
			tokenDataFloat.assignFloat(0, (float)paramValue);
			ribTokens.push_back(tokenDataFloat);
		}
		else if (ptype == siUnknown || ptype == siEmpty) {
			float floatParam[3] = {0.0, 0.0, 0.0};
			xsiRibTokens tokenDataFloat; float alphachan;

			// constant shader has a parameter named "color"
			if (!strcmp(pname, "color")) pname = "diffuse";

			// ambience seems to be a key word in RSL
			if (!strcmp(pname, "ambience")) pname = "ambienceval";
			
			// bump seems to be a key word in RSL
			if (!strcmp(pname, "bump")) pname = "bumpval";

			tokenDataFloat.putDetailType(UniformDetail);
			shader.GetColorParameterValue(paramName, floatParam[0], floatParam[1], floatParam[2], alphachan, g_CurrentFrame);
			tokenDataFloat.put(pname, ColorType, false, false, false, 0);							
			tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
			ribTokens.push_back(tokenDataFloat);
		}
		else if (ptype == siString) {
			if (paramName != L"Name") {
				xsiRibTokens tokenDataString;
				tokenDataString.put(pname, StringType, false, false, false, 0);
				tokenDataString.putDetailType(UniformDetail);

				CString nuStringName = paramValue.GetAsText();

				// substitute global variables
				nuStringName = subGlobalVars(nuStringName);
										
				// replace object name as well	
				if (stringHasChar(nuStringName, L"$OBJ")) {
					nuStringName = replaceSubString(nuStringName, L"$OBJ", g_CurrentNode);
				}
				
				// we seem to need to replace back slashes
				nuStringName = replaceSubString(nuStringName, L"\\", L"/");

				unsigned int strlen = nuStringName.Length();
				char* str = NULL;
				XSIW2A(&str, nuStringName.GetWideString());

				tokenDataString.assignString(str, strlen);
				ribTokens.push_back(tokenDataString);
			}
		}
	}
	
	return stat;
}

// add ambient occlusion and indirect illumination params here
void xsiRibTranslator::addGlobalIllumParams(vector<xsiRibTokens>& ribTokens)
{
	float notrace, enableAmbientOcclusion, enableIndirectIllumination;

	if (g_EnableRayTracing) notrace = 0;
	else notrace = 1;
	
	if (g_EnableAmbientOcclusion) enableAmbientOcclusion = 1;
	else enableAmbientOcclusion = 0;

	if (g_EnableIndirectIllumination) enableIndirectIllumination = 1;
	else enableIndirectIllumination = 0;

	xsiRibTokens tokenDataFloat1;
	tokenDataFloat1.put("notrace", FloatType, false, false, false, 0);
	tokenDataFloat1.putDetailType(UniformDetail);
	tokenDataFloat1.assignFloat(0, notrace);
	ribTokens.push_back(tokenDataFloat1);

	xsiRibTokens tokenDataFloat2;
	tokenDataFloat2.put("doOcclusion", FloatType, false, false, false, 0);
	tokenDataFloat2.putDetailType(UniformDetail);
	tokenDataFloat2.assignFloat(0, enableAmbientOcclusion);
	ribTokens.push_back(tokenDataFloat2);
				
	xsiRibTokens tokenDataFloat3;
	tokenDataFloat3.put("occlusion_samples", FloatType, false, false, false, 0);
	tokenDataFloat3.putDetailType(UniformDetail);
	tokenDataFloat3.assignFloat(0, (float)g_OcclusionSamples);
	ribTokens.push_back(tokenDataFloat3);

	xsiRibTokens tokenDataFloat4;
	tokenDataFloat4.put("maxdist", FloatType, false, false, false, 0);
	tokenDataFloat4.putDetailType(UniformDetail);
	tokenDataFloat4.assignFloat(0, (float)g_OcclusionMaxdist);
	ribTokens.push_back(tokenDataFloat4);

	xsiRibTokens tokenDataFloat5;
	tokenDataFloat5.put("doIndirect", FloatType, false, false, false, 0);
	tokenDataFloat5.putDetailType(UniformDetail);
	tokenDataFloat5.assignFloat(0, enableIndirectIllumination);
	ribTokens.push_back(tokenDataFloat5);

	xsiRibTokens tokenDataFloat6;
	tokenDataFloat6.put("indirect_samples", FloatType, false, false, false, 0);
	tokenDataFloat6.putDetailType(UniformDetail);
	tokenDataFloat6.assignFloat(0, (float)g_IndirectSamples);
	ribTokens.push_back(tokenDataFloat6);
}

XSI::CString xsiRibTranslator::getFileExt(CString format) 
{
	CString fileExt;
	wchar_t* drivers[14] = {L"it", L"combiner", L"framebuffer", L"file", L"tiff", L"alias", L"sgif", 
		L"mayaiff", L"cineon", L"targa", L"picio", L"softimage", L"shadow", L"zfile"};
	wchar_t* extensions[14] = {L"", L"", L"", L".tif", L".tif", L".rla", L".sgi", 
		L".iff", L".cin", L".tga", L".pic", L".pic", L".tex", L".zfile"};

	// determine file extension
	for (long i = 0; i < 14; i++) 
		if (format == drivers[i]) fileExt = extensions[i];

	return fileExt;
}

XSI::CString xsiRibTranslator::alfredJobScript()
{
	CString jobScript, renderCommand;
	CString services_tags = L"";
	CString procs = L""; 
	FILE* stream;
	vector<taskStruct*>::iterator iter;
	vector<taskStruct*>::iterator iiter;
	vector<CString>::iterator jter;

	renderCommand = g_RenderCommand;

	CString alfcommand = L"Cmd";
	if (g_RemoteCmd) alfcommand = L"RemoteCmd";

	if (g_AlfredServices != L"") services_tags =  L"-service {" + g_AlfredServices + L"}";
	if (g_AlfredTags != L"") services_tags += L" -tags {" + g_AlfredTags + L"}";
	procs = L" -atleast " + CValue((long)g_MinProcs).GetAsText() + L" -atmost " + CValue((long)g_MaxProcs).GetAsText();
	
	jobScript = g_JobPath + job->ribFileName + L"." + g_CameraName + L".alf"; 

	if ((stream = _wfopen(jobScript.GetWideString(), L"w")) == NULL)
		m_app.LogMessage(L"Could not create job script " + jobScript + L"!");
	else {
		fwprintf(stream, L"##AlfredToDo 3.0\n\n");
		fwprintf(stream, CString(L"Job -title {" + job->ribFileName + L"." + g_CameraName + L"} -comment {#Created by XSIMan 1.0}" + procs + L" -subtasks {\n").GetWideString());

		if (g_PreJobCommand != L"") {
			fwprintf(stream, L"\tTask {Pre-job command} -cmds {\n");
			fwprintf(stream, CString(L"\t\tCmd {" + g_PreJobCommand + L"}\n").GetWideString());
			fwprintf(stream, L"\t}\n");
		}

		for (iter=renderList.begin(); iter!=renderList.end(); ++iter) {
			taskStruct* task = *iter;	
			CString fileName = task->fileName;
			TaskType type = task->type;
			fileName = replaceSubString(fileName, L"\\", L"/");
			CString imageName = task->imageName;
			imageName = replaceSubString(imageName, L"\\", L"/");
			long slen = fileName.Length();
			CString frameNumber = subString(fileName, slen-5-g_FramePadding+1, slen-5);

			if (type != GlobalTask) continue;

			fwprintf(stream, CString(L"\tTask {" + task->name + L"} ").GetWideString());
			fwprintf(stream, L"-cmds {\n");
			fwprintf(stream, CString(L"\t\t" + alfcommand + L" {" + renderCommand + L" \"" + fileName + L"\"} " + services_tags + L"\n").GetWideString());
			fwprintf(stream, L"\t} ");
			fwprintf(stream, CString(L"-chaser {" + g_Imager + L" \"" + imageName + L"\"}").GetWideString());
			fwprintf(stream, L"\n");
		}

		long subTask = 1;
		for (iter=renderList.begin(); iter!=renderList.end(); ++iter) {
			taskStruct* task = *iter;	
			CString fileName = task->fileName;
			TaskType type = task->type;
			fileName = replaceSubString(fileName, L"\\", L"/");
			CString imageName = task->imageName;
			imageName = replaceSubString(imageName, L"\\", L"/");
			long slen = fileName.Length();
			CString frameNumber = CValue((long)task->frame).GetAsText();
			
			if (subTask == 1) {
				fwprintf(stream, CString(L"\tTask {Frame " + frameNumber + L"} ").GetWideString());
				if (type == SubTask) fwprintf(stream, L"-subtasks {\n");
				if (g_PreJobCommand != L"") fwprintf(stream, L"\t\tInstance {Pre-job command}\n");
				for (iiter=renderList.begin(); iiter!=renderList.end(); ++iiter) {
					taskStruct* gtask = *iiter;
					if(gtask->type == GlobalTask) {
						fwprintf(stream, CString(L"\t\tInstance {" + gtask->name + L"}\n").GetWideString());
					}
				}
			}

			if (type == SubTask) {
				fwprintf(stream, CString(L"\t\tTask {" + task->name + L"} ").GetWideString());
				fwprintf(stream, L"-cmds {\n");
				fwprintf(stream, CString(L"\t\t\t" + alfcommand + L" {" + renderCommand + L" \"" + fileName + L"\"} " + services_tags + L"\n").GetWideString());
				fwprintf(stream, L"\t\t}");
				if (g_Imager != L"") fwprintf(stream, CString(L" -chaser {" + g_Imager + L" \"" + imageName + L"\"}").GetWideString());
				fwprintf(stream, L"\n");
				subTask += 1;
			}
			else if (type == SuperTask) {
				if (subTask > 1) fwprintf(stream, L"\t} ");
				fwprintf(stream, L"-cmds {\n");
				if (g_PreFrameCommand != L"") {
					fwprintf(stream, CString(L"\t\tCmd {" + g_PreFrameCommand + L"} " + services_tags + L"\n").GetWideString());
				}
				fwprintf(stream, CString(L"\t\t" + alfcommand + L" {" + renderCommand + L" \"" + fileName + L"\"} " + services_tags + L"\n").GetWideString());
				fwprintf(stream, L"\t}");
				if (g_PostFrameCommand != L"") {
					fwprintf(stream, L" -cleanup {\n");
					fwprintf(stream, CString(L"\t\tCmd {" + g_PostFrameCommand + L"}\n").GetWideString());
					fwprintf(stream, L"\t}");
				}
				if (g_Imager != L"") fwprintf(stream, CString(L" -chaser {" + g_Imager + L" \"" + imageName + L"\"}").GetWideString());
				fwprintf(stream, L"\n");
				subTask = 1;
			}

		}
		fwprintf(stream, L"} ");

			
		fwprintf(stream, L"-cleanup {\n");

		// check for prejob script
		for (jter=cleanupList.begin(); jter!=cleanupList.end(); ++jter) {
			CString fileName = *jter;
			fileName = replaceSubString(fileName, L"\\", L"\\\\");
			if(stringHasChar(fileName, L"_prejob.bat") && !g_Cleanup) {
				CString command = cleanupCommand(fileName);
				fwprintf(stream, CString(L"\tCmd {" + command + L"}\n").GetWideString());
				break;
			}
		}

		if (g_Cleanup) {
			for (jter=cleanupList.begin(); jter!=cleanupList.end(); ++jter) {
				CString fileName = *jter;
				fileName = replaceSubString(fileName, L"\\", L"\\\\");
				long slen = fileName.Length();
				CString frameNumber = subString(fileName, slen-5-g_FramePadding, slen-6);
				CString command = cleanupCommand(fileName);
				fwprintf(stream, CString(L"\tCmd {" + command + L"}\n").GetWideString());
			}
		}
		if (g_PostJobCommand != L"") 
			fwprintf(stream, CString(L"\tCmd {" + g_PostJobCommand + L"}\n").GetWideString());
					
		// cleanup job script
		if (g_CleanJobScript == true) {
			CString fileName = jobScript;
			fileName = replaceSubString(fileName, L"\\", L"\\\\");
			long slen = fileName.Length();
			CString frameNumber = subString(fileName, slen-5-g_FramePadding, slen-6);
			CString command = cleanupCommand(fileName);
			fwprintf(stream, CString(L"\tCmd {" + command + L"}\n").GetWideString());
		}
		fwprintf(stream, L"}\n");

	}

	fclose(stream);

	return jobScript;
}

XSI::CString xsiRibTranslator::shellJobScript()
{
	FILE* stream;

	CString sceneName = stringAfterLastAppearance(m_app.GetActiveProject().GetFullName(), L"\\");

#ifdef WIN32		
	CString jobScript = g_JobPath + sceneName + L"_" + job->ribFileName + L"." + g_CameraName + L"." + CValue((long)g_CurrentFrame).GetAsText() + L".cmd"; 
#endif
#ifdef LINUX
	CString jobScript = g_JobPath + sceneName + L"_" + job->ribFileName + L"." + g_CameraName + L"." + CValue((long)g_CurrentFrame).GetAsText() + L".csh";
#endif

	// cleanup
	if (g_CleanJobScript == true) {
	    cleanupList.push_back(jobScript);
	}

	if ((stream = _wfopen(jobScript.GetWideString(), L"w")) == NULL)
		m_app.LogMessage(L"Could not create job script " + jobScript + L"!");
	else {		
		m_app.LogMessage(L"Executing " + jobScript + L"\n");

		fwprintf(stream, L"echo XSIMan 1.0 Job Script\n\n");

		vector<taskStruct*>::iterator iter;
		for (iter=renderList.begin(); iter!=renderList.end(); ++iter) {
			taskStruct* task = *iter;
			CString command = g_RenderCommand + L" \"" + task->fileName + L"\"\n";
			fwprintf(stream, command.GetWideString());
		}

		vector<CString>::iterator jter;
		for (jter=cleanupList.begin(); jter!=cleanupList.end(); ++jter) {
			CString fileName = *jter;
#ifdef WIN32
			CString command = L"del \"" + fileName + L"\"\n";
#endif
#ifdef LINUX
			CString command = L"/bin/rm \"" + fileName + L"\"\n";
#endif
			fwprintf(stream, command.GetWideString());
		}
	}

	fclose(stream);

	return jobScript;
}

CString xsiRibTranslator::constructFileName(bool ribFile, jobStruct* myjob)
{
	// frame padding
	wchar_t padframe[10];
    swprintf(padframe, L"%0*ld", g_FramePadding, (long)g_CurrentFrame);
	if (g_CurrentFrame < 0) {
		long currentFrame = abs((long)g_CurrentFrame);
		swprintf(padframe, L"-%0*ld", g_FramePadding, currentFrame);
	}

	// get current pass
	CValueArray args; CValue retval;
	m_app.ExecuteCommand(L"GetCurrentPass", args, retval);
	Pass currentPass = Pass(CRef(retval));

	CString fileExt, filePath, fileNameStem, jobName;
	if (ribFile) {
		fileExt = L".rib";
		filePath = g_RibPath;
		fileNameStem = myjob->ribFileName;
		if (myjob->jobType == BeautyJob || myjob->jobType == ShadowJob || myjob->jobType == InstanceJob) jobName = myjob->camera[0].name;
		else if (myjob->jobType == ArchiveJob || myjob->jobType == EnvmapJob || myjob->jobType == PhotonMapJob) jobName = myjob->name;
	}
	else {
		fileExt = myjob->fileExt;
		if (myjob->jobType == ShadowJob) filePath = g_ShdPath;
		else if (myjob->jobType == BeautyJob) filePath = g_PixPath;
		else if (myjob->jobType == EnvmapJob || myjob->jobType == PhotonMapJob) filePath = g_TexPath;
		fileNameStem = myjob->imageName;
		if (myjob->jobType == BeautyJob || myjob->jobType == ShadowJob) jobName = myjob->camera[0].name;
		else if (myjob->jobType == EnvmapJob || myjob->jobType == PhotonMapJob) jobName = myjob->name;
	}

	CString jobSuffix = L"";
	if (myjob->jobType == ShadowJob) jobSuffix = L".SHD";
	if (myjob->jobType == EnvmapJob) jobSuffix = L".ENV";
	if (myjob->jobType == PhotonMapJob) jobSuffix = L".PHT";
	if (myjob->jobType == InstanceJob) jobSuffix = L".INST";

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
	else if(myjob->jobType == PhotonMapJob) {
		if (myjob->writeFrequency == L"Every Frame") 
			fileName = filePath + fileNameStem + L"." + jobName + L"" + jobSuffix + L"." + padframe + fileExt;
		else if (myjob->writeFrequency == L"Once Per Job") 
			fileName = filePath + fileNameStem + L"." + jobName + L"" + jobSuffix + fileExt;
	}
	else if(myjob->jobType == InstanceJob) {
			fileName = filePath + fileNameStem + L"." + jobName + L"" + jobSuffix + L"." + padframe + fileExt;
	}
	else {
		if (myjob->writeFrequency == L"Every Frame") 
			fileName = filePath + fileNameStem + L"." + jobName + L"." + padframe + fileExt;
		else if (myjob->writeFrequency == L"Once Per Job") 
			fileName = filePath + fileNameStem + L"." + jobName + fileExt;
	}
	
	return fileName;
}

CString xsiRibTranslator::cleanupCommand(CString fileName) 
{
	CString cleanupCommand;

#ifdef WIN32
	if (g_UseJobScript) {
		cleanupCommand = L"cmd.exe /c del {" + fileName + L"}";
	}
	else {
		cleanupCommand = L"cmd.exe /c del \"" + fileName + L"\"";
	}
#endif
#ifdef LINUX
	cleanupCommand = L"/bin/rm \"" + fileName + L"\"";
#endif

	return cleanupCommand;
}

CString xsiRibTranslator::textureCommand(CString sourceFile, CString destFile)
{
	CString texmake;

	// get settings from XSIManOptions
	Property prop(m_sceneRoot.GetProperties().GetItem(L"XSIManOptions"));

	Parameter TexResize = prop.GetParameters().GetItem(L"TexResize");
	CString texResize = TexResize.GetValue(); 
	Parameter TexSMode = prop.GetParameters().GetItem(L"TexSMode");
	CString texSMode = TexSMode.GetValue(); 
	Parameter TexTMode = prop.GetParameters().GetItem(L"TexTMode");
	CString texTMode = TexTMode.GetValue(); 
	Parameter TexPattern = prop.GetParameters().GetItem(L"TexPattern");
	CString texPattern = TexPattern.GetValue();
	Parameter TexFormat = prop.GetParameters().GetItem(L"TexFormat");
	CString texFormat = TexFormat.GetValue();
	Parameter TexBytes = prop.GetParameters().GetItem(L"TexBytes");
	CString texBytes = TexBytes.GetValue();
	if (texBytes != L"") texBytes = L" -" + texBytes;
	Parameter TexFilter = prop.GetParameters().GetItem(L"TexFilter");
	CString texFilter = TexFilter.GetValue();
	Parameter TexFilterWidthS = prop.GetParameters().GetItem(L"TexFilterWidthS");
	long texFilterWidthS = (long)TexFilterWidthS.GetValue();
	Parameter TexFilterWidthT = prop.GetParameters().GetItem(L"TexFilterWidthT");
	long texFilterWidthT = (long)TexFilterWidthT.GetValue();

#ifdef WIN32
	//texmake = L"IF NOT EXIST \"" + destFile + L"\" ";
#endif
#ifdef LINUX
	//texmake = L"if [ ! -f \"" + destFile + L"\" ]; then ";
#endif

	if (g_Renderer == L"PRMan") {
		texmake = L"txmake -resize " + texResize + L" -smode " + texSMode + L" -tmode " + texTMode + 
			L" -pattern " + texPattern + L" -format " + texFormat + texBytes + L" -filter " + 
			texFilter + L" -sfilterwidth " + CValue(texFilterWidthS).GetAsText() + L" -tfilterwidth " + 
			CValue(texFilterWidthT).GetAsText() + L" \"" + sourceFile + L"\" \"" + destFile + L"\"";
	}
	else if (g_Renderer == L"3Delight") {
		texmake = L"tdlmake -smode " + texSMode + L" -tmode " + texTMode + L" -filter " + 
			texFilter + L" -sfilterwidth " + CValue(texFilterWidthS).GetAsText() + L" -tfilterwidth " + 
			CValue(texFilterWidthT).GetAsText() + L" \"" + sourceFile + L"\" \"" + destFile + L"\"";
	}
	else if (g_Renderer == L"Air") {
		if (texBytes == L"-byte") texBytes = L"-8";
		else if (texBytes == L"-short") texBytes = L"-16";
		else if (texBytes == L"-float") texBytes = L"-float";
		texmake = L"mktex -resize " + texResize + L" -smode " + texSMode + L" -tmode " + texTMode + texBytes +
			L" \"" + sourceFile + L"\" \"" + destFile + L"\"";
	}
	//else if (g_Renderer == L"RenderDotC") {
		//if (texBytes == L"-byte") texBytes = L"-1";
		//else if (texBytes == L"-short") texBytes = L"-2";
		//else if (texBytes == L"-word") texBytes = L"-4";
		//else if (texBytes == L"-float") texBytes = L"-f";
		//texmake = L"texdc -smode " + texSMode + L" -tmode " + texTMode + L" -pattern " + texPattern + texBytes +
		//	L" \"" + sourceFile + L"\" \"" + destFile + L"\"";
	//}

#ifdef LINUX
	texmake += L"; fi\n";
#endif

	return texmake;
}
	
CStatus xsiRibTranslator::rmanShaderParams(Shader shader, RtColor surfaceColor, RtColor surfaceOpacity, RtFloat& displacementBound) 
{
	CStatus stat = CStatus::OK;
	CString renderer;
	if (shader.HasRenderer(L"prman")) renderer = L"prman";
	else if (shader.HasRenderer(L"3delight")) renderer = L"3delight";
	else if (shader.HasRenderer(L"Air")) renderer = L"Air";
	CValue shaderType = shader.GetRendererOption(renderer, L"Type");

	CRefArray paramArray = shader.GetParameters();
	long numParam = paramArray.GetCount();

	for (long j=0; j<numParam; j++ ) {
		Parameter param = paramArray[j];
		CValue::DataType ptype = param.GetValueType();
		CString paramName = param.GetName();
		CValue paramValue = param.GetValue();

		float alphaChan;
		if (paramName == L"DisplacementBound") {
			if ((float)paramValue > displacementBound) displacementBound = (float)paramValue;
		}
		else if (paramName == L"Cs") {
			stat = shader.GetColorParameterValue(L"Cs", surfaceColor[0], surfaceColor[1], 
				surfaceColor[2], alphaChan, g_CurrentFrame);
		}
		else if (paramName == L"Os") {
			stat = shader.GetColorParameterValue(L"Os", surfaceOpacity[0], surfaceOpacity[1], 
				surfaceOpacity[2], alphaChan, g_CurrentFrame);
		}

		// determine parameter detail and type
		CValue paramDetailType = shader.GetRendererOption(renderer, paramName);
		wchar_t paramdetail[10];
		wchar_t paramtype[10];
		swscanf(paramDetailType.GetAsText().GetWideString(), L"%s %s", paramdetail, paramtype);
		CString paramType = CString(paramtype);
		CString paramDetail = CString(paramdetail);

		char* pname = NULL;
		XSIW2A(&pname, paramName.GetWideString());
		if (ptype == siFloat) {
			if (paramName != L"DisplacementBound") {
				xsiRibTokens tokenDataFloat;

				tokenDataFloat.put(pname, FloatType, false, false, false, 0);

				if (paramDetail == L"uniform") tokenDataFloat.putDetailType(UniformDetail);
				else if (paramDetail == L"varying") tokenDataFloat.putDetailType(VaryingDetail);
				else tokenDataFloat.putDetailType(UniformDetail);

				tokenDataFloat.assignFloat(0, (float)paramValue);

				if (shaderType == L"material") ribTokensSurf.push_back(tokenDataFloat);
				else if (shaderType == L"displace") ribTokensDisp.push_back(tokenDataFloat);
				else if (shaderType == L"volume") ribTokensVol.push_back(tokenDataFloat);
				else if (shaderType == L"interior") ribTokensInterior.push_back(tokenDataFloat);
				else if (shaderType == L"imager") ribTokensImager.push_back(tokenDataFloat);
			}
		}
		else if (ptype == siUnknown || ptype == siEmpty) {
			if (paramName != L"Cs" && paramName != L"Os") {
				xsiRibTokens tokenDataFloat;

				if (paramDetail == L"uniform") tokenDataFloat.putDetailType(UniformDetail);
				else if (paramDetail == L"varying") tokenDataFloat.putDetailType(VaryingDetail);
				else tokenDataFloat.putDetailType(UniformDetail);

				float floatParam[3] = {0.0, 0.0, 0.0};
				float matrixParam[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
								
				if (paramType == L"color") {
					stat = shader.GetColorParameterValue(paramName, floatParam[0], floatParam[1], 
						floatParam[2], alphaChan, g_CurrentFrame);
					tokenDataFloat.put(pname, ColorType, false, false, false, 0);
				}
				else if (paramType == L"point") {
					// this is a compound parameter
					getCompoundParameter(param, floatParam);
					tokenDataFloat.put(pname, PointType, false, false, false, 0);
				}							
				else if (paramType == L"vector") {
					// this is a compound parameter
					getCompoundParameter(param, floatParam);
					tokenDataFloat.put(pname, VectorType, false, false, false, 0);
				}
				else if (paramType == L"normal") {
					// this is a compound parameter
					getCompoundParameter(param, floatParam);
					tokenDataFloat.put(pname, NormalType, false, false, false, 0);
				}
				else if (paramType == L"matrix") {
					// this is a compound parameter
					getMatrixParameter(param, matrixParam);
					tokenDataFloat.put(pname, MatrixType, false, false, false, 0);
				}
				else {
					tokenDataFloat.put(pname, ColorType, false, false, false, 0);
				}

				if (paramType != L"matrix") {
					tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
				}
				else {
					tokenDataFloat.assignFloat(0, matrixParam[0], matrixParam[1], matrixParam[2], matrixParam[3], 
						matrixParam[4], matrixParam[5], matrixParam[6], matrixParam[7],
						matrixParam[8], matrixParam[9], matrixParam[10], matrixParam[11],
						matrixParam[12], matrixParam[13], matrixParam[14], matrixParam[15]);
				}
									
				if (shaderType == L"material") ribTokensSurf.push_back(tokenDataFloat);
				else if (shaderType == L"displace") ribTokensDisp.push_back(tokenDataFloat);
				else if (shaderType == L"volume") ribTokensVol.push_back(tokenDataFloat);
				else if (shaderType == L"interior") ribTokensInterior.push_back(tokenDataFloat);
				else if (shaderType == L"imager") ribTokensImager.push_back(tokenDataFloat);
			}
		}
		else if (ptype == siString) {
			if (paramName != L"Name") {
				xsiRibTokens tokenDataString;

				tokenDataString.put(pname, StringType, false, false, false, 0);

				if (paramDetail == L"uniform") tokenDataString.putDetailType(UniformDetail);
				else if (paramDetail == L"varying") tokenDataString.putDetailType(VaryingDetail);
				else tokenDataString.putDetailType(UniformDetail);
							
				CString nuStringName = paramValue.GetAsText();

				// substitute global vars
				nuStringName = subGlobalVars(nuStringName);

				// replace object name as well	
				if (stringHasChar(nuStringName, L"$OBJ")) {
					nuStringName = replaceSubString(nuStringName, L"$OBJ", g_CurrentNode);
				}

				// we seem to need to replace back slashes
				nuStringName = replaceSubString(nuStringName, L"\\", L"/");

				unsigned int strlen = nuStringName.Length();
				char* str = NULL;
				XSIW2A(&str, nuStringName.GetWideString());
				tokenDataString.assignString(str, strlen);

				if (shaderType == L"material") ribTokensSurf.push_back(tokenDataString);
				else if (shaderType == L"displace") ribTokensDisp.push_back(tokenDataString);
				else if (shaderType == L"volume") ribTokensVol.push_back(tokenDataString);
				else if (shaderType == L"interior") ribTokensInterior.push_back(tokenDataString);
				else if (shaderType == L"imager") ribTokensImager.push_back(tokenDataString);
			}
		}
	}

	return stat;
}

// compile shader
void xsiRibTranslator::compileShader(CString displaceShaderName) 
{
	CString shaderPath = g_ShaderPath + L"/" + displaceShaderName + L".sl";
	CString shaderCommand = L"shader ";
							
	// get XSIMAN environment variable
	wchar_t* xsimanhome;
	xsimanhome = _wgetenv(L"XSIMAN");
	CString funcPath = CString(xsimanhome);
	if (funcPath == L"") m_app.LogMessage(L"Warning: XSIMAN directory not set!");
	funcPath += L"/shaders/src";
	funcPath = L"\"" + replaceSubString(funcPath, L"\\", L"/") + L"\"";
	shaderPath = L"\"" + replaceSubString(shaderPath, L"\\", L"/") + L"\"";

	CString mkshader, sloPath;
	if (g_Renderer == L"PRMan") {
		shaderCommand = L"shader -I" + funcPath;
		sloPath = L"\"" + replaceSubString(g_ShaderPath, L"\\", L"/") + L"/" + displaceShaderName + L".slo" + L"\"";
		mkshader = shaderCommand + L" -o " + sloPath + L" " + shaderPath;
	}
	else if (g_Renderer == L"3Delight") {
		shaderCommand = L"shaderdl -I" + funcPath;
		sloPath = L"\"" + replaceSubString(g_ShaderPath, L"\\", L"/") + L"\"";
		mkshader = shaderCommand + L" -d " + sloPath + L" " + shaderPath;
	}
	else if (g_Renderer == L"Air") {
		shaderCommand = L"shaded -I" + funcPath;
		sloPath = L"\"" + replaceSubString(g_ShaderPath, L"\\", L"/") + L"/" + displaceShaderName + L".slb" + L"\"";
		mkshader = shaderCommand + L" -DAir -o " + sloPath + L" " + shaderPath;
	}
						
	//execute(mkshader);
	prejobList.push_back(mkshader);
}

// make sure this shader argument is valid
CStatus xsiRibTranslator::shaderArgIsValid(shaderTemplateItem stitem) 
{
	CStatus stat = CStatus::Fail;
	CString shaderFunction;
	map<const wchar_t*, wchar_t*, ltwstr>::iterator cur;
	
	cur = shaderFuncs.find(stitem.shader.GetWideString());
	if (CString((*cur).first) == CString(stitem.shader))
		shaderFunction = (*cur).second;

	// this crashes XSI for some reason
	map<const wchar_t*, wchar_t*, ltwstr>::iterator jter;
	//jter = shaderArgTypes.find(CString(shaderFunction + L"_" + stitem.param).GetWideString());
	//if (CString((*jter).first) == (shaderFunction + L"_" + stitem.param))
	//	stat = CStatus::OK;
	
	for (jter=shaderArgTypes.begin(); jter!=shaderArgTypes.end(); jter++) {
		if (CString((*jter).first) == (shaderFunction + L"_" + stitem.param)) {
			stat = CStatus::OK;
			break;
		}
	}

	return stat;
}

CStatus xsiRibTranslator::shaderFuncIsValid(CString shaderFunction) 
{	
	CStatus stat = CStatus::Fail;

	map<const wchar_t*, wchar_t*, ltwstr>::iterator cur = shaderFuncs.find(shaderFunction.GetWideString());

	if (CString((*cur).first) == shaderFunction)
		stat = CStatus::OK;	

	return stat;
}

CString xsiRibTranslator::getParamValue(Parameter param)
{
	CString paramValue;

	CValue::DataType ptype = param.GetValueType();
	if (ptype == siFloat || ptype == siDouble || ptype == siInt2 || ptype == siInt4 || ptype == siBool) {
		paramValue = param.GetValue();

		// check for commas in float values (non-English versions of XP use commas as decimal places)
		paramValue = replaceSubString(paramValue, L",", L".");
	}
	else if (ptype == siString) {
		CString paramValue = param.GetValue();
		paramValue = L"\"" + paramValue + L"\"";
						
		// substitute global vars
		paramValue = subGlobalVars(paramValue);
						
		// replace object name as well	
		if (stringHasChar(paramValue, L"$OBJ")) {
			paramValue = replaceSubString(paramValue, L"$OBJ", g_CurrentNode);
		}
				
		// we seem to need to replace back slashes
		paramValue = replaceSubString(paramValue, L"\\", L"/");
	} 
	else if (ptype == siUnknown || ptype == siEmpty) {	// could be color, vector or matrix
		float rgb[3] = {-INF, -INF, -INF};
		float vec3[3] = {-INF, -INF, -INF};
		CParameterRefArray compoundParameterRefArray(param.GetParameters());
		for(long k = 0; k < compoundParameterRefArray.GetCount(); k++) {
			Parameter compoundparam(compoundParameterRefArray[k]);
			if (compoundparam.GetName() == L"x") vec3[0] = compoundparam.GetValue();
			else if (compoundparam.GetName() == L"y") vec3[1] = compoundparam.GetValue();
			else if (compoundparam.GetName() == L"z") vec3[2] = compoundparam.GetValue();
			if (compoundparam.GetName() == L"red") rgb[0] = compoundparam.GetValue();
			else if (compoundparam.GetName() == L"green") rgb[1] = compoundparam.GetValue();
			else if (compoundparam.GetName() == L"blue") rgb[2] = compoundparam.GetValue();
		}
				
		if (rgb[0] != -INF) 
			paramValue = L"color (" + CValue(rgb[0]).GetAsText() + L"," + CValue(rgb[1]).GetAsText() + L"," + CValue(rgb[2]).GetAsText() + L")";
		else if (vec3[0] != -INF) 
			paramValue = L"vector (" + CValue(vec3[0]).GetAsText() + L"," + CValue(vec3[1]).GetAsText() + L"," + CValue(vec3[2]).GetAsText() + L")";

	}
	else paramValue = L"";

	return paramValue;
}

bool xsiRibTranslator::globalEnvironmentParam(CString globalEnvParam)
{
	bool retval = false;
	wchar_t* envParams[7] = {L"material_phong_environment", L"material_blinn_environment", L"material_lambert_environment", 
		L"material_cooktorr_environment", L"material_ward_environment", L"material_constant_environment", 
		L"material_strauss_environment"};

	for (long i=0; i<7; i++) {
		if (globalEnvParam == CString(envParams[i])) {
			retval = true;
			break;
		}
	}
	return retval;
}

CString xsiRibTranslator::getArgType(CString paramName)
{
	CString argType;
	map<const wchar_t*, wchar_t*, ltwstr>::iterator iter;
	//for (iter=shaderArgTypes.begin(); iter!=shaderArgTypes.end(); iter++) {
	//	if (CString((*iter).first) == paramName) {
	//		argType = (*iter).second;
	//		break;
	//	}
	//}
	//m_app.LogMessage(L"DEBUG2: " + paramName);
		
	iter = shaderArgTypes.find(paramName.GetWideString());
	if (CString((*iter).first) == paramName)
		argType = (*iter).second;

	return argType;
}

bool xsiRibTranslator::shaderExists(CString shaderName)
{
	bool exists = false;
	
	vector<CString>::iterator iter;
	for (iter=shaderList.begin(); iter!=shaderList.end(); iter++) {
		if (CString(*iter) == shaderName) {
			exists = true;
			break;
		}
	}

	return exists;
}

// add shader param list to token list
void xsiRibTranslator::addShaderParamToTokenList(CString shaderName, CString shaderType)
{
	vector<shaderParamStruct>::iterator iter;
	iter = shaderParamList.begin();	
	while (iter != shaderParamList.end()) {
		shaderParamStruct spitem = *iter; 
		iter++;	

		if (spitem.shaderName == shaderName) {
			char* pname = NULL;
			XSIW2A(&pname, spitem.paramName.GetWideString());
			char* pvalue = NULL;
			XSIW2A(&pvalue, spitem.paramValue.GetWideString());
			long strlen = spitem.paramValue.Length();

			if (spitem.argType == L"float") {		
				xsiRibTokens tokenDataFloat;
				tokenDataFloat.put(pname, FloatType, false, false, false, 0);
				tokenDataFloat.putDetailType(UniformDetail);
				tokenDataFloat.assignFloat(0, (float)atof(pvalue));
				if (shaderType == L"surface") ribTokensSurf.push_back(tokenDataFloat);
				if (shaderType == L"displacement") ribTokensDisp.push_back(tokenDataFloat);
				if (shaderType == L"volume") ribTokensVol.push_back(tokenDataFloat);
			}
			else if (spitem.argType == L"color") {
				float floatParam[3] = {0.0, 0.0, 0.0};
				xsiRibTokens tokenDataFloat;
				tokenDataFloat.putDetailType(UniformDetail);
				swscanf(spitem.paramValue.GetWideString(), L"color (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
				tokenDataFloat.put(pname, ColorType, false, false, false, 0);							
				tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
				if (shaderType == L"surface") ribTokensSurf.push_back(tokenDataFloat);
				if (shaderType == L"displacement") ribTokensDisp.push_back(tokenDataFloat);
				if (shaderType == L"volume") ribTokensVol.push_back(tokenDataFloat);									
			}
			else if (spitem.argType == L"vector") {
				float floatParam[3] = {0.0, 0.0, 0.0};
				xsiRibTokens tokenDataFloat;
				tokenDataFloat.putDetailType(UniformDetail);
				swscanf(spitem.paramValue.GetWideString(), L"vector (%f,%f,%f)", &floatParam[0], &floatParam[1], &floatParam[2]);
				tokenDataFloat.put(pname, VectorType, false, false, false, 0);							
				tokenDataFloat.assignFloat(0, floatParam[0], floatParam[1], floatParam[2]);
				if (shaderType == L"surface") ribTokensSurf.push_back(tokenDataFloat);
				if (shaderType == L"displacement") ribTokensDisp.push_back(tokenDataFloat);
				if (shaderType == L"volume") ribTokensVol.push_back(tokenDataFloat);
			}
			else if (spitem.argType == L"string") {
				xsiRibTokens tokenDataString;
				tokenDataString.put(pname, StringType, false, false, false, 0);
				tokenDataString.putDetailType(UniformDetail);
				tokenDataString.assignString(pvalue, strlen);
				if (shaderType == L"surface") ribTokensSurf.push_back(tokenDataString);
				if (shaderType == L"displacement") ribTokensDisp.push_back(tokenDataString);
				if (shaderType == L"volume") ribTokensVol.push_back(tokenDataString);
			}
		}
	}
}

CString xsiRibTranslator::subGlobalVars(CString oldString)
{
	CString newString = oldString;

	// we also replace # & $F4 with padded numbers and @ & $F with unpadded numbers
	wchar_t padframe[10];
	if (stringHasChar(newString, L"#")) {
		swprintf(padframe, L"%0*ld", 4, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"#", CString(padframe));
	}
	else if (stringHasChar(newString, L"@")) {
		swprintf(padframe, L"%0*ld", 1, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"@", CString(padframe));
	}
	if (stringHasChar(newString, L"$F4")) {
		swprintf(padframe, L"%0*ld", 4, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"$F4", CString(padframe));
	}
	else if (stringHasChar(newString, L"$F")) {
		swprintf(padframe, L"%0*ld", 1, (long)g_CurrentFrame);
		newString = replaceSubString(newString, L"$F", CString(padframe));
	}							
						
	// check for additional keywords ($PROJ, $TEX, $SHD, $PIX, $RIB)
	if (stringHasChar(newString, L"$PROJ")) {
		CString projPath = m_app.GetActiveProject().GetFullName(); 
		newString = replaceSubString(newString, L"$PROJ", projPath);
	}
		
	if (stringHasChar(newString, L"$TEX")) {
		newString = replaceSubString(newString, L"$TEX", g_TexPath);
	}
														
	if (stringHasChar(newString, L"$SHD")) {
		newString = replaceSubString(newString, L"$SHD", g_ShdPath);
	}
														
	if (stringHasChar(newString, L"$PIX")) {
		newString = replaceSubString(newString, L"$PIX", g_PixPath);
	}
														
	if (stringHasChar(newString, L"$RIB")) {
		newString = replaceSubString(newString, L"$RIB", g_RibPath);
	}

	return newString;
}

void xsiRibTranslator::addDefaultMaterial(XSI::X3DObject& x3dobject, nodeStruct node)
{
	Property propRef;

	// get shader info
	bool hasSurfaceShader = false;
	bool hasDisplacementShader = false;
	bool hasVolumeShader = false;
	bool hasInteriorShader = false;
	bool hasImagerShader = false;
	CString surfaceShaderName, displaceShaderName, volumeShaderName, interiorShaderName, imagerShaderName;
	RtColor surfaceColor = {1.0, 1.0, 1.0}; 
	RtColor surfaceOpacity = {1.0, 1.0, 1.0};
	RtFloat displacementBound = 0.0;

	// there may be polygon clusters with a material
	Material material = x3dobject.GetMaterial();

	CRefArray shaderArray = material.GetShaders();
	for (long i=0; i<shaderArray.GetCount(); i++ ) {
		Shader shader = shaderArray[i];
		bool rmanShader = shader.HasRenderer(L"prman") || shader.HasRenderer(L"3delight") || shader.HasRenderer(L"Air");

		if (rmanShader) {
			CString renderer;
			if (shader.HasRenderer(L"prman")) renderer = L"prman";
			else if (shader.HasRenderer(L"3delight")) renderer = L"3delight";
			else if (shader.HasRenderer(L"Air")) renderer = L"Air";

			CValue shaderType = shader.GetRendererOption(renderer, L"Type");
			if (shaderType == L"material") {
				hasSurfaceShader = true;
				surfaceShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"displace") {
				hasDisplacementShader = true;
				displaceShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"volume") {
				hasVolumeShader = true;
				volumeShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"interior") {
				hasInteriorShader = true;
				interiorShaderName = shader.GetCodePath(renderer);
			}
			else if (shaderType == L"imager") {
				hasImagerShader = true;
				imagerShaderName = shader.GetCodePath(renderer);
			}
			rmanShaderParams(shader, surfaceColor, surfaceOpacity, displacementBound);
		}
	}
		
	// check if using Air
	propRef = node.sceneItem.GetProperties().GetItem(L"XSIManAirOptions");
	bool trueDisplacement = false;
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter truedisplacement = prop.GetParameters().GetItem(L"truedisplacement");
		trueDisplacement = truedisplacement.GetValue();
	}

	if (g_Renderer == L"Air" && trueDisplacement == true) {
		RtInt truedisp = 1;
		RiAttribute("render", "integer truedisplacement", &truedisp, RI_NULL);
	}

	// check if using 3Delight
	propRef = node.sceneItem.GetProperties().GetItem(L"XSIManDelightOptions");
	bool subSurface = false;
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter subsurface = prop.GetParameters().GetItem(L"subsurface");
		subSurface = subsurface.GetValue();
		Parameter ssgroup = prop.GetParameters().GetItem(L"ssGroup");
		CString ssGroup = ssgroup.GetValue();
		Parameter sssRed = prop.GetParameters().GetItem(L"sssred");
		float sssred = (float)sssRed.GetValue();
		Parameter sssGreen = prop.GetParameters().GetItem(L"sssgreen");
		float sssgreen = (float)sssGreen.GetValue();
		Parameter sssBlue = prop.GetParameters().GetItem(L"sssblue");
		float sssblue = (float)sssBlue.GetValue();
		Parameter ssaRed = prop.GetParameters().GetItem(L"ssared");
		float ssared = (float)ssaRed.GetValue();
		Parameter ssaGreen = prop.GetParameters().GetItem(L"ssagreen");
		float ssagreen = (float)ssaGreen.GetValue();
		Parameter ssaBlue = prop.GetParameters().GetItem(L"ssablue");
		float ssablue = (float)ssaBlue.GetValue();
		Parameter ssrefractionindex = prop.GetParameters().GetItem(L"ssRefractionIndex");
		float ssRefractionIndex = (float)ssrefractionindex.GetValue();
		Parameter ssscale = prop.GetParameters().GetItem(L"ssScale");
		float ssScale = (float)ssscale.GetValue();
		Parameter ssshadingrate = prop.GetParameters().GetItem(L"ssShadingRate");
		float ssShadingRate = (float)ssshadingrate.GetValue();

		if (g_Renderer == L"3Delight" && subSurface == true) {
			char* group = NULL;
			XSIW2A(&group, ssGroup.GetWideString());
			RiArchiveRecord(RI_VERBATIM, "Attribute \"visibility\"  \"subsurface\" \"%s\"\n", group);
			RiArchiveRecord(RI_VERBATIM, "Attribute \"subsurface\" \"color scattering\" [%f %f %f] \"color absorption\" [%f %f %f] \"refractionindex\" %f \"scale\" %f \"shadingrate\" %f\n", sssred, sssgreen, sssblue, ssared, ssagreen, ssablue, ssRefractionIndex, ssScale, ssShadingRate);
		}
	}

	// put photon shading model stuff here
	propRef = node.sceneItem.GetProperties().GetItem(L"XSIManShadingModel");
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter ShadingModel = prop.GetParameters().GetItem(L"ShadingModel");
		CString shadingModel = ShadingModel.GetValue();
		if (shadingModel != L"") {
			char* shadingmodel = NULL;
			XSIW2A(&shadingmodel, shadingModel.GetWideString());
			RiAttribute("photon", "shadingmodel", &shadingmodel, RI_NULL);
		}
	}

	// get any displacement bound settings and Cs & Os
	propRef = x3dobject.GetProperties().GetItem(L"XSIManMiscOptions");
	if (propRef.IsValid()) {
		Property prop = propRef;

		Parameter surfacecolor = prop.GetParameters().GetItem(L"surfacecolor");
		if (surfacecolor.GetValue() == true) {
			Parameter cred = prop.GetParameters().GetItem(L"cred");
			Parameter cgreen = prop.GetParameters().GetItem(L"cgreen");
			Parameter cblue = prop.GetParameters().GetItem(L"cblue");
			surfaceColor[0] = cred.GetValue();
			surfaceColor[1] = cgreen.GetValue();
			surfaceColor[2] = cblue.GetValue();
		}

		Parameter surfaceopacity = prop.GetParameters().GetItem(L"surfaceopacity");
		if (surfaceopacity.GetValue() == true) {
			Parameter ored = prop.GetParameters().GetItem(L"ored");
			Parameter ogreen = prop.GetParameters().GetItem(L"ogreen");
			Parameter oblue = prop.GetParameters().GetItem(L"oblue");
			surfaceOpacity[0] = ored.GetValue();
			surfaceOpacity[1] = ogreen.GetValue();
			surfaceOpacity[2] = oblue.GetValue();
		}

		Parameter displacementbound_flag = prop.GetParameters().GetItem(L"displacementbound_flag");
		if (displacementbound_flag.GetValue()) {
			Parameter displacementbound = prop.GetParameters().GetItem(L"displacementbound");
			displacementBound = (float)displacementbound.GetValue();
		}
	}

	// displacement bound
	if (displacementBound > 0.0) {
		RiAttribute("displacementbound", "sphere", &displacementBound, RI_NULL);
	}

	// get shadow shading mode 
	propRef = node.sceneItem.GetProperties().GetItem(L"XSIManShadowShading");
	CString shadowRIBShading = L"opaque";
	if (propRef.IsValid()) {
		Property prop = propRef;
		Parameter shadowShading = prop.GetParameters().GetItem(L"ShadowShading");
			shadowRIBShading = shadowShading.GetValue();
	}

	// surface color and opacity
	RiColor(surfaceColor);
	if (job->jobType != ShadowJob) {
		RiOpacity(surfaceOpacity);
	}
	else if (job->jobType == ShadowJob && (shadowRIBShading == L"Os" || shadowRIBShading == L"shade")) {
		RiOpacity(surfaceOpacity);
	}

	// update texture maps
	updateTextureMaps(material);

	// shaders
	if (job->jobType == ShadowJob && (shadowRIBShading == L"opaque" || shadowRIBShading == L"Os")) {
		RiSurface("matte", RI_NULL);
	}
	else if (hasSurfaceShader) {
		unsigned numTokens = ribTokensSurf.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
		char* surfaceName = NULL;
		XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
		RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
		ribTokensSurf.clear();
	}
	else {
		if(material.IsValid()) {
			if (g_RenderTreeSupport == true) {
				if (getSurfaceShader(material, node.sceneItem.GetName(), surfaceShaderName) == CStatus::OK) {
					// if object is nurb, invert t coordinate
					if (node.objectType == NurbsObject) {
						xsiRibTokens tokenDataFloat;
						tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
						tokenDataFloat.putDetailType(UniformDetail);
						tokenDataFloat.assignFloat(0, 1);
						ribTokensSurf.push_back(tokenDataFloat);
					}

					// add global illumination paramaters
					if (surfaceShaderName != L"xsimanParticle") 
						addGlobalIllumParams(ribTokensSurf);
										
					unsigned numTokens = ribTokensSurf.size();
					RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
					RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
					assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
					char* surfaceName = NULL;
					XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
					RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
					ribTokensSurf.clear();
				}
			}
			else if (g_RenderTreeSupport == false) {
				if (builtInShader(material, surfaceShaderName) == CStatus::OK) {
					// if object is nurb, invert t coordinate
					if (node.objectType == NurbsObject) {
						xsiRibTokens tokenDataFloat;
						tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
						tokenDataFloat.putDetailType(UniformDetail);
						tokenDataFloat.assignFloat(0, 1);
						ribTokens.push_back(tokenDataFloat);
					}
						
					// add global illumination paramaters
					if (surfaceShaderName != L"xsimanParticle") 
						addGlobalIllumParams(ribTokens);

					unsigned numTokens = ribTokens.size();
					RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
					RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
					assignRibTokensV(&ribTokens, tokenArray, pointerArray);
					char* surfaceName = NULL;
					XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
					RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
					ribTokens.clear();
				}
			}
			else RiSurface("matte", RI_NULL);
		}
	} 

	if (hasDisplacementShader) {
		unsigned numTokens = ribTokensDisp.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensDisp, tokenArray, pointerArray);
		char* displaceName = NULL;
		XSIW2A(&displaceName, displaceShaderName.GetWideString());
		RiDisplacementV(displaceName, numTokens, tokenArray, pointerArray);
		ribTokensDisp.clear();
	}
	else {
		if(material.IsValid()) {
			if (g_RenderTreeSupport == true) {

				if (getDisplacementShader(material, node.sceneItem.GetName(), displaceShaderName) == CStatus::OK) {

					// go ahead and put a default bound number if it is undefined
					if (displacementBound == 0.0) {
						displacementBound = 1.0;
						RiAttribute("displacementbound", "sphere", &displacementBound, RI_NULL);
					}
		
					// if object is nurb, invert t coordinate
					if (node.objectType == NurbsObject) {
						xsiRibTokens tokenDataFloat;
						tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
						tokenDataFloat.putDetailType(UniformDetail);
						tokenDataFloat.assignFloat(0, 1);
						ribTokensDisp.push_back(tokenDataFloat);
					}

					unsigned numTokens = ribTokensDisp.size();
					RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
					RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
					assignRibTokensV(&ribTokensDisp, tokenArray, pointerArray);
					char* displaceName = NULL;
					XSIW2A(&displaceName, displaceShaderName.GetWideString());
					RiDisplacementV(displaceName, numTokens, tokenArray, pointerArray);
					ribTokensDisp.clear();
				}
			}
		}
	}

	if (hasVolumeShader && (job->jobType != ShadowJob || (job->jobType == ShadowJob && shadowRIBShading == L"shade"))) {
		unsigned numTokens = ribTokensVol.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensVol, tokenArray, pointerArray);
		char* volumeName = NULL;
		XSIW2A(&volumeName, volumeShaderName.GetWideString());
		RiAtmosphereV(volumeName, numTokens, tokenArray, pointerArray);
		ribTokensVol.clear();
	}
	else if (job->jobType != ShadowJob || (job->jobType == ShadowJob && shadowRIBShading == L"shade")) {
		if(material.IsValid()) {
			if (g_RenderTreeSupport == true) {

				if (getVolumeShader(material, node.sceneItem.GetName(), volumeShaderName) == CStatus::OK) {	

					// if object is nurb, invert t coordinate
					if (node.objectType == NurbsObject) {
						xsiRibTokens tokenDataFloat;
						tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
						tokenDataFloat.putDetailType(UniformDetail);
						tokenDataFloat.assignFloat(0, 1);
						ribTokensVol.push_back(tokenDataFloat);
					}

					unsigned numTokens = ribTokensVol.size();
					RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
					RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
					assignRibTokensV(&ribTokensVol, tokenArray, pointerArray);
					char* volumeName = NULL;
					XSIW2A(&volumeName, volumeShaderName.GetWideString());
					RiAtmosphereV(volumeName, numTokens, tokenArray, pointerArray);
					ribTokensVol.clear();
				}
			}
		}
	}

	if (hasInteriorShader && (job->jobType != ShadowJob || (job->jobType == ShadowJob && shadowRIBShading == L"shade"))) {
		unsigned numTokens = ribTokensInterior.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensInterior, tokenArray, pointerArray);
		char* interiorName = NULL;
		XSIW2A(&interiorName, interiorShaderName.GetWideString());
		RiInteriorV(interiorName, numTokens, tokenArray, pointerArray);
		ribTokensInterior.clear();
	}

	if (hasImagerShader && job->jobType == BeautyJob) {
		unsigned numTokens = ribTokensImager.size();
		RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
		RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
		assignRibTokensV(&ribTokensImager, tokenArray, pointerArray);
		char* imagerName = NULL;
		XSIW2A(&imagerName, imagerShaderName.GetWideString());
		RiImagerV(imagerName, numTokens, tokenArray, pointerArray);
		ribTokensImager.clear();
	}
}

void xsiRibTranslator::addClusterMaterials(XSI::X3DObject& x3dobject, nodeStruct node)
{
	Property propRef;

	// get shader info
	bool hasSurfaceShader = false;
	bool hasDisplacementShader = false;
	bool hasVolumeShader = false;
	bool hasInteriorShader = false;
	bool hasImagerShader = false;
	CString surfaceShaderName, displaceShaderName, volumeShaderName, interiorShaderName, imagerShaderName;
	RtColor surfaceColor = {1.0, 1.0, 1.0}; 
	RtColor surfaceOpacity = {1.0, 1.0, 1.0};
	RtFloat displacementBound = 0.0;

	CRefArray clusterRefArray;
	Geometry geom(x3dobject.GetActivePrimitive().GetGeometry(g_CurrentFrame));
	geom.GetClusters().Filter(siPolygonCluster, CStringArray(), L"", clusterRefArray);
			
	int numMaterials = clusterRefArray.GetCount();

	for (long j = 0; j < numMaterials; ++j) {
		if (node.objectType == SubdivisionObject) {
			RiAttributeBegin();
		}

		Cluster cluster(clusterRefArray[j]);
		Material material = cluster.GetMaterial();

		CRefArray shaderArray = material.GetShaders();
		for (long i=0; i<shaderArray.GetCount(); i++ ) {
			Shader shader = shaderArray[i];
			bool rmanShader = shader.HasRenderer(L"prman") || shader.HasRenderer(L"3delight") || shader.HasRenderer(L"Air");

			if (rmanShader) {
				CString renderer;
				if (shader.HasRenderer(L"prman")) renderer = L"prman";
				else if (shader.HasRenderer(L"3delight")) renderer = L"3delight";
				else if (shader.HasRenderer(L"Air")) renderer = L"Air";

				CValue shaderType = shader.GetRendererOption(renderer, L"Type");
				if (shaderType == L"material") {
					hasSurfaceShader = true;
					surfaceShaderName = shader.GetCodePath(renderer);
				}
				else if (shaderType == L"displace") {
					hasDisplacementShader = true;
					displaceShaderName = shader.GetCodePath(renderer);
				}
				else if (shaderType == L"volume") {
					hasVolumeShader = true;
					volumeShaderName = shader.GetCodePath(renderer);
				}
				else if (shaderType == L"interior") {
					hasInteriorShader = true;
					interiorShaderName = shader.GetCodePath(renderer);
				}
				else if (shaderType == L"imager") {
					hasImagerShader = true;
					imagerShaderName = shader.GetCodePath(renderer);
				}
				rmanShaderParams(shader, surfaceColor, surfaceOpacity, displacementBound);
			}
		}
		
		// check if using Air
		propRef = node.sceneItem.GetProperties().GetItem(L"XSIManAirOptions");
		bool trueDisplacement = false;
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter truedisplacement = prop.GetParameters().GetItem(L"truedisplacement");
			trueDisplacement = truedisplacement.GetValue();
		}

		if (g_Renderer == L"Air" && trueDisplacement == true) {
			RtInt truedisp = 1;
			RiAttribute("render", "integer truedisplacement", &truedisp, RI_NULL);
		}

		// check if using 3Delight
		propRef = node.sceneItem.GetProperties().GetItem(L"XSIManDelightOptions");
		bool subSurface = false;
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter subsurface = prop.GetParameters().GetItem(L"subsurface");
			subSurface = subsurface.GetValue();
			Parameter ssgroup = prop.GetParameters().GetItem(L"ssGroup");
			CString ssGroup = ssgroup.GetValue();
			Parameter sssRed = prop.GetParameters().GetItem(L"sssred");
			float sssred = (float)sssRed.GetValue();
			Parameter sssGreen = prop.GetParameters().GetItem(L"sssgreen");
			float sssgreen = (float)sssGreen.GetValue();
			Parameter sssBlue = prop.GetParameters().GetItem(L"sssblue");
			float sssblue = (float)sssBlue.GetValue();
			Parameter ssaRed = prop.GetParameters().GetItem(L"ssared");
			float ssared = (float)ssaRed.GetValue();
			Parameter ssaGreen = prop.GetParameters().GetItem(L"ssagreen");
			float ssagreen = (float)ssaGreen.GetValue();
			Parameter ssaBlue = prop.GetParameters().GetItem(L"ssablue");
			float ssablue = (float)ssaBlue.GetValue();
			Parameter ssrefractionindex = prop.GetParameters().GetItem(L"ssRefractionIndex");
			float ssRefractionIndex = (float)ssrefractionindex.GetValue();
			Parameter ssscale = prop.GetParameters().GetItem(L"ssScale");
			float ssScale = (float)ssscale.GetValue();
			Parameter ssshadingrate = prop.GetParameters().GetItem(L"ssShadingRate");
			float ssShadingRate = (float)ssshadingrate.GetValue();

			if (g_Renderer == L"3Delight" && subSurface == true) {
				char* group = NULL;
				XSIW2A(&group, ssGroup.GetWideString());
				RiArchiveRecord(RI_VERBATIM, "Attribute \"visibility\"  \"subsurface\" \"%s\"\n", group);
				RiArchiveRecord(RI_VERBATIM, "Attribute \"subsurface\" \"color scattering\" [%f %f %f] \"color absorption\" [%f %f %f] \"refractionindex\" %f \"scale\" %f \"shadingrate\" %f\n", sssred, sssgreen, sssblue, ssared, ssagreen, ssablue, ssRefractionIndex, ssScale, ssShadingRate);
			}
		}

		// put photon shading model stuff here
		propRef = node.sceneItem.GetProperties().GetItem(L"XSIManShadingModel");
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter ShadingModel = prop.GetParameters().GetItem(L"ShadingModel");
			CString shadingModel = ShadingModel.GetValue();
			if (shadingModel != L"") {
				char* shadingmodel = NULL;
				XSIW2A(&shadingmodel, shadingModel.GetWideString());
				RiAttribute("photon", "shadingmodel", &shadingmodel, RI_NULL);
			}
		}

		// get any displacement bound settings and Cs & Os
		propRef = x3dobject.GetProperties().GetItem(L"XSIManMiscOptions");
		if (propRef.IsValid()) {
			Property prop = propRef;

			Parameter surfacecolor = prop.GetParameters().GetItem(L"surfacecolor");
			if (surfacecolor.GetValue() == true) {
				Parameter cred = prop.GetParameters().GetItem(L"cred");
				Parameter cgreen = prop.GetParameters().GetItem(L"cgreen");
				Parameter cblue = prop.GetParameters().GetItem(L"cblue");
				surfaceColor[0] = cred.GetValue();
				surfaceColor[1] = cgreen.GetValue();
				surfaceColor[2] = cblue.GetValue();
			}

			Parameter surfaceopacity = prop.GetParameters().GetItem(L"surfaceopacity");
			if (surfaceopacity.GetValue() == true) {
				Parameter ored = prop.GetParameters().GetItem(L"ored");
				Parameter ogreen = prop.GetParameters().GetItem(L"ogreen");
				Parameter oblue = prop.GetParameters().GetItem(L"oblue");
				surfaceOpacity[0] = ored.GetValue();
				surfaceOpacity[1] = ogreen.GetValue();
				surfaceOpacity[2] = oblue.GetValue();
			}

			Parameter displacementbound_flag = prop.GetParameters().GetItem(L"displacementbound_flag");
			if (displacementbound_flag.GetValue()) {
				Parameter displacementbound = prop.GetParameters().GetItem(L"displacementbound");
				displacementBound = (float)displacementbound.GetValue();
			}
		}

		// displacement bound
		if (displacementBound > 0.0) {
			RiAttribute("displacementbound", "sphere", &displacementBound, RI_NULL);
		}

		// get shadow shading mode 
		propRef = node.sceneItem.GetProperties().GetItem(L"XSIManShadowShading");
		CString shadowRIBShading = L"opaque";
		if (propRef.IsValid()) {
			Property prop = propRef;
			Parameter shadowShading = prop.GetParameters().GetItem(L"ShadowShading");
			shadowRIBShading = shadowShading.GetValue();
		}

		// surface color and opacity
		RiColor(surfaceColor);
		if (job->jobType != ShadowJob) {
			RiOpacity(surfaceOpacity);
		}
		else if (job->jobType == ShadowJob && (shadowRIBShading == L"Os" || shadowRIBShading == L"shade")) {
			RiOpacity(surfaceOpacity);
		}

		// update texture maps
		updateTextureMaps(material);

		// shaders
		if (job->jobType == ShadowJob && (shadowRIBShading == L"opaque" || shadowRIBShading == L"Os")) {
			RiSurface("matte", RI_NULL);
		}
		else if (hasSurfaceShader) {
			unsigned numTokens = ribTokensSurf.size();
			RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
			RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
			assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
			char* surfaceName = NULL;
			XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
			RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
			ribTokensSurf.clear();
		}
		else {
			if(material.IsValid()) {
				if (g_RenderTreeSupport == true) {
					if (getSurfaceShader(material, node.sceneItem.GetName(), surfaceShaderName) == CStatus::OK) {
						// if object is nurb, invert t coordinate
						if (node.objectType == NurbsObject) {
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, 1);
							ribTokensSurf.push_back(tokenDataFloat);
						}

						// add global illumination paramaters
						if (surfaceShaderName != L"xsimanParticle") 
							addGlobalIllumParams(ribTokensSurf);
										
						unsigned numTokens = ribTokensSurf.size();
						RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
						RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens);
						assignRibTokensV(&ribTokensSurf, tokenArray, pointerArray);
						char* surfaceName = NULL;
						XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
						RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
						ribTokensSurf.clear();
					}
				}
				else if (g_RenderTreeSupport == false) {
					if (builtInShader(material, surfaceShaderName) == CStatus::OK) {
						// if object is nurb, invert t coordinate
						if (node.objectType == NurbsObject) {
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, 1);
							ribTokens.push_back(tokenDataFloat);
						}
						
						// add global illumination paramaters
						if (surfaceShaderName != L"xsimanParticle") 
							addGlobalIllumParams(ribTokens);

						unsigned numTokens = ribTokens.size();
						RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
						RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
						assignRibTokensV(&ribTokens, tokenArray, pointerArray);
						char* surfaceName = NULL;
						XSIW2A(&surfaceName, surfaceShaderName.GetWideString());
						RiSurfaceV(surfaceName, numTokens, tokenArray, pointerArray);
						ribTokens.clear();
					}
				}
				else RiSurface("matte", RI_NULL);
			}
		} 

		if (hasDisplacementShader) {
			unsigned numTokens = ribTokensDisp.size();
			RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
			RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
			assignRibTokensV(&ribTokensDisp, tokenArray, pointerArray);
			char* displaceName = NULL;
			XSIW2A(&displaceName, displaceShaderName.GetWideString());
			RiDisplacementV(displaceName, numTokens, tokenArray, pointerArray);
			ribTokensDisp.clear();
		}
		else {
			if(material.IsValid()) {
				if (g_RenderTreeSupport == true) {

					if (getDisplacementShader(material, node.sceneItem.GetName(), displaceShaderName) == CStatus::OK) {

						// go ahead and put a default bound number if it is undefined
						if (displacementBound == 0.0) {
							displacementBound = 1.0;
							RiAttribute("displacementbound", "sphere", &displacementBound, RI_NULL);
						}
		
						// if object is nurb, invert t coordinate
						if (node.objectType == NurbsObject) {
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, 1);
							ribTokensDisp.push_back(tokenDataFloat);
						}

						unsigned numTokens = ribTokensDisp.size();
						RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
						RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
						assignRibTokensV(&ribTokensDisp, tokenArray, pointerArray);
						char* displaceName = NULL;
						XSIW2A(&displaceName, displaceShaderName.GetWideString());
						RiDisplacementV(displaceName, numTokens, tokenArray, pointerArray);
						ribTokensDisp.clear();
					}
					else RiDisplacement("null", RI_NULL);
				}
			}
		}

		if (hasVolumeShader && (job->jobType != ShadowJob || (job->jobType == ShadowJob && shadowRIBShading == L"shade"))) {
			unsigned numTokens = ribTokensVol.size();
			RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
			RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
			assignRibTokensV(&ribTokensVol, tokenArray, pointerArray);
			char* volumeName = NULL;
			XSIW2A(&volumeName, volumeShaderName.GetWideString());
			RiAtmosphereV(volumeName, numTokens, tokenArray, pointerArray);
			ribTokensVol.clear();
		}
		else if (job->jobType != ShadowJob || (job->jobType == ShadowJob && shadowRIBShading == L"shade")) {
			if(material.IsValid()) {
				if (g_RenderTreeSupport == true) {

					if (getVolumeShader(material, node.sceneItem.GetName(), volumeShaderName) == CStatus::OK) {	

						// if object is nurb, invert t coordinate
						if (node.objectType == NurbsObject) {
							xsiRibTokens tokenDataFloat;
							tokenDataFloat.put("invert_t", FloatType, false, false, false, 0);
							tokenDataFloat.putDetailType(UniformDetail);
							tokenDataFloat.assignFloat(0, 1);
							ribTokensVol.push_back(tokenDataFloat);
						}

						unsigned numTokens = ribTokensVol.size();
						RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
						RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
						assignRibTokensV(&ribTokensVol, tokenArray, pointerArray);
						char* volumeName = NULL;
						XSIW2A(&volumeName, volumeShaderName.GetWideString());
						RiAtmosphereV(volumeName, numTokens, tokenArray, pointerArray);
						ribTokensVol.clear();
					}
				}
			}
		}

		if (hasInteriorShader && (job->jobType != ShadowJob || (job->jobType == ShadowJob && shadowRIBShading == L"shade"))) {
			unsigned numTokens = ribTokensInterior.size();
			RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
			RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
			assignRibTokensV(&ribTokensInterior, tokenArray, pointerArray);
			char* interiorName = NULL;
			XSIW2A(&interiorName, interiorShaderName.GetWideString());
			RiInteriorV(interiorName, numTokens, tokenArray, pointerArray);
			ribTokensInterior.clear();
		}

		if (hasImagerShader && job->jobType == BeautyJob) {
			unsigned numTokens = ribTokensImager.size();
			RtToken* tokenArray = (RtToken*)alloca(sizeof(RtToken) * numTokens);
			RtPointer* pointerArray = (RtPointer*)alloca(sizeof(RtPointer) * numTokens); 
			assignRibTokensV(&ribTokensImager, tokenArray, pointerArray);
			char* imagerName = NULL;
			XSIW2A(&imagerName, imagerShaderName.GetWideString());
			RiImagerV(imagerName, numTokens, tokenArray, pointerArray);
			ribTokensImager.clear();
		}

		// set resource handle name
		if (node.objectType == SubdivisionObject) {
			char* handleName = NULL;
			Cluster cluster(clusterRefArray[j]);
			XSIW2A(&handleName, cluster.GetFullName().GetWideString());
			const char* operation = "save";
			const char* subset = "shading";
			RiResource((RtString)handleName, "attributes", "string operation", &operation, "string subset", &subset, RI_NULL);
			RiAttributeEnd();
		}
	}
}

