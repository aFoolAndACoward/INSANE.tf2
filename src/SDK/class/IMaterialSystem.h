#pragma once

#include "IMatRenderContext.h"

#include "../../Utility/Interface Handler/Interface.h"
#include "../../Utility/Signature Handler/signatures.h"

class ITexture;
class KeyValues;
class IMaterial;
class IMaterialSystem;

// Functions
MAKE_SIG(CMaterialSystem_CreateNamedRenderTargetTextureEx, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 8B 9C 24",                   MATERIALSYSTEM_DLL, ITexture*, void*, const char*, int, int, int, int, int, int, int)
MAKE_SIG(CMaterialSystem_GetRenderContext,                 "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 48 81 C1",                                    MATERIALSYSTEM_DLL, IMatRenderContext*, void*)
MAKE_SIG(CMaterialSystem_ClearBuffer,                      "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B D9 41 0F B6 F9",                   MATERIALSYSTEM_DLL, void*, void*, bool, bool, bool)
MAKE_SIG(CMaterialSystem_GetBackBufferFormat,              "48 8B 0D ? ? ? ? 48 8B 01 48 FF 60 ? CC CC 48 8B 0D ? ? ? ? 48 8B 01 48 FF 60 ? CC CC 48 83 EC", MATERIALSYSTEM_DLL, int64_t, int)


MAKE_SIG(CMateiralSystem_FindTexture,    "40 55 57 41 55",                       MATERIALSYSTEM_DLL, ITexture*,	 void*,            const char*, const char*, bool,     int)
MAKE_SIG(CMaterialSystem_FindMaterial,   "48 83 EC ? 48 8B 44 24 ? 4C 8B 11",    MATERIALSYSTEM_DLL, IMaterial*, void*,            const char*, const char*, bool, const char*)
MAKE_SIG(CMaterialSystem_CreateMaterial, "48 89 5C 24 ? 57 48 83 EC ? 48 8B C2", MATERIALSYSTEM_DLL, IMaterial*, IMaterialSystem*, const char*, KeyValues*)
MAKE_SIG(CMaterialSystem_FirstMaterial,  "0F B7 81 ? ? ? ? 4C 8B C9",            MATERIALSYSTEM_DLL, unsigned short, void*)
MAKE_SIG(CMaterialSystem_NextMaterial,   "48 81 C1 ? ? ? ? E9 ? ? ? ? CC CC CC CC 89 15", MATERIALSYSTEM_DLL, unsigned short, void*, unsigned short)
MAKE_SIG(CMaterialSystem_GetMaterial,    "0F B7 C2 48 8D 14 40",                 MATERIALSYSTEM_DLL, IMaterial*, void*, unsigned short)


#define CREATERENDERTARGETFLAGS_HDR				0x00000001
#define CREATERENDERTARGETFLAGS_AUTOMIPMAP		0x00000002
#define CREATERENDERTARGETFLAGS_UNFILTERABLE_OK 0x00000004


///////////////////////////////////////////////////////////////////////////
enum StencilOperation_t
{
    STENCILOPERATION_KEEP = 1,
    STENCILOPERATION_ZERO = 2,
    STENCILOPERATION_REPLACE = 3,
    STENCILOPERATION_INCRSAT = 4,
    STENCILOPERATION_DECRSAT = 5,
    STENCILOPERATION_INVERT = 6,
    STENCILOPERATION_INCR = 7,
    STENCILOPERATION_DECR = 8,

    STENCILOPERATION_FORCE_DWORD = 0x7fffffff
};
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
enum CompiledVtfFlags
{
    TEXTUREFLAGS_POINTSAMPLE        = 0x00000001,
    TEXTUREFLAGS_TRILINEAR          = 0x00000002,
    TEXTUREFLAGS_CLAMPS             = 0x00000004,
    TEXTUREFLAGS_CLAMPT             = 0x00000008,
    TEXTUREFLAGS_ANISOTROPIC        = 0x00000010,
    TEXTUREFLAGS_HINT_DXT5          = 0x00000020,
    TEXTUREFLAGS_SRGB               = 0x00000040,
    TEXTUREFLAGS_NORMAL             = 0x00000080,
    TEXTUREFLAGS_NOMIP              = 0x00000100,
    TEXTUREFLAGS_NOLOD              = 0x00000200,
    TEXTUREFLAGS_ALL_MIPS           = 0x00000400,
    TEXTUREFLAGS_PROCEDURAL         = 0x00000800,
    TEXTUREFLAGS_ONEBITALPHA        = 0x00001000,
    TEXTUREFLAGS_EIGHTBITALPHA      = 0x00002000,
    TEXTUREFLAGS_ENVMAP             = 0x00004000,
    TEXTUREFLAGS_RENDERTARGET       = 0x00008000,
    TEXTUREFLAGS_DEPTHRENDERTARGET  = 0x00010000,
    TEXTUREFLAGS_NODEBUGOVERRIDE    = 0x00020000,
    TEXTUREFLAGS_SINGLECOPY         = 0x00040000,
    TEXTUREFLAGS_STAGING_MEMORY     = 0x00080000,
    TEXTUREFLAGS_IMMEDIATE_CLEANUP  = 0x00100000,
    TEXTUREFLAGS_IGNORE_PICMIP      = 0x00200000,
    TEXTUREFLAGS_UNUSED_00400000    = 0x00400000,
    TEXTUREFLAGS_NODEPTHBUFFER      = 0x00800000,
    TEXTUREFLAGS_UNUSED_01000000    = 0x01000000,
    TEXTUREFLAGS_CLAMPU             = 0x02000000,
    TEXTUREFLAGS_VERTEXTEXTURE      = 0x04000000,					// Useable as a vertex texture
    TEXTUREFLAGS_SSBUMP             = 0x08000000,
    TEXTUREFLAGS_UNUSED_10000000    = 0x10000000,
    TEXTUREFLAGS_BORDER             = 0x20000000,
    TEXTUREFLAGS_STREAMABLE_COARSE  = 0x40000000,
    TEXTUREFLAGS_STREAMABLE_FINE    = 0x80000000,
    TEXTUREFLAGS_STREAMABLE         = (TEXTUREFLAGS_STREAMABLE_COARSE | TEXTUREFLAGS_STREAMABLE_FINE)
};
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
enum MaterialRenderTargetDepth_t
{
    MATERIAL_RT_DEPTH_SHARED = 0x0,
    MATERIAL_RT_DEPTH_SEPARATE = 0x1,
    MATERIAL_RT_DEPTH_NONE = 0x2,
    MATERIAL_RT_DEPTH_ONLY = 0x3,
};
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
enum RenderTargetSizeMode_t
{
    RT_SIZE_NO_CHANGE = 0,			// Only allowed for render targets that don't want a depth buffer
    // (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
    RT_SIZE_DEFAULT = 1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
    RT_SIZE_PICMIP = 2,				// Apply picmip to the render target's width and height.
    RT_SIZE_HDR = 3,					// frame_buffer_width / 4
    RT_SIZE_FULL_FRAME_BUFFER = 4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
    RT_SIZE_OFFSCREEN = 5,			// Target of specified size, don't mess with dimensions
    RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6, // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
    RT_SIZE_REPLAY_SCREENSHOT = 7,	// Rounded down to power of 2, essentially...
    RT_SIZE_LITERAL = 8,			// Use the size passed in. Don't clamp it to the frame buffer size. Really.
    RT_SIZE_LITERAL_PICMIP = 9		// Use the size passed in, don't clamp to the frame buffer size, but do apply picmip restrictions.

};
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
enum ImageFormat
{
    IMAGE_FORMAT_UNKNOWN = -1,
    IMAGE_FORMAT_RGBA8888 = 0,
    IMAGE_FORMAT_ABGR8888,
    IMAGE_FORMAT_RGB888,
    IMAGE_FORMAT_BGR888,
    IMAGE_FORMAT_RGB565,
    IMAGE_FORMAT_I8,
    IMAGE_FORMAT_IA88,
    IMAGE_FORMAT_P8,
    IMAGE_FORMAT_A8,
    IMAGE_FORMAT_RGB888_BLUESCREEN,
    IMAGE_FORMAT_BGR888_BLUESCREEN,
    IMAGE_FORMAT_ARGB8888,
    IMAGE_FORMAT_BGRA8888,
    IMAGE_FORMAT_DXT1,
    IMAGE_FORMAT_DXT3,
    IMAGE_FORMAT_DXT5,
    IMAGE_FORMAT_BGRX8888,
    IMAGE_FORMAT_BGR565,
    IMAGE_FORMAT_BGRX5551,
    IMAGE_FORMAT_BGRA4444,
    IMAGE_FORMAT_DXT1_ONEBITALPHA,
    IMAGE_FORMAT_BGRA5551,
    IMAGE_FORMAT_UV88,
    IMAGE_FORMAT_UVWQ8888,
    IMAGE_FORMAT_RGBA16161616F,
    IMAGE_FORMAT_RGBA16161616,
    IMAGE_FORMAT_UVLX8888,
    IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
    IMAGE_FORMAT_RGB323232F,
    IMAGE_FORMAT_RGBA32323232F,

    // Depth-stencil texture formats for shadow depth mapping
    IMAGE_FORMAT_NV_DST16,		// 
    IMAGE_FORMAT_NV_DST24,		//
    IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
    IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
    IMAGE_FORMAT_ATI_DST16,		// 
    IMAGE_FORMAT_ATI_DST24,		//
    IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

    // Compressed normal map formats
    IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
    IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

    IMAGE_FORMAT_DXT1_RUNTIME,
    IMAGE_FORMAT_DXT5_RUNTIME,

    NUM_IMAGE_FORMATS
};
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
enum StencilComparisonFunction_t
{
    STENCILCOMPARISONFUNCTION_NEVER = 1,
    STENCILCOMPARISONFUNCTION_LESS = 2,
    STENCILCOMPARISONFUNCTION_EQUAL = 3,
    STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
    STENCILCOMPARISONFUNCTION_GREATER = 5,
    STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
    STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
    STENCILCOMPARISONFUNCTION_ALWAYS = 8,

    STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
};
///////////////////////////////////////////////////////////////////////////



class IMaterialSystem
{
public:
    ///////////////////////////////////////////////////////////////////////////
    inline void* ClearBuffer(bool bClearColor, bool bClearDepth, bool bClearStencil = false) 
    { 
        return Sig::CMaterialSystem_ClearBuffer(this, bClearColor, bClearDepth, bClearStencil);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline ITexture* FindTexture(const char* szTextureName, const char* szType, bool bComplain = true, int iAdditionalFlags = 0)
    {
        return Sig::CMateiralSystem_FindTexture(this, szTextureName, szType, bComplain, iAdditionalFlags);
    }
    

    ///////////////////////////////////////////////////////////////////////////
    inline IMatRenderContext* GetRenderContext() { return Sig::CMaterialSystem_GetRenderContext(this); }


    ///////////////////////////////////////////////////////////////////////////
    inline void* CreateRenderTarget( int w, int h, int sizeMode, int format, int depth)
    {
        return 
            Sig::CMaterialSystem_CreateNamedRenderTargetTextureEx(this, NULL, w, h, sizeMode, format, depth, 4 | 8, 0);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline ITexture* CreateRenderTarget(const char* szTargetName, int w, int h, int sizeMode, int format, int depth)
    {
        return 
            Sig::CMaterialSystem_CreateNamedRenderTargetTextureEx(this, szTargetName, w, h, sizeMode, format, depth, 4 | 8, 0);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline void* CreateNamedRenderTargetTextureEx(const char* pRTName,
        int w, int h,
        RenderTargetSizeMode_t sizeMode, ImageFormat format,
        MaterialRenderTargetDepth_t depth,
        unsigned int textureFlags, unsigned int renderTargetFlags)
    {
        return Sig::CMaterialSystem_CreateNamedRenderTargetTextureEx(
            this, pRTName, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags
        );
    }


    ///////////////////////////////////////////////////////////////////////////
    // Find Materials
    inline IMaterial* FindMaterial(const char* szMaterialName, const char* szTextureGroupName, bool bComplain, const char* szCommonPrefix)
    {
        return Sig::CMaterialSystem_FindMaterial(this, szMaterialName, szTextureGroupName, bComplain, szCommonPrefix);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline IMaterial* CreateMaterial(const char* szMaterialName, KeyValues* pKV)
    {
        return Sig::CMaterialSystem_CreateMaterial(this, szMaterialName, pKV);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline unsigned short FirstMaterial()
    {
        return Sig::CMaterialSystem_FirstMaterial(this);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline unsigned short NextMaterial(unsigned short hMaterial)
    {
        return Sig::CMaterialSystem_NextMaterial(this, hMaterial);
    }


    ///////////////////////////////////////////////////////////////////////////
    inline unsigned short InvalidMaterial()
    {
        return 0xFFFF;
    }


    ///////////////////////////////////////////////////////////////////////////
    inline IMaterial* GetMaterial(unsigned short hMaterial)
    {
        return Sig::CMaterialSystem_GetMaterial(this, hMaterial);
    }
};

MAKE_INTERFACE_VERSION(iMaterialSystem, "VMaterialSystem082", IMaterialSystem, MATERIALSYSTEM_DLL)