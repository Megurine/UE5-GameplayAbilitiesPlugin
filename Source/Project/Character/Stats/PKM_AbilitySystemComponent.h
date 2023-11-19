// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PKM_AttributeSet.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_GameplayEffectActorComponent.h"
#include "GameplayAbilitieTest/PKM_Types.h"
#include "PKM_AbilitySystemComponent.generated.h"

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

	bool operator==(const FGameplayEffectClassFilter& Other) const
	{
		return Delegate == Other.Delegate && EffectClassFilter == Other.EffectClassFilter && allowChildClass == Other.allowChildClass;
	}

	bool operator!=(const FGameplayEffectClassFilter& Other) const
	{
		return !(*this == Other);
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

	bool operator==(const FGameplayTagEventStruct& Other) const
	{
		return Delegate == Other.Delegate && Tag == Other.Tag && EventType == Other.EventType && allowChildTag == Other.allowChildTag;
	}

	bool operator!=(const FGameplayTagEventStruct& Other) const
	{
		return !(*this == Other);
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

protected:

	virtual void BeginPlay() override;

public :
	
	UPKM_AbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
		void Init();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
		bool bInitTagsAndEffectsOnBeginPlay = false;

private:

	void InitTagsAndEffects();

public:

	const UPKM_AttributeSet* pkmAttributes;

	UFUNCTION(BlueprintPure, BlueprintCallable)
		FPKMAttributeCachedValues GetComputedAttributeCachedValues();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool GetShouldTick() const override;

	virtual void SetNumericAttribute_Internal(const FGameplayAttribute& Attribute, float& NewFloatValue) override;

	virtual void OnPeriodicGameplayEffectExecuteOnTarget(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle) override;
	virtual void OnPeriodicGameplayEffectExecuteOnSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle) override;

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

public:

	virtual bool ShouldGenerateLastAttributeChangeDatas(const FGameplayAttribute Attribute) const override;
	virtual void GenerateLastAttributeChangeDatas(AActor* Instigator) override;
	virtual void GenerateLastAttributeChangeDatasWithSpec(const FGameplayEffectSpec& Spec) override;

	UFUNCTION(BlueprintCallable)
		void BindFunctionToAttributeValueChange(EPKM_Attributes attribute, EPKM_AttributesType type, FAttributeValueChangeDelegate InDelegate);

	UFUNCTION(BlueprintCallable)
		void UnBindFunctionToAttributeValueChange(EPKM_Attributes attribute, EPKM_AttributesType type, FAttributeValueChangeDelegate InDelegate);

private : 

	TMultiMap<FGameplayAttribute, FAttributeValueChangeDelegate> AttributeValueChangeDelegates;

	void AfterRemoveAttributeValueChange(FGameplayAttribute Attribute);
	void OnAttributeValueChange(const FOnAttributeChangeData& Data);

#pragma endregion AttributeValueChange

#pragma region ActiveGameplayEffectAdded

public :

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
		TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsAtInit;

	/** Bind a BP function delegate to the ASC effect events */
	UFUNCTION(BlueprintCallable)
		void BindFunctionToOnActiveGameplayEffectAdded(FGameplayEffectAddedDelegate InDelegate, TSubclassOf<UGameplayEffect> EffectClassFilter = nullptr, bool allowChildClass = true);

	/** Unbind a BP function delegate to the ASC effect events */
	UFUNCTION(BlueprintCallable)
		void UnBindFunctionToOnActiveGameplayEffectAdded(FGameplayEffectAddedDelegate InDelegate);

	/** Unbind a BP function delegate to the ASC effect events */
	UFUNCTION(BlueprintCallable)
		void UnBindFunctionToOnActiveGameplayEffectAddedForObject(const UObject* _Object);

	UPKM_GameplayEffectActorComponent* AddGameplayEffectActorComponent(TSubclassOf<UPKM_GameplayEffectActorComponent> GameplayEffectActorComponentClass, FActiveGameplayEffectHandle handle);

private: 

	TArray<FGameplayEffectClassFilter> OnActiveGameplayEffectAddedDelegates;
	FDelegateHandle OnActiveGameplayEffectAddedDelegateHandle;

	void RemoveOnActiveGameplayEffectAddedDelegateHandle();

	void OnActiveGameplayEffectAdd(UAbilitySystemComponent* AbilitySystemComp, const FGameplayEffectSpec& Data, FActiveGameplayEffectHandle Handle);
	void OnActiveGameplayEffectRemove(const FActiveGameplayEffect& EffectRemoved);

	TMap<uint32, UPKM_GameplayEffectActorComponent*> GameplayEffectActorComponents;

	FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey = FPredictionKey()) override;

#pragma endregion ActiveGameplayEffectAdded

#pragma region Tags

public : 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
		FGameplayTagContainer TagsAtInit;

	/** Bind a BP function delegate to the ASC tag events */
	UFUNCTION(BlueprintCallable, Category = "Tags")
		void BindFunctionToGameplayTagEvent(FOnGameplayTagChanged Delegate, FGameplayTag Tag, bool allowChildTag = false, EGameplayTagEventType::Type eventType = EGameplayTagEventType::NewOrRemoved);

	/** Unbind a BP function delegate to the ASC tag events */
	UFUNCTION(BlueprintCallable, Category = "Tags")
		void UnbindFunctionToGameplayTagEventForTag(FOnGameplayTagChanged Delegate, FGameplayTag Tag, bool checkTag = false);

private : 

	TArray<FGameplayTagEventStruct> OnGameplayTagEventDelegates;
	TMap<FRegisterGameplayTagEventStruct, FDelegateHandle> OnGameplayTagEventDelegateHandles;

	/** Internaly remove the */
	void RemoveOnGameplayTagEventDelegateHandle(FGameplayTag Tag);

	void OnGameplayTagEventNewOrRemoved(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, EOnGameplayEffectTagCountOperation TagOperation);
	void OnGameplayTagEventAnyCountChange(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, EOnGameplayEffectTagCountOperation TagOperation);
	void OnGameplayTagEvent(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, EOnGameplayEffectTagCountOperation TagOperation, EGameplayTagEventType::Type eventType);

	bool CheckIsNotBoundedOnDeadWidget(UObject* ObjectBound, bool CallForTag);

	FString GetDebugStringUObject(UObject* object);

#pragma endregion Tags

#pragma region Color

public :

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
		FName ColorParameterNameTimelineGameplayEffectComp;

	UFUNCTION(BlueprintCallable)
		void SetValueToOwnerMaterialInstances(const FName ParameterName, const FVector value);

	void AddTimelineGameplayEffectCompColors(UPKM_TimelineGameplayEffectComp* Component);
	void RemoveTimelineGameplayEffectCompColors(UPKM_TimelineGameplayEffectComp* Component);

	void ComputeTimelineGameplayEffectCompColors();

	TArray<UPKM_TimelineGameplayEffectComp*> TimelineGameplayEffectCompColor;

	int32 TimelineGameplayEffectCompColorCountCached = 0;

#pragma endregion Color

};
