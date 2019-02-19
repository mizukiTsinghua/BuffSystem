// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPG.h"
#include "GraveRPGBuffImpBase.h"
#include "GraveRPGBuffManagerBase.h"


namespace nmSkillAndBuff
{
	static int32 CurrentBuffHandle = 0;
};

// Sets default values
AGraveRPGBuffManagerBase::AGraveRPGBuffManagerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 0.1;
	MinNetUpdateFrequency = 0.1f;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	bInited = false;
	//bNetManualUpdate = false;

}


void AGraveRPGBuffManagerBase::OnRep_BuffTableName()
{
	if (GetOwner() && !bInited)
	{
		Init(BuffTableName);
	}
}

void AGraveRPGBuffManagerBase::OnRep_Owner()
{
	if (GetOwner())
	{
		if (!bInited)
		{
			Init(BuffTableName);
		}
		OnRep_PendingBuffer();
	}
}

bool AGraveRPGBuffManagerBase::Init(const FString& TableName)
{
	if (!TableName.IsEmpty() && !TableName.Equals(TEXT("None"), ESearchCase::IgnoreCase))
	{
		if (Role == ROLE_Authority)
		{
			BuffTableName = TableName;
			ForceNetUpdate();
		}
	}
	if (BuffTableName.IsEmpty())
	{
		return false;
	}
	LoadConfig();
	//InitSkillList();
	bInited = true;
	return true;
}

bool AGraveRPGBuffManagerBase::LoadConfig()
{
	BuffTable = LoadObject<UDataTable>(nullptr, *BuffTableName);
	check(BuffTable);
	return true;
}

//#if USE_COMPATIBLE_REPLAYOUT
//void AGraveRPGBuffManagerBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps, uint32 LifetimeVersion) const
////#else
////void AGraveRPGBuffManagerBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
////#endif
//{
////#if USE_COMPATIBLE_REPLAYOUT
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps, LifetimeVersion);
////#else
////	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
////#endif
//
//	DOREPLIFETIME(AGraveRPGBuffManagerBase, PendingBuffList);
//	DOREPLIFETIME(AGraveRPGBuffManagerBase, BuffTableName);
//
//}


bool AGraveRPGBuffManagerBase::FilterBuff(int32 BuffIndex)
{
	return true;
}

int  AGraveRPGBuffManagerBase::GetBufferTargetType(int32 BuffID)
{
	FBuffRow* Row = BuffTable->FindRow<FBuffRow>(FName(*(FString::Printf(TEXT("%d"), BuffID))), "", false);
	if (Row)
		return Row->TargetType;

	return 0;
}

bool AGraveRPGBuffManagerBase::IsBuffActive(int BuffID)
{
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].BuffID == BuffID)
		{
			return CurrentBuffList[i].Buff->IsBuffActive();
		}
	}
	return false;
}

bool AGraveRPGBuffManagerBase::IsBuffActive(TSubclassOf<UGraveRPGBuffImpBase> BuffClass)
{
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].Buff->IsA(BuffClass))
		{
			return CurrentBuffList[i].Buff->IsBuffActive();
		}
	}
	return false;
}

bool AGraveRPGBuffManagerBase::IsBuffWaiting(int BuffID)
{
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].BuffID == BuffID)
		{
			return CurrentBuffList[i].Buff->IsBuffWaiting();
		}
	}
	return false;
}

bool AGraveRPGBuffManagerBase::IsBuffWaiting(TSubclassOf<UGraveRPGBuffImpBase> BuffClass)
{
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].Buff->IsA(BuffClass))
		{
			return CurrentBuffList[i].Buff->IsBuffWaiting();
		}
	}
	return false;
}

bool AGraveRPGBuffManagerBase::IsBuffAdded(int BuffID)
{
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].BuffID == BuffID)
		{
			return CurrentBuffList[i].Buff->IsBuffAdded();
		}
	}
	return false;
}

bool AGraveRPGBuffManagerBase::IsBuffAdded(TSubclassOf<UGraveRPGBuffImpBase> BuffClass)
{
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].Buff->IsA(BuffClass))
		{
			return CurrentBuffList[i].Buff->IsBuffAdded();
		}
	}
	return false;
}

int32 AGraveRPGBuffManagerBase::AddBuff_Internal(int32 BuffID, int32 SkillSessionID)
{
	if (!FilterBuff(BuffID))
	{
		UE_LOG(LogActionRPG, Log, TEXT("FiterBuff failed BuffIndex[%d], SkillSessionID[%d]"), BuffID, SkillSessionID);
		return -1;
	}

	FBuffRow* Row = BuffTable->FindRow<FBuffRow>(FName(*(FString::Printf(TEXT("%d"), BuffID))), "", false);
	check(Row);
	UClass* ObjectClass = StaticLoadClass(UGraveRPGBuffImpBase::StaticClass(), NULL, *(Row->ClassName), NULL, LOAD_None, NULL);
	if (!ObjectClass)
	{
		UE_LOG(LogActionRPG, Warning, TEXT("AddBuff_Internal ObjectClass[%s] Load Failed"), *(Row->ClassName));
		return -1;
	}
	UGraveRPGBuffImpBase* Buff = NewObject<UGraveRPGBuffImpBase>(this, ObjectClass);
	check(Buff);
	if (!Buff)
	{
		UE_LOG(LogActionRPG, Warning, TEXT("AddBuff_Internal NewObject ObjectClass[%s] Failed"), *(ObjectClass->GetName()));
		return false;
	}
	Buff->InitAndStart(this, Row->DelayStartTime, Row->DurationTime, Row->ResName, Row->Param1, Row->Param2, Row->Param3);
	if (Role == ROLE_Authority)
	{
		PendingBuffList.Add(BuffID);
		ForceNetUpdate();
	}
	CurrentBuffList.Add(FBuffInfo(BuffID, Buff, ++(nmSkillAndBuff::CurrentBuffHandle), SkillSessionID));
	UE_LOG(LogActionRPG, Warning, TEXT("AddBuff_Internal Succ CurrentBuffList [%d]"), CurrentBuffList.Num());

	return nmSkillAndBuff::CurrentBuffHandle;
}

