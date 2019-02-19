// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionRPG.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GraveRPGBuffImpBase.h"
#include "GraveRPGBuffManagerBase.generated.h"

USTRUCT()
struct ACTIONRPG_API FBuffRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		FString ClassName;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		float DelayStartTime;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		float DurationTime;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		FString ResName;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		int32 TargetType;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		FString Param1;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		FString Param2;
	UPROPERTY(BlueprintReadOnly, Category = "Buff")
		FString Param3;
};

USTRUCT(Blueprintable)
struct ACTIONRPG_API FBuffInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Buff")
		int32 BuffID;
	UPROPERTY(BlueprintReadWrite, Category = "Buff")
		UGraveRPGBuffImpBase* Buff;

	int32 BuffSessionID;
	int32 SkillSessionID;
	FBuffInfo() :
		BuffID(-1)
		, Buff(nullptr)
		, BuffSessionID(-1)
		, SkillSessionID(-1)
	{}

	FBuffInfo(int32 inBuffID, UGraveRPGBuffImpBase* inBuff, int32 inBuffSessionID, int32 inSkillSessionID) :
		BuffID(inBuffID)
		, Buff(inBuff)
		, BuffSessionID(inBuffSessionID)
		, SkillSessionID(inSkillSessionID)
	{

	}
	void Clear()
	{
		BuffID = -1;
		if (Buff)
		{
			Buff->Stop();
		}
		Buff = nullptr;
		BuffSessionID = -1;
		SkillSessionID = -1;
	}
};

UCLASS(Abstract, Blueprintable)
class ACTIONRPG_API AGraveRPGBuffManagerBase : public AActor
{
	GENERATED_BODY()

public:
	AGraveRPGBuffManagerBase();

	bool Init(const FString& TableName);

	/** Player Client Or Server */
	int32 AddBuffFromSkill(int32 BuffIndex, int32 SkillSessionID);

	UFUNCTION(exec)
		int32 AddBuffByID(int32 BuffID);

	bool RemoveBuffBySkillSessionID(int32 SkillSessionID);
	bool RemoveBuffByBuffSessionID(int32 BuffSessionID);

	/** ServerOnly */
	bool RemoveAllBuff();

	bool IsBuffActive(int BuffID);
	bool IsBuffActive(TSubclassOf<UGraveRPGBuffImpBase> BuffClass);
	bool IsBuffWaiting(int BuffID);
	bool IsBuffWaiting(TSubclassOf<UGraveRPGBuffImpBase> BuffClass);
	bool IsBuffAdded(int BuffID);
	bool IsBuffAdded(TSubclassOf<UGraveRPGBuffImpBase> BuffClass);
	int  GetBufferTargetType(int32 BuffID);

protected:
	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual bool FilterBuff(int32 BuffIndex);
	UFUNCTION()
		virtual void OnRep_PendingBuffer();

	int32 AddBuff_Internal(int32 BuffID, int32 SkillSessionID);
	int32 RemoveBuff_Internal(int32 BuffSessionID);

	bool LoadConfig();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void OnRep_Owner();
protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BuffTableName)
		FString BuffTableName;
private:
	UFUNCTION()
		virtual void OnRep_BuffTableName();

private:
	UPROPERTY(Transient)
		TArray<FBuffInfo> CurrentBuffList;
	
	/** 客户端服务器同步使用的Buff, 只允许比CurrentBuffList大 PendingBuffList 一场游戏内不进行remove*/
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PendingBuffer)
		TArray<int32> PendingBuffList;

	UPROPERTY()
		UDataTable* BuffTable;

	bool bInited;
};
