// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActionRPG.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GraveRPGBuffImpBase.generated.h"

UENUM(BlueprintType)
namespace EBuffState
{
	enum Type
	{
		EBS_Invalid,
		EBS_Wait,//Buff �Ѿ�����,���ڵȴ�״̬
		EBS_Active,//Buff �Ѿ�����,������δ����
		EBS_Dead,  //Buff �����Ѿ�������ִ�л���Buff������Դ�Ȳ���
	};
}


UCLASS(Abstract, Blueprintable)
class ACTIONRPG_API UGraveRPGBuffImpBase : public UObject
{
	GENERATED_BODY()

public:
	void InitAndStart(class AGraveRPGBuffManagerBase* Manager, float inDelayStart, float inDuration, const FString& inResName, const FString& inParam1, const FString& inParam2, const FString& inParam3);

	bool Start();
	bool Stop();

	ENetRole GetNetRole();
	ENetMode GetNetMode();

	bool IsBuffActive();
	bool IsBuffWaiting();
	bool IsBuffAdded();

	virtual UWorld* GetWorld() const override;

public:
	UGraveRPGBuffImpBase();

protected:
	virtual void Init(float inDelayStartTime, float inDurationTime, const FString& inResName, const FString& inParam1, const FString& inParam2, const FString& inParam3);

	/** State Changed */
	virtual bool OnStateWait();
	virtual bool OnStateActive();
	virtual bool OnStateDead();

private:
	bool GotoState(EBuffState::Type NewState);
	/** Timer Process function */
	void TimeToStop();
	void TimeToActive();
private:
	float DelayStartTime;
	float DurationTime;
protected:
	UPROPERTY()
	class AGraveRPGBuffManagerBase* BuffManager;
	FTimerHandle DefaultBuffTimer;
	TEnumAsByte<EBuffState::Type> CurrentState;


};