int32 AGraveRPGBuffManagerBase::AddBuffFromSkill(int32 BuffID, int32 SkillSessionID)
{
	UE_LOG(LogActionRPG, Warning, TEXT("AddBuffFromSkill [%d] SkillSessionID[%d]"), BuffID, SkillSessionID);
	return AddBuff_Internal(BuffID, SkillSessionID);
}

int32 AGraveRPGBuffManagerBase::AddBuffByID(int32 BuffID)
{
	UE_LOG(LogActionRPG, Warning, TEXT("AddBuffByID [%d]"), BuffID);
	return  AddBuff_Internal(BuffID, -1);
}

void AGraveRPGBuffManagerBase::OnRep_PendingBuffer()
{
	UE_LOG(LogActionRPG, Warning, TEXT("OnRep_PendingBuffer Curr[%d] Pending[%d]"), CurrentBuffList.Num(), PendingBuffList.Num());
	if (!GetOwner())
	{
		return;
	}
	if (PendingBuffList.Num() == 0)
	{
		for (int i = 0; i < CurrentBuffList.Num(); i++)
		{
			RemoveBuff_Internal(i);
		}
		CurrentBuffList.Empty();
	}
	if (CurrentBuffList.Num() > PendingBuffList.Num())
	{
		for (int i = PendingBuffList.Num(); i < CurrentBuffList.Num(); i++)
		{
			RemoveBuff_Internal(i);
		}
		CurrentBuffList.RemoveAt(PendingBuffList.Num(), CurrentBuffList.Num() - PendingBuffList.Num());
		UE_LOG(LogActionRPG, Warning, TEXT("CurrentBuffList.Num() > PendingBuffList.Num() clear CurrentBuffList"));
	}

	for (int i = 0; i < PendingBuffList.Num(); i++)
	{
		if (i >= CurrentBuffList.Num())
		{
			if (PendingBuffList[i] == -1)
			{
				CurrentBuffList.Add(FBuffInfo(-1, nullptr, -1, -1));
			}
			else
			{
				int32 BuffHandle = AddBuffByID(PendingBuffList[i]);
				check(BuffHandle > -1);
			}
			continue;
		}
		if (PendingBuffList[i] == CurrentBuffList[i].BuffID)
			continue;
		if (PendingBuffList[i] == -1)
		{
			RemoveBuff_Internal(i);
		}
		//else
		//{
		//	if (CurrentBuffList[i].BuffID != -1)
		//	{
		//		RemoveBuff_Internal(i);
		//	}
		//	//int32 BuffHandle = AddBuffByID(PendingBuffList[i]);
		//	//check(BuffHandle > -1);
		//}

	}
#if !UE_BUILD_SHIPPING
	check(CurrentBuffList.Num() == PendingBuffList.Num());
#endif
}

int32 AGraveRPGBuffManagerBase::RemoveBuff_Internal(int32 Index)
{
	UE_LOG(LogActionRPG, Log,TEXT("RemoveBuff_Internal Index[%d] CurrentBuffList[%d]"), Index, CurrentBuffList.Num());
	CurrentBuffList[Index].Clear();
	if (Role == ROLE_Authority)
	{
		PendingBuffList[Index] = -1;
		ForceNetUpdate();
	}
	return Index;
}

bool AGraveRPGBuffManagerBase::RemoveBuffBySkillSessionID(int32 SkillSessionID)
{
	if (Role != ROLE_Authority)
	{
		return false;
	}
	UE_LOG(LogActionRPG, Log, TEXT("RemoveBuffBySkillSessionID SkillSessionID[%d] CurrentBuffList[%d]"), SkillSessionID, CurrentBuffList.Num());
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].SkillSessionID == SkillSessionID)
		{
			RemoveBuff_Internal(i);
		}
	}
	return true;
}

bool AGraveRPGBuffManagerBase::RemoveBuffByBuffSessionID(int32 BuffSessionID)
{
	UE_LOG(LogActionRPG, Log, TEXT("RemoveBuffByBuffSessionID BuffSessionID[%d] CurrentBuffList[%d]"), BuffSessionID, CurrentBuffList.Num());
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		if (CurrentBuffList[i].BuffSessionID == BuffSessionID)
		{
			RemoveBuff_Internal(i);
		}
	}
	return true;
}

bool AGraveRPGBuffManagerBase::RemoveAllBuff()
{
	UE_LOG(LogActionRPG, Log, TEXT("RemoveAllBuff"));
	for (int32 i = 0; i < CurrentBuffList.Num(); i++)
	{
		RemoveBuff_Internal(i);
	}
	return true;
}

void AGraveRPGBuffManagerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllBuff();
	CurrentBuffList.Empty();
	PendingBuffList.Empty();
	ForceNetUpdate();
	nmSkillAndBuff::CurrentBuffHandle = 0;
	Super::EndPlay(EndPlayReason);
	return;
}

