#pragma once
#include "Basic Structures.h"


///////////////////////////////////////////////////////////////////////////
struct GlowObjectDefinition_t
{
	int		m_hEntity;
	vec		m_vGlowColor;
	float	m_flGlowAlpha;

	bool	m_bRenderWhenOccluded;
	bool	m_bRenderWhenUnoccluded;
	int		m_nSplitScreenSlot;

	// Linked list of free slots
	int		m_nNextFreeSlot;
};
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
class CGlowObjectManager
{
public:
	// CUtlVector unfolded.
	GlowObjectDefinition_t* m_pVecDrawObjects;
	int						m_nAllocationCount;
	int						m_nGrowSize;
	int						m_Size;

	// GlowManager's stuff.
	int						m_nFirstFreeSlot;
};
///////////////////////////////////////////////////////////////////////////