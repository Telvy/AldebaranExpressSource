//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Patty Petrinova - Patty Petrinova is the quintessential in-house poster child of Aldebaran Express and an integral core member of the main Aldebaran Express cast. 
//Patty is an energetic, bubbly and optimistic young female clerk for the Aldebaran Express Floral Department. She is full of spunk and charisma, and while her infantile and 
//clumsy personality may offset her appearance as a dunce, her fierce attitude and tomboyish demeanor sets her apart as one of Aldebaran Express’s most hardworking and rigorous 
//staff members. While Patty is hard at work sorting, cutting and watering flowers with her older colleague Ms.Beth, she will often go around the store assisting others in her 
//down time, and especially loves to hang out with her “bestie” Kiesha Williams.Patty loves to interact with anyone and everyone and tends to get in everyone’s business in a child - like way.

//
//=============================================================================//

//-----------------------------------------------------------------------------
// Generic NPC - purely for scripted sequence work.
//-----------------------------------------------------------------------------
#include "cbase.h"
#include "npcevent.h"
#include "ai_basenpc.h"
#include "ai_hull.h"
#include "ai_behavior_follow.h"
#include "ai_playerally.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// NPC's Anim Events Go Here
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CNPC_Patty : public CAI_PlayerAlly
{
public:
	DECLARE_CLASS(CNPC_Patty, CAI_PlayerAlly);
	DECLARE_DATADESC();

	void	Spawn(void);
	void	Precache(void);
	Class_T Classify(void);
	void	HandleAnimEvent(animevent_t *pEvent);
	virtual Disposition_t IRelationType(CBaseEntity *pTarget);
	int		GetSoundInterests(void);
	bool	CreateBehaviors(void);
	int		SelectSchedule(void);

private:
	CAI_FollowBehavior	m_FollowBehavior;
};

LINK_ENTITY_TO_CLASS(npc_patty, CNPC_Patty)

BEGIN_DATADESC(CNPC_Patty)
// (auto saved by AI)
//	DEFINE_FIELD( m_FollowBehavior, FIELD_EMBEDDED ),	(auto saved by AI)
END_DATADESC()

//-----------------------------------------------------------------------------
// Classify - indicates this NPC's place in the 
// relationship table.
//-----------------------------------------------------------------------------
Class_T	CNPC_Patty::Classify(void)
{
	return CLASS_PLAYER_ALLY_VITAL;
}

//-----------------------------------------------------------------------------
// HandleAnimEvent - catches the NPC-specific messages
// that occur when tagged animation frames are played.
//-----------------------------------------------------------------------------
void CNPC_Patty::HandleAnimEvent(animevent_t *pEvent)
{
	switch (pEvent->event)
	{
	case 1:
	default:
		BaseClass::HandleAnimEvent(pEvent);
		break;
	}
}

//-----------------------------------------------------------------------------
// GetSoundInterests - generic NPC can't hear.
//-----------------------------------------------------------------------------
int CNPC_Patty::GetSoundInterests(void)
{
	return NULL;
}

//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CNPC_Patty::Spawn()
{
	Precache();

	BaseClass::Spawn();

	SetModel("models/humans/patty/patty.mdl");

	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);
	SetBloodColor(BLOOD_COLOR_RED);
	m_iHealth = 8;
	m_flFieldOfView = 0.5;// indicates the width of this NPC's forward view cone ( as a dotproduct result )
	m_NPCState = NPC_STATE_NONE;
	SetImpactEnergyScale(0.0f); // no physics damage on the gman

	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_OPEN_DOORS | bits_CAP_ANIMATEDFACE | bits_CAP_TURN_HEAD);
	CapabilitiesAdd(bits_CAP_FRIENDLY_DMG_IMMUNE);
	AddEFlags(EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL);

	NPCInit();
}

//-----------------------------------------------------------------------------
// Precache - precaches all resources this NPC needs
//-----------------------------------------------------------------------------
void CNPC_Patty::Precache()
{
	PrecacheModel("models/humans/patty/patty.mdl");

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Patty ain't scared of nothing! (except maybe little green space men)
//-----------------------------------------------------------------------------
Disposition_t CNPC_Patty::IRelationType(CBaseEntity *pTarget)
{
	return D_NU;
}

//=========================================================
// Purpose:
//=========================================================
bool CNPC_Patty::CreateBehaviors()
{
	AddBehavior(&m_FollowBehavior);

	return BaseClass::CreateBehaviors();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CNPC_Patty::SelectSchedule(void)
{
	if (!BehaviorSelectSchedule())
	{
	}

	return BaseClass::SelectSchedule();
}


//-----------------------------------------------------------------------------
// AI Schedules Specific to this NPC
//-----------------------------------------------------------------------------