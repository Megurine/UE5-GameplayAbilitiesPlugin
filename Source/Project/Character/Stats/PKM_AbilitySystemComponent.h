// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PKM_AttributeSet.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_GameplayEffectActorComponent.h"
#include "GameplayAbilitieTest/PKM_Types.h"
#include "PKM_AbilitySystemComponent.generated.h"

//class UPKM_TimelineGameplayEffectComp;

USTRUCT(BlueprintType)
struct FGameplayEffectClassFilter
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayEffectAddedDelegate Delegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UGameplayEffect> EffectClassFilter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool allowChildClass = true;

	FGameplayEffectClassFilter() {}

	FGameplayEffectClassFilter(FGameplayEffectAddedDelegate _Delegate, TSubclassOf<UGameplayEffect> _EffectClassFilter, bool _allowChildClass)
	{
		Delegate = _Delegate;
		EffectClassFilter = _EffectClassFilter;
		allowChildClass = _allowChildClass;
	}
};

USTRUCT(BlueprintType)
struct FGameplayTagEventStruct
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FOnGameplayTagChanged Delegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EGameplayTagEventType::Type> EventType = EGameplayTagEventType::NewOrRemoved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool allowChildTag = false;

	FGameplayTagEventStruct() {}

	FGameplayTagEventStruct(FOnGameplayTagChanged _Delegate, FGameplayTag _Tag, EGameplayTagEventType::Type _EventType, bool _allowChildTag)
	{
		Delegate = _Delegate;
		Tag = _Tag;
		EventType = _EventType;
		allowChildTag = _allowChildTag;
	}
};

USTRUCT(BlueprintType)
struct FRegisterGameplayTagEventStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FGameplayTag Tag;
	TEnumAsByte<EGameplayTagEventType::Type> eventType = EGameplayTagEventType::NewOrRemoved;

	FRegisterGameplayTagEventStruct() {}

	FRegisterGameplayTagEventStruct(FGameplayTag _Tag, EGameplayTagEventType::Type _eventType)
	{
		Tag = _Tag;
		eventType = _eventType;
	}

	bool operator==(const FRegisterGameplayTagEventStruct& Other) const
	{
		return Tag == Other.Tag && eventType == Other.eventType;
	}

	bool operator!=(const FRegisterGameplayTagEventStruct& Other) const
	{
		return !(*this == Other);
	}

	friend inline uint32 GetTypeHash(const FRegisterGameplayTagEventStruct& MyStruct)
	{
		uint32 Hash = FCrc::MemCrc32(&MyStruct, sizeof(FRegisterGameplayTagEventStruct));
		return Hash;
	}
};

USTRUCT(BlueprintType)
struct FFNameAndFGameplayTag
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag Tag;

	FFNameAndFGameplayTag() {}

	FFNameAndFGameplayTag(FName _Name, FGameplayTag _Tag)
	{
		Name = _Name;
		Tag = _Tag;
	}
};

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEPLAYABILITIETEST_API UPKM_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public :
	
	UPKM_AbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
		void Init();

	const UPKM_AttributeSet* pkmAttributes;

	UFUNCTION(BlueprintPure, BlueprintCallable)
		FPKMAttributeCachedValues GetComputedAttributeCachedValues();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool GetShouldTick() const override;

	virtual void SetNumericAttribute_Internal(const FGameplayAttribute& Attribute, float& NewFloatValue) override;

	virtual void OnPeriodicGameplayEffectExecuteOnTarget(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle) override;

	UFUNCTION(BlueprintCallable)
		const FGameplayAttribute GetAttributeByEnum(EPKM_Attributes attribute, EPKM_AttributesType type) const;

	UFUNCTION(BlueprintCallable)
		void SetAttributeBaseValue(EPKM_Attributes attribute, float value, EPKM_AttributesType type, bool clamp, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
		void SetAttributeBaseValues(EPKM_Attributes attribute, float value, float min, float max, AActor* Instigator, bool clampValue = true, bool TriggerOnChangeIfEqualToBeforeValue = true);

	UFUNCTION(BlueprintCallable)
		void SetAttributeBaseValueLimitWithMultiplyValueBefore(EPKM_Attributes attribute, float value, EPKM_AttributesType type, AActor* Instigator, bool clamp = true, float multiplierValueBefore = 1.0f);

	UFUNCTION(BlueprintCallable)
		void SetAttributeBaseValueToMax(EPKM_Attributes attribute, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
		void SetAttributeBaseValueToMin(EPKM_Attributes attribute, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
		void AddAttributeBaseValue(EPKM_Attributes attribute, float valueToAdd, EPKM_AttributesType type, AActor* Instigator, EPKM_AddToAttributeType AddToAttributeType = EPKM_AddToAttributeType::TOBASEVALUE, bool clamp = true);

	UFUNCTION(BlueprintCallable)
		float GetAttributeCurrentValue(EPKM_Attributes attribute, EPKM_AttributesType type) const;

	UFUNCTION(BlueprintCallable)
		float GetAttributeCurrentValuePercent(EPKM_Attributes attribute) const;

	float ClampAttributeValue(EPKM_Attributes attribute, EPKM_AttributesType type, float value) const;

	UFUNCTION(BlueprintCallable, Category = Ability)
		FGameplayEffectSpecHandle CreateMagnitudeEffectSpec(TSubclassOf<UGameplayEffect> GameplayEffectClass, FGameplayEffectContextHandle Context, TMap<FGameplayTag, float> TagMagnitudes, float Level = 1.f) const;

	UFUNCTION(BlueprintCallable, Category = GameplayEffects)
		FActiveGameplayEffectHandle ApplyGameplayEffectToSelfWithTagMagnitude(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext, FGameplayTag TagMagnitude, float Magnitude);

	UFUNCTION(BlueprintCallable, Category = GameplayEffects)
		FActiveGameplayEffectHandle ApplyGameplayEffectToSelfWithTagMagnitudes(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext, TMap<FGameplayTag, float> TagMagnitudes);

	UFUNCTION(BlueprintCallable, Category = GameplayEffects)
		FActiveGameplayEffectHandle ApplyGameplayEffectToTargetWithTagMagnitude(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* Target, float Level, FGameplayEffectContextHandle EffectContext, FGameplayTag TagMagnitude, float Magnitude);

	UFUNCTION(BlueprintCallable, Category = GameplayEffects)
		FActiveGameplayEffectHandle ApplyGameplayEffectToTargetWithTagMagnitudes(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* Target, float Level, FGameplayEffectContextHandle EffectContext, TMap<FGameplayTag, float> TagMagnitudes);

#pragma region AttributeValueChange

	TMultiMap<FGameplayAttribute, FAttributeValueChangeDelegate> AttributeValueChangeDelegates;

	UFUNCTION(BlueprintCallable)
		void BindFunctionToAttributeValueChange(EPKM_Attributes attribute, EPKM_AttributesType type, FAttributeValueChangeDelegate InDelegate);

	UFUNCTION(BlueprintCallable)
		void UnBindFunctionToAttributeValueChange(EPKM_Attributes attribute, EPKM_AttributesType type, FAttributeValueChangeDelegate InDelegate);

	void OnAttributeValueChange(const FOnAttributeChangeData& Data);

#pragma endregion AttributeValueChange

#pragma region ActiveGameplayEffectAdded

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
		TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsAtInit;

	TArray<FGameplayEffectClassFilter> OnActiveGameplayEffectAddedDelegates;
	FDelegateHandle OnActiveGameplayEffectAddedDelegateHandle;

	UFUNCTION(BlueprintCallable)
		void BindFunctionToOnActiveGameplayEffectAdded(FGameplayEffectAddedDelegate InDelegate, TSubclassOf<UGameplayEffect> EffectClassFilter = nullptr, bool allowChildClass = true);

	UFUNCTION(BlueprintCallable)
		void UnBindFunctionToOnActiveGameplayEffectAdded(FGameplayEffectAddedDelegate InDelegate);

	UFUNCTION(BlueprintCallable)
		void UnBindFunctionToOnActiveGameplayEffectAddedForObject(const UObject* _Object);

	void RemoveOnActiveGameplayEffectAddedDelegateHandle();

	void OnActiveGameplayEffectAdd(UAbilitySystemComponent* AbilitySystemComp, const FGameplayEffectSpec& Data, FActiveGameplayEffectHandle Handle);
	void OnActiveGameplayEffectRemove(const FActiveGameplayEffect& EffectRemoved);

	TMap<uint32, UPKM_GameplayEffectActorComponent*> GameplayEffectActorComponents;

	FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey = FPredictionKey()) override;

	UPKM_GameplayEffectActorComponent* AddGameplayEffectActorComponent(TSubclassOf<UPKM_GameplayEffectActorComponent> GameplayEffectActorComponentClass, FActiveGameplayEffectHandle handle);

#pragma endregion ActiveGameplayEffectAdded

#pragma region Tags

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
		FGameplayTagContainer TagsAtInit;

	TArray<FGameplayTagEventStruct> OnGameplayTagEventDelegates;
	TMap<FRegisterGameplayTagEventStruct, FDelegateHandle> OnGameplayTagEventDelegateHandles;

	UFUNCTION(BlueprintCallable)
		void BindFunctionToGameplayTagEvent(FOnGameplayTagChanged Delegate, FGameplayTag Tag, bool allowChildTag = false, EGameplayTagEventType::Type eventType = EGameplayTagEventType::NewOrRemoved);

	UFUNCTION(BlueprintCallable)
		void UnbindFunctionToGameplayTagEventForTag(FOnGameplayTagChanged Delegate, FGameplayTag Tag, bool checkTag = false);

	void RemoveOnGameplayTagEventDelegateHandle(FGameplayTag Tag);

	void OnGameplayTagEventNewOrRemoved(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, bool TagAdded);
	void OnGameplayTagEventAnyCountChange(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, bool TagAdded);
	void OnGameplayTagEvent(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, bool TagAdded, EGameplayTagEventType::Type eventType);

#pragma endregion Tags

#pragma region Color

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
		FName ColorParameterNameTimelineGameplayEffectComp;

	UFUNCTION(BlueprintCallable)
		void SetValueToOwnerMaterialInstances(const FName ParameterName, const FVector value);

	void AddTimelineGameplayEffectCompColors(UPKM_TimelineGameplayEffectComp* Component);
	void RemoveTimelineGameplayEffectCompColors(UPKM_TimelineGameplayEffectComp* Component);

	void ComputeTimelineGameplayEffectCompColors();

	TArray<UPKM_TimelineGameplayEffectComp*> TimelineGameplayEffectCompColor;

private:

	int32 TimelineGameplayEffectCompColorCountCached = 0;

#pragma endregion Color

};
