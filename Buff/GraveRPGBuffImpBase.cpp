// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "ActionRPG.h"
#include "GraveRPGBuffManagerBase.h"
#include "GraveRPGBuffImpBase.h"



UGraveRPGBuffImpBase::UGraveRPGBuffImpBase()
{
	CurrentState = EBuffState::EBS_Invalid;
	DelayStartTime = 0.0f;
	DurationTime = 0.0f;
}

void UGraveRPGBuffImpBase::InitAndStart(AGraveRPGBuffManagerBase* Manager, float inDelayStart, float inDuration, const FString& inResName, const FString& inParam1, const FString& inParam2, const FString& inParam3)
{
	BuffManager = Manager;
	Init(inDelayStart, inDuration, inResName, inParam1, inParam2, inParam3);
	Start();
}

ENetMode UGraveRPGBuffImpBase::GetNetMode()
{
	check(BuffManager);
	if (BuffManager)
	{
		return BuffManager->GetNetMode();
	}
	else
	{
		return NM_MAX;
	}
}

ENetRole UGraveRPGBuffImpBase::GetNetRole()
{
	check(BuffManager);
	if (BuffManager)
	{
		return BuffManager->Role;
	}
	else
	{
		return ROLE_None;
	}
}

bool UGraveRPGBuffImpBase::IsBuffActive()
{
	return CurrentState == EBuffState::EBS_Active;
}

bool UGraveRPGBuffImpBase::IsBuffWaiting()
{
	return CurrentState == EBuffState::EBS_Wait;
}

bool UGraveRPGBuffImpBase::IsBuffAdded()
{
	return IsBuffActive() || IsBuffWaiting();
}

bool UGraveRPGBuffImpBase::Start()
{
	bool Result = false;
	if (DelayStartTime > 0.f)
	{
		Result = GotoState(EBuffState::EBS_Wait);
	}
	else
	{
		Result = GotoState(EBuffState::EBS_Active);
	}
	return Result;
}

bool UGraveRPGBuffImpBase::Stop()
{
	bool Result = GotoState(EBuffState::EBS_Dead);
	return true;
}

void UGraveRPGBuffImpBase::Init(float inDelayStartTime, float inDurationTime, const FString& inResName, const FString& inParam1, const FString& inParam2, const FString& inParam3)
{
	DelayStartTime = inDelayStartTime;
	DurationTime = inDurationTime;
	UE_LOG(LogActionRPG, Log, TEXT("DelayStart[%f], Duration[%f], ResName[%s], Param1[%s], Param2[%s]"), DelayStartTime, DurationTime, *inResName, *inParam1, *inParam2);

}

/** State Changed */
bool UGraveRPGBuffImpBase::OnStateWait()
{
	check(BuffManager);
	if (BuffManager)
	{
		BuffManager->GetWorldTimerManager().SetTimer(DefaultBuffTimer, this, &UGraveRPGBuffImpBase::TimeToActive, DelayStartTime, false);
	}

	return true;
}

bool UGraveRPGBuffImpBase::OnStateActive()
{
	check(BuffManager);
	if (BuffManager)
	{
		BuffManager->GetWorldTimerManager().SetTimer(DefaultBuffTimer, this, &UGraveRPGBuffImpBase::TimeToStop, DurationTime, false);
	}
	return true;
}

bool UGraveRPGBuffImpBase::OnStateDead()
{
	return true;
}

/** Timer Process function */
void UGraveRPGBuffImpBase::TimeToStop()
{
	Stop();
}

void UGraveRPGBuffImpBase::TimeToActive()
{
	GotoState(EBuffState::EBS_Active);
}

bool UGraveRPGBuffImpBase::GotoState(EBuffState::Type NewState)
{
	if (CurrentState == NewState)
	{
		UE_LOG(LogActionRPG, Log, TEXT("GotoState SameState [%d]"), (int32)NewState);
		return true;
	}
	bool Result = false;
	UE_LOG(LogActionRPG, Log, TEXT("GotoState[%d]--->[%d]"), (int32)CurrentState, (int32)NewState);

	switch (NewState)
	{
	case EBuffState::Type::EBS_Wait:
		Result = OnStateWait();
		break;
	case EBuffState::Type::EBS_Active:
		Result = OnStateActive();
		break;
	case EBuffState::Type::EBS_Dead:
		Result = OnStateDead();
		break;
	default:
		UE_LOG(LogActionRPG, Log, TEXT("no matched state NewState[%d] CurrentState[%d]"), (int32)NewState, (int32)CurrentState);
		return false;
	}
	if (Result)
	{
		CurrentState = NewState;
	}
	else
	{
		UE_LOG(LogActionRPG, Warning, TEXT("GotoState failed[%d]--->[%d]"), (int32)CurrentState, (int32)NewState);
	}
	return Result;
}

UWorld* UGraveRPGBuffImpBase::GetWorld() const
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		return GWorld;
	}
#endif // WITH_EDITOR
	return GEngine->GetWorldContexts()[0].World();
}
