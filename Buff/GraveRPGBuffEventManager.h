// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "UObject/NoExportTypes.h"
#include "GraveRPGBuffImpBase.h"
#include "WeakObjectPtrTemplates.h"
#include "RPGCharacterBase.h"
#include "RPGTypes.h"
#include "SkillBuffEventManager.generated.h"


UCLASS(BlueprintType)
class ACTIONRPG_API UGraveRPGBuffEventManager : public UObject
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FCalCameraEyeHeight,float&,Eyeheight); 
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAddSkillDelegate, int32, SkillID, int32, Group);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActivateSkillDelegate, int32, SkillID, int32, Group);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStartSkillDelegate, int32, SkillID, int32, Group);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRemoveAllSkillDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRemoveSkillDelegate, int32, SkillID, int32, Group);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRefreshSkillDelegate, int32, SkillID, int32, Group, bool, IsActivate);
	DECLARE_MULTICAST_DELEGATE_FourParams(FModifyDamageDelegate, class ARPGCharacterBase*, struct FDamageEvent const&, float, float&);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshUIDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUsingPropDelegate, int32, PlayerID, int32, PropID);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEnergyChangedDelegate, int32, PlayerID, int32, Energy, int32, FullEnergy);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRefreshArmorBuffDelegate, class ARPGCharacterBase*, Instigator, float, LeftArmor, bool, IsBody);

public:
	UGraveRPGBuffEventManager();

public:
	void OnFinishMatch();

	/** ��Ӽ���(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FAddSkillDelegate		AddSkillDelegate;

	/** �����(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FActivateSkillDelegate	ActivateSkillDelegate;

	/** ʹ�ü���(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FStartSkillDelegate		StartSkillDelegate;

	/** ɾ������(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FRemoveAllSkillDelegate	RemoveAllSkillDelegate;

	/** ɾ������(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FRemoveSkillDelegate	RemoveSkillDelegate;

	/** ɾ������(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FRefreshSkillDelegate	RefreshSkillDelegate;

	/** �ض������UIˢ��(��ͼʹ��) */
	UPROPERTY(BlueprintAssignable)
	FRefreshUIDelegate	RefreshUIDelegate;

	FModifyDamageDelegate OnModifyDamageDelegate;

	FUsingPropDelegate OnUsingPropIDChangedDelegate;

	FEnergyChangedDelegate OnEnergyChangedDelegate;

	FRefreshArmorBuffDelegate OnRefreshArmorBuffDelegate;
	
};
