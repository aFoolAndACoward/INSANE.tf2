#pragma once

// Forward decls..
class IMaterial;
class ITexture;


///////////////////////////////////////////////////////////////////////////
class IMaterialVar
{
public:
	virtual ITexture*		GetTextureValue(void) = 0;

	virtual char const*		GetName(void) const = 0;
	virtual int				GetNameAsSymbol() const = 0;

	virtual void			SetFloatValue(float val) = 0;

	virtual void			SetIntValue(int val) = 0;

	virtual void			SetStringValue(char const* val) = 0;
	virtual char const*		GetStringValue(void) const = 0;

	// Use FourCC values to pass app-defined data structures between
	// the proxy and the shader. The shader should ignore the data if
	// its FourCC type not correct.	
	virtual void			SetFourCCValue(int type, void* pData) = 0;
	virtual void			GetFourCCValue(int* type, void** ppData) = 0;

	// Vec (dim 2-4)
	virtual void			SetVecValue(float const* val, int numcomps) = 0;
	virtual void			SetVecValue(float x, float y) = 0;
	virtual void			SetVecValue(float x, float y, float z) = 0;
	virtual void			SetVecValue(float x, float y, float z, float w) = 0;
	virtual void			GetLinearVecValue(float* val, int numcomps) const = 0;

	// revisit: is this a good interface for textures?
	virtual void			SetTextureValue(ITexture*) = 0;

	virtual IMaterial* GetMaterialValue(void) = 0;
	virtual void			SetMaterialValue(IMaterial*) = 0;

	virtual bool			IsDefined() const = 0;
	virtual void			SetUndefined() = 0;

	// Matrix
	virtual void			SetMatrixValue(void* matrix) = 0;
	virtual const void*		GetMatrixValue() = 0;
	virtual bool			MatrixIsIdentity() const = 0;

	// Copy....
	virtual void			CopyFrom(IMaterialVar* pMaterialVar) = 0;

	virtual void			SetValueAutodetectType(char const* val) = 0;

	virtual IMaterial*	    GetOwningMaterial() = 0;

	//set just 1 component
	virtual void			SetVecComponentValue(float fVal, int nComponent) = 0;

protected:
	virtual int				GetIntValueInternal(void) const = 0;
	virtual float			GetFloatValueInternal(void) const = 0;
	virtual float const*	GetVecValueInternal() const = 0;
	virtual void			GetVecValueInternal(float* val, int numcomps) const = 0;
	virtual int				VectorSizeInternal() const = 0;
};
///////////////////////////////////////////////////////////////////////////