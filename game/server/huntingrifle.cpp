//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Snipars - 360 noscope
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "player.h"
#include "gamerules.h"
#include "ammodef.h"
#include "mathlib/mathlib.h"
#include "in_buttons.h"
#include "soundent.h"
#include "basebludgeonweapon.h"
#include "vstdlib/random.h"
#include "npcevent.h"
#include "ai_basenpc.h"
#include "gib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
#define CWeaponSnipars C_WeaponSnipars
#endif

class CWeaponSnipars : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponSnipars, CBaseHLCombatWeapon);

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

public:

	CWeaponSnipars(void);

	void	Precache(void);
	void	PrimaryAttack(void);
	//void	SecondaryAttack(void);
	virtual void	ItemPostFrame(void);
	bool	Reload(void);
	void	WeaponIdle(void);
	void	DryFire(void);
	bool	Holster(CBaseCombatWeapon *pSwitchingTo = NULL);

	DECLARE_DATADESC();

private:
	void	SniparsFire();
	void	CheckZoomToggle(void);
	void	ToggleZoom(void);
private:
	//CNetworkVar(float, m_fInZoom);
	bool				m_bInZoom;
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponSnipars, DT_WeaponSnipars);

BEGIN_NETWORK_TABLE(CWeaponSnipars, DT_WeaponSnipars)
//what
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_huntingrifle, CWeaponSnipars);

BEGIN_DATADESC(CWeaponSnipars)

DEFINE_FIELD(m_bInZoom, FIELD_BOOLEAN),

END_DATADESC()

BEGIN_PREDICTION_DATA(CWeaponSnipars)
//what
END_PREDICTION_DATA()

PRECACHE_WEAPON_REGISTER(weapon_huntingrifle);


CWeaponSnipars::CWeaponSnipars(void)
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = true;
	m_bAltFiresUnderwater = true;
	m_bInZoom = false;
}


void CWeaponSnipars::Precache(void)
{

	BaseClass::Precache();
}


void CWeaponSnipars::DryFire(void)
{
	WeaponSound(EMPTY);
	SendWeaponAnim(ACT_VM_DRYFIRE);

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
}


void CWeaponSnipars::PrimaryAttack(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
	{
		return;
	}
	if (pPlayer->GetWaterLevel() == 3)
	{
		{
			WeaponSound(EMPTY);
		}
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.5;
		m_flNextSecondaryAttack = gpGlobals->curtime + 0.5;
		return;
	}
	SniparsFire();
}


//void CWeaponSnipars::SecondaryAttack(void)
//{
//	ToggleZoom();
//	m_flNextSecondaryAttack = gpGlobals->curtime + 0.5;
//}

void CWeaponSnipars::CheckZoomToggle(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer->m_afButtonPressed & IN_ATTACK2)
	{
		ToggleZoom();
	}
}

void CWeaponSnipars::ItemPostFrame(void)
{
	// Allow zoom toggling
	CheckZoomToggle();


	BaseClass::ItemPostFrame();
}


void CWeaponSnipars::SniparsFire(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
	{
		return;
	}

	if (m_iClip1 <= 0)
	{
		if (!m_bFireOnEmpty)
		{
			Reload();
		}
		else
		{
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	WeaponSound(SINGLE);
	//pPlayer->DoMuzzleFlash();

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_flNextPrimaryAttack = gpGlobals->curtime + 1.2;
	m_flNextSecondaryAttack = gpGlobals->curtime + 0.25;

	m_iClip1--;

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	//	pPlayer->FireBullets( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 0 );

	FireBulletsInfo_t info(1, vecSrc, vecAiming, Vector(0, 0, 0), MAX_TRACE_LENGTH, m_iPrimaryAmmoType);
	info.m_pAttacker = pPlayer;
	info.m_iTracerFreq = 0;

	pPlayer->FireBullets(info);

#ifndef CLIENT_DLL
	//pPlayer->SetMuzzleFlashTime(gpGlobals->curtime + 0.5);
#endif

	pPlayer->ViewPunch(QAngle(-8, 0, 0));

#ifndef CLIENT_DLL
	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 600, 0.2);
#endif

	if (!m_iClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		// HEV suit - indicate out of ammo condition
	}
}


bool CWeaponSnipars::Reload(void)
{
	bool iResult;
	iResult = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (m_bInZoom)
	{
		ToggleZoom();
	}

	return iResult;
}



void CWeaponSnipars::WeaponIdle(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (pPlayer)
	{
		pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
	}

	// only idle if the slid isn't back
	if (m_iClip1 != 0)
	{
		BaseClass::WeaponIdle();
	}
}

bool CWeaponSnipars::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	if (m_bInZoom)
	{
		ToggleZoom();
	}

	return BaseClass::Holster(pSwitchingTo);
}

void CWeaponSnipars::ToggleZoom(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
	{
		return;
	}

#if !defined(CLIENT_DLL)
	if (m_bInZoom)
	{
		if (pPlayer->SetFOV(this, 0, 0.2f))
		{
			m_bInZoom = false;
		}
	}
	else
	{
		if (pPlayer->SetFOV(this, 60, 0.1f))
		{
			m_bInZoom = true;
		}
	}
#endif
}