#pragma once

class ITexture
{
public:
    virtual const    char* GetName            (void) const                                        = 0;
    virtual int      GetMappingWidth          () const                                            = 0;
    virtual int      GetMappingHeight         () const                                            = 0;
    virtual int      GetActualWidth           () const                                            = 0;
    virtual int      GetActualHeight          () const                                            = 0;
    virtual int      GetNumAnimationFrames    () const                                            = 0;
    virtual bool     IsTranslucent            () const                                            = 0;
    virtual bool     IsMipmapped              () const                                            = 0;
    virtual void     GetLowResColorSample     (float s, float t, float* color) const              = 0;
    virtual void*    GetResourceData          (unsigned long eDataType, size_t* pNumBytes) const  = 0;
    virtual void     IncrementReferenceCount  (void)                                              = 0;
    virtual void     DecrementReferenceCount  (void)                                              = 0;
    inline void AddRef() { IncrementReferenceCount(); }
    inline void Release() { DecrementReferenceCount(); }
    virtual void     SetTextureRegenerator    (void* pTextureRegen)                               = 0;
    virtual void     Download                 (void* pRect = 0, int nAdditionalCreationFlags = 0) = 0;
    virtual int      GetApproximateVidMemBytes(void) const                                        = 0;
    virtual bool     IsError                  () const                                            = 0;
    virtual bool     IsVolumeTexture          () const                                            = 0;
    virtual int      GetMappingDepth          () const                                            = 0;
    virtual int      GetActualDepth           () const                                            = 0;
    virtual void     GetImageFormat           () const                                            = 0;
    virtual void     GetNormalDecodeMode      () const                                            = 0;
    virtual bool     IsRenderTarget           () const                                            = 0;
    virtual bool     IsCubeMap                () const                                            = 0;
    virtual bool     IsNormalMap              () const                                            = 0;
    virtual bool     IsProcedural             () const                                            = 0;
    virtual void     SwapContents             (ITexture* pOther)                                  = 0;
    virtual unsigned int GetFlags             (void) const                                        = 0;
    virtual void     ForceLODOverride         (int iNumLodsOverrideUpOrDown)                      = 0;
    virtual bool     SaveToFile               (const char* fileName)                              = 0;
    virtual void     CopyToStagingTexture     (ITexture* pDstTex)                                 = 0;
    virtual void     SetErrorTexture          (bool bIsErrorTexture)                              = 0;
};


class ITextureInternal : public ITexture
{
public:

    virtual void      Bind                       (int sampler)                                                           = 0;
    virtual void      Bind                       (int sampler1, int nFrame, int sampler2)                                = 0;
    virtual int       GetReferenceCount          ()                                                                      = 0;
    virtual void      GetReflectivity            (vec& reflectivity)                                                     = 0;
    virtual bool      SetRenderTarget            (int nRenderTargetID)                                                   = 0;
    virtual void      ReleaseMemory              ()                                                                      = 0;
    virtual void      OnRestore                  ()                                                                      = 0;
    virtual void      SetFilteringAndClampingMode(bool bOnlyLodValues = false)                                           = 0;
    virtual void      Precache                   ()                                                                      = 0;
    virtual void      CopyFrameBufferToMe        (int nRenderTargetID = 0, void* pSrcRect = NULL, void* pDstRect = NULL) = 0;
    virtual void      CopyMeToFrameBuffer        (int nRenderTargetID = 0, void* pSrcRect = NULL, void* pDstRect = NULL) = 0;
    virtual ITexture* GetEmbeddedTexture         (int nIndex)                                                            = 0;
    virtual int64_t   GetTextureHandle           (int nFrame, int nTextureChannel = 0)                                   = 0;
};