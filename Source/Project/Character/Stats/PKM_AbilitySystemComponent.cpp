// Fill out your copyright notice in the Description page of Project Settings.

#include "PKM_AbilitySystemComponent.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_TimelineGameplayEffectComp.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_GameplayEffect.h"
#include "Components/Widget.h"

UPKM_AbilitySystemComponent::UPKM_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	AttributeChangeDatasObjectClass = UAttributeChangeDatasObject::StaticClass();
}

void UPKM_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bInitTagsAndEffectsOnBeginPlay)
	{
		InitTagsAndEffects();
	}
}

void UPKM_AbilitySystemComponent::Init()
{
	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UPKM_AbilitySystemComponent::OnActiveGameplayEffectRemove);

	UDataTable* DataTableTemp = NewObject<UDataTable>();
	pkmAttributes = Cast<UPKM_AttributeSet>(InitStats(UPKM_AttributeSet::StaticClass(), DataTableTemp));

	if (!bInitTagsAndEffectsOnBeginPlay)
	{
		InitTagsAndEffects();
	}
}

void UPKM_AbilitySystemComponent::InitTagsAndEffects()
{
	AddLooseGameplayTags(TagsAtInit);

	for (TSubclassOf<UGameplayEffect> GameplayEffectClass : GameplayEffectsAtInit)
	{
		if (GameplayEffectClass)
		{
			FGameplayEffectContextHandle EffectContext = MakeEffectContextFromInstigator(GetOwner());
			UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
			ApplyGameplayEffectToSelf(GameplayEffect, 0.0f, EffectContext);
		}
	}
}

FPKMAttributeCachedValues UPKM_AbilitySystemComponent::GetComputedAttributeCachedValues()
{
	return pkmAttributes->ComputeAttributeCachedValues();
}

void UPKM_AbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ComputeTimelineGameplayEffectCompColors();

	if (!GetShouldTick())
	{
		SetComponentTickEnabled(false);
	}
}

bool UPKM_AbilitySystemComponent::GetShouldTick() const
{
	return Super::GetShouldTick() ||
		(TimelineGameplayEffectCompColor.Num() > 0) ||    //if there is color to compute
		(TimelineGameplayEffectCompColorCountCached > 0); //or last frame there is a color
}

void UPKM_AbilitySystemComponent::SetNumericAttribute_Internal(const FGameplayAttribute& Attribute, float& NewFloatValue)
{
	FGameplayAttributeCouple attributeCouple = pkmAttributes->AttributeCouple[Attribute.AttributeName];
	if (attributeCouple.type == EPKM_AttributesType::VALUE)
	{
		NewFloatValue = ClampAttributeValue(attributeCouple.attribute, attributeCouple.type, NewFloatValue);
	}

	Super::SetNumericAttribute_Internal(Attribute, NewFloatValue);
}

void UPKM_AbilitySystemComponent::OnPeriodicGameplayEffectExecuteOnTarget(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle)
{
	Super::OnPeriodicGameplayEffectExecuteOnTarget(Target, SpecExecuted, ActiveHandle);

	uint32 handleId = GetTypeHash(ActiveHandle);
	if (handleId != -1)
	{
		if (GameplayEffectActorComponents.Contains(handleId))
		{
			UPKM_GameplayEffectActorComponent* GameplayEffectActorComponent = GameplayEffectActorComponents.FindRef(handleId);
			if (GameplayEffectActorComponent)
			{
				GameplayEffectActorComponent->PeriodicExecute();
			}
		}
	}
}

void UPKM_AbilitySystemComponent::OnPeriodicGameplayEffectExecuteOnSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle)
{
	Super::OnPeriodicGameplayEffectExecuteOnSelf(Source, SpecExecuted, ActiveHandle);

	uint32 handleId = GetTypeHash(ActiveHandle);
	if (handleId != -1)
	{
		if (GameplayEffectActorComponents.Contains(handleId))
		{
			UPKM_GameplayEffectActorComponent* GameplayEffectActorComponent = GameplayEffectActorComponents.FindRef(handleId);
			if (GameplayEffectActorComponent)
			{
				GameplayEffectActorComponent->PeriodicExecute();
			}
		}
	}
}

const FGameplayAttribute UPKM_AbilitySystemComponent::GetAttributeByEnum(EPKM_Attributes attribute, EPKM_AttributesType type) const
{
	return pkmAttributes->GetAttributeByEnum(attribute, type);
}

void UPKM_AbilitySystemComponent::SetAttributeBaseValue(EPKM_Attributes attribute, float value, EPKM_AttributesType type, bool clamp, AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("--- PKM_GAS ---"));
	//UE_LOG(LogTemp, Log, TEXT("PKM_GAS EPKM_Attributes : %s"), *UEnum::GetValueAsName(attribute).ToString());
	//UE_LOG(LogTemp, Log, TEXT("PKM_GAS EPKM_AttributesType : %s"), *UEnum::GetValueAsName(type).ToString());
	//UE_LOG(LogTemp, Log, TEXT("PKM_GAS Value : %f"), value);
	if (clamp)
	{
		value = ClampAttributeValue(attribute, type, value);
	}
	//UE_LOG(LogTemp, Log, TEXT("PKM_GAS Value clamped : %f"), value);
	//UE_LOG(LogTemp, Log, TEXT("--- PKM_GAS ---"));

	FGameplayAttribute attributeByEnum = GetAttributeByEnum(attribute, type);
	if (attributeByEnum.IsValid())
	{
		if (ShouldGenerateLastAttributeChangeDatas(attributeByEnum))
		{
			GenerateLastAttributeChangeDatas(Instigator);
		}

		SetNumericAttributeBase(attributeByEnum, value);

		if (type != EPKM_AttributesType::VALUE) //Update and clamp value if we changed min or max
		{
			SetAttributeBaseValue(attribute, GetAttributeCurrentValue(attribute, EPKM_AttributesType::VALUE), EPKM_AttributesType::VALUE, true, Instigator);
		}
	}
}

void UPKM_AbilitySystemComponent::SetAttributeBaseValues(EPKM_Attributes attribute, float value, float min, float max, AActor* Instigator, bool clampValue, bool TriggerOnChangeIfEqualToBeforeValue)
{
	float beforeValue = -1.0f;
	if (TriggerOnChangeIfEqualToBeforeValue)
	{
		beforeValue = GetAttributeCurrentValue(attribute, EPKM_AttributesType::VALUE);
	}

	SetAttributeBaseValue(attribute, min, EPKM_AttributesType::MINIMUM, false, Instigator);
	SetAttributeBaseValue(attribute, max, EPKM_AttributesType::MAXIMUM, false, Instigator);
	SetAttributeBaseValue(attribute, value, EPKM_AttributesType::VALUE, clampValue, Instigator);

	//Check if the base value from init 
	if (TriggerOnChangeIfEqualToBeforeValue)
	{
		float currentValue = GetAttributeCurrentValue(attribute, EPKM_AttributesType::VALUE);
		if (beforeValue == currentValue)
		{
			//Trigger the binding
			FOnGameplayAttributeValueChange& AttributeValueChange = GetGameplayAttributeValueChangeDelegate(GetAttributeByEnum(attribute, EPKM_AttributesType::VALUE));
			if (AttributeValueChange.IsBoundToObject(this))
			{
				FOnAttributeChangeData DataChanges = FOnAttributeChangeData();
				DataChanges.Attribute = GetAttributeByEnum(attribute, EPKM_AttributesType::VALUE);
				DataChanges.OldValue = -1.0f;
				DataChanges.NewValue = 0.0f;
				AttributeValueChange.Broadcast(DataChanges);
			}
		}
	}
}

void UPKM_AbilitySystemComponent::SetAttributeBaseValueLimitWithMultiplyValueBefore(EPKM_Attributes attribute, float value, EPKM_AttributesType type, AActor* Instigator, bool clamp, float multiplierValueBefore)
{
	if (type == EPKM_AttributesType::MINIMUM || type == EPKM_AttributesType::MAXIMUM)
	{
		float valueBefore = GetAttributeCurrentValue(attribute, EPKM_AttributesType::VALUE);
		float minBefore = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MINIMUM);
		float maxBefore = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MAXIMUM);
		float percent = (valueBefore - minBefore) / (maxBefore - minBefore) * multiplierValueBefore;

		if (clamp)
		{
			value = ClampAttributeValue(attribute, type, value);
		}

		if (type == EPKM_AttributesType::MINIMUM)
		{
			minBefore = value;
		}
		else if (type == EPKM_AttributesType::MAXIMUM)
		{
			maxBefore = value;
		}

		//Update min or max
		FGameplayAttribute attributeByEnum = GetAttributeByEnum(attribute, type);
		if (attributeByEnum.IsValid())
		{
			if (ShouldGenerateLastAttributeChangeDatas(attributeByEnum))
			{
				GenerateLastAttributeChangeDatas(Instigator);
			}

			SetNumericAttributeBase(attributeByEnum, value * multiplierValueBefore);
		}

		//Update value with percentage from old limits
		SetAttributeBaseValue(attribute, ((maxBefore - minBefore) * percent) + minBefore, EPKM_AttributesType::VALUE, true, Instigator);
	}
	else
	{
		//Error
		SetAttributeBaseValue(attribute, value * multiplierValueBefore, type, clamp, Instigator);
	}
}

void UPKM_AbilitySystemComponent::SetAttributeBaseValueToMax(EPKM_Attributes attribute, AActor* Instigator)
{
	float maxTemp = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MAXIMUM);
	if (maxTemp != -1)
	{
		SetAttributeBaseValue(attribute, maxTemp, EPKM_AttributesType::VALUE, true, Instigator);
	}
}

void UPKM_AbilitySystemComponent::SetAttributeBaseValueToMin(EPKM_Attributes attribute, AActor* Instigator)
{
	float minTemp = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MINIMUM);
	if (minTemp != -1)
	{
		SetAttributeBaseValue(attribute, minTemp, EPKM_AttributesType::VALUE, true, Instigator);
	}
}

void UPKM_AbilitySystemComponent::AddAttributeBaseValue(EPKM_Attributes attribute, float valueToAdd, EPKM_AttributesType type, AActor* Instigator, EPKM_AddToAttributeType AddToAttributeType, bool clamp)
{
	float currentValue;
	if (AddToAttributeType == EPKM_AddToAttributeType::TOBASEVALUE)
	{
		currentValue = GetNumericAttributeBase(GetAttributeByEnum(attribute, type));
	}
	else //EPKM_AddToAttributeType::TOCURRENTVALUE
	{
		currentValue = GetAttributeCurrentValue(attribute, type);
	}
	SetAttributeBaseValue(attribute, currentValue + valueToAdd, type, true, Instigator);
}

float UPKM_AbilitySystemComponent::GetAttributeCurrentValue(EPKM_Attributes attribute, EPKM_AttributesType type) const
{
	bool found;
	float value = GetGameplayAttributeValue(GetAttributeByEnum(attribute, type), found);

	if (found)
	{
		if (type == EPKM_AttributesType::VALUE)
		{
			value = ClampAttributeValue(attribute, type, value);
		}
		return value;
	}
	else
	{
		return -1;
	}
}

float UPKM_AbilitySystemComponent::GetAttributeCurrentValuePercent(EPKM_Attributes attribute) const
{
	bool foundValue;
	float value = GetGameplayAttributeValue(GetAttributeByEnum(attribute, EPKM_AttributesType::VALUE), foundValue);
	if (foundValue)
	{
		bool foundMax;
		float max = GetGameplayAttributeValue(GetAttributeByEnum(attribute, EPKM_AttributesType::MAXIMUM), foundMax);
		if (foundMax)
		{
			bool foundMin;
			float min = GetGameplayAttributeValue(GetAttributeByEnum(attribute, EPKM_AttributesType::MINIMUM), foundMin);
			if (!foundMin)
			{
				min = 0;
			}
			return (value - min) / (max - min);
		}
		else
		{
			return value;
		}
	}
	else
	{
		return -1;
	}
}

float UPKM_AbilitySystemComponent::ClampAttributeValue(EPKM_Attributes attribute, EPKM_AttributesType type, float value) const
{
	if (type == EPKM_AttributesType::VALUE)
	{
		float min = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MINIMUM);
		float max = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MAXIMUM);
		bool minInfinite = min == -1; //infinite min
		bool maxInfinite = max == -1; //infinite max
		if (!minInfinite && !maxInfinite)
		{
			return FMath::Clamp(value, min, max);
		}
		else if (minInfinite && maxInfinite)
		{
			return value;
		}
		else if (minInfinite)
		{
			return FMath::Min(value, max);
		}
		else if (maxInfinite)
		{
			return FMath::Max(value, min);
		}
	}
	else if (type == EPKM_AttributesType::MINIMUM)
	{
		float max = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MAXIMUM);
		bool maxInfinite = max == -1; //infinite max
		if (!maxInfinite)
		{
			return FMath::Min(value, max);
		}
		else
		{
			return value;
		}
	}
	else if (type == EPKM_AttributesType::MAXIMUM)
	{
		float min = GetAttributeCurrentValue(attribute, EPKM_AttributesType::MINIMUM);
		bool minInfinite = min == -1; //infinite min
		if (!minInfinite)
		{
			return FMath::Max(value, min);
		}
		else
		{
			return value;
		}
	}
	return value;
}

FGameplayEffectSpecHandle UPKM_AbilitySystemComponent::CreateMagnitudeEffectSpec(TSubclassOf<UGameplayEffect> GameplayEffectClass, FGameplayEffectContextHandle Context, TMap<FGameplayTag, float> TagMagnitudes, float Level) const
{
	if (!Context.IsValid())
	{
		Context = MakeEffectContextFromInstigator(GetOwner());
	}

	FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(GameplayEffectClass, Level, Context);
	if (NewHandle.IsValid())
	{
		NewHandle.Data->SetByCallerTagMagnitudes = TagMagnitudes;
	}

	return NewHandle;
}

FActiveGameplayEffectHandle UPKM_AbilitySystemComponent::ApplyGameplayEffectToSelfWithTagMagnitude(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext, FGameplayTag TagMagnitude, float Magnitude)
{
	if (GameplayEffectClass)
	{
		TMap<FGameplayTag, float> TagMagnitudes;
		TagMagnitudes.Add(TagMagnitude, Magnitude);
		return BP_ApplyGameplayEffectSpecToSelf(CreateMagnitudeEffectSpec(GameplayEffectClass, EffectContext, TagMagnitudes, Level));
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UPKM_AbilitySystemComponent::ApplyGameplayEffectToSelfWithTagMagnitudes(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext, TMap<FGameplayTag, float> TagMagnitudes)
{
	if (GameplayEffectClass)
	{
		return BP_ApplyGameplayEffectSpecToSelf(CreateMagnitudeEffectSpec(GameplayEffectClass, EffectContext, TagMagnitudes, Level));
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UPKM_AbilitySystemComponent::ApplyGameplayEffectToTargetWithTagMagnitude(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* Target, float Level, FGameplayEffectContextHandle EffectContext, FGameplayTag TagMagnitude, float Magnitude)
{
	if (GameplayEffectClass)
	{
		TMap<FGameplayTag, float> TagMagnitudes;
		TagMagnitudes.Add(TagMagnitude, Magnitude);
		return BP_ApplyGameplayEffectSpecToTarget(CreateMagnitudeEffectSpec(GameplayEffectClass, EffectContext, TagMagnitudes, Level), Target);
	}
	return FActiveGameplayEffectHandle();
}

FActiveGameplayEffectHandle UPKM_AbilitySystemComponent::ApplyGameplayEffectToTargetWithTagMagnitudes(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* Target, float Level, FGameplayEffectContextHandle EffectContext, TMap<FGameplayTag, float> TagMagnitudes)
{
	if (GameplayEffectClass)
	{
		return BP_ApplyGameplayEffectSpecToTarget(CreateMagnitudeEffectSpec(GameplayEffectClass, EffectContext, TagMagnitudes, Level), Target);
	}
	return FActiveGameplayEffectHandle();
}

bool UPKM_AbilitySystemComponent::ShouldGenerateLastAttributeChangeDatas(const FGameplayAttribute Attribute) const
{
	if (Attribute.AttributeName == "Hp")
	{
		return true;
	}
	return false;
}

void UPKM_AbilitySystemComponent::GenerateLastAttributeChangeDatas(AActor* Instigator)
{
	Super::GenerateLastAttributeChangeDatas(Instigator);
}

void UPKM_AbilitySystemComponent::GenerateLastAttributeChangeDatasWithSpec(const FGameplayEffectSpec& Spec)
{
	Super::GenerateLastAttributeChangeDatasWithSpec(Spec);
}

void UPKM_AbilitySystemComponent::BindFunctionToAttributeValueChange(EPKM_Attributes attribute, EPKM_AttributesType type, FAttributeValueChangeDelegate InDelegate)
{
	FGameplayAttribute Attribute = GetAttributeByEnum(attribute, type);
	if (Attribute.IsValid())
	{
		FOnGameplayAttributeValueChange& AttributeValueChange = GetGameplayAttributeValueChangeDelegate(Attribute);
		if (!AttributeValueChange.IsBoundToObject(this))
		{
			AttributeValueChange.AddUObject(this, &UPKM_AbilitySystemComponent::OnAttributeValueChange);
		}

		FArrayFAttributeValueChangeDelegate* ArrayStruct = AttributeValueChangeDelegates.Find(Attribute);
		if (ArrayStruct)
		{
			ArrayStruct->Delegates.Add(InDelegate);
		}
		else
		{
			AttributeValueChangeDelegates.Add(Attribute, FArrayFAttributeValueChangeDelegate({ InDelegate }));
		}
	}
}

void UPKM_AbilitySystemComponent::UnBindFunctionToAttributeValueChange(EPKM_Attributes attribute, EPKM_AttributesType type, FAttributeValueChangeDelegate InDelegate)
{
	FGameplayAttribute Attribute = GetAttributeByEnum(attribute, type);
	if (Attribute.IsValid())
	{
		FArrayFAttributeValueChangeDelegate* ArrayStruct = AttributeValueChangeDelegates.Find(Attribute);
		if (ArrayStruct)
		{
			if (ArrayStruct->Delegates.Remove(InDelegate) > 0)
			{
				AfterRemoveAttributeValueChange(Attribute);

				if (ArrayStruct->Delegates.Num() == 0)
				{
					AttributeValueChangeDelegates.Remove(Attribute);
				}
			}
		}
	}
}

void UPKM_AbilitySystemComponent::AfterRemoveAttributeValueChange(FGameplayAttribute Attribute)
{
	if (!AttributeValueChangeDelegates.Contains(Attribute))
	{
		FOnGameplayAttributeValueChange& AttributeValueChange = GetGameplayAttributeValueChangeDelegate(Attribute);
		if (AttributeValueChange.IsBoundToObject(this))
		{
			AttributeValueChange.RemoveAll(this);
		}
	}
}

void UPKM_AbilitySystemComponent::OnAttributeValueChange(const FOnAttributeChangeData& Data)
{
	bool atLeastOneRemoved = false;
	TArray<FAttributeValueChangeDelegate> Delegates;
	FArrayFAttributeValueChangeDelegate* ArrayStruct = AttributeValueChangeDelegates.Find(Data.Attribute);
	if (ArrayStruct)
	{
		Delegates = ArrayStruct->Delegates;
	}

	TArray<FAttributeValueChangeDelegate> DelegatesTempToExecute;
	TArray<float> newValues;
	TArray<float> oldValues;

	for (size_t i = 0; i < Delegates.Num(); i++)
	{
		bool ValidBound = true;

		if (Delegates[i].IsBound())
		{
			ValidBound = CheckIsNotBoundedOnDeadWidget(Delegates[i].GetUObject(), false);

			if (ValidBound)
			{
				float newValue = Data.NewValue;
				float oldValue = Data.OldValue;

				const FGameplayAttributeCouple* attributeCouple = pkmAttributes->AttributeCouple.Find(Data.Attribute.AttributeName);
				if (attributeCouple)
				{
					if (attributeCouple->type == EPKM_AttributesType::VALUE)
					{
						//newValue = ClampAttributeValue(attributeCouple->attribute, attributeCouple->type, newValue);
						bool found;
						newValue = GetGameplayAttributeValue(GetAttributeByEnum(attributeCouple->attribute, EPKM_AttributesType::VALUE), found);
						//oldValue = ClampAttributeValue(attributeCouple->attribute, oldValue);
					}
				}
				if (newValue != oldValue) //Optimization - Maybe remove this condition ?
				{
					//UAttributeChangeDatasObject* aaa = NewObject<UAttributeChangeDatasObject>(this, UAttributeChangeDatasObject::StaticClass());
					DelegatesTempToExecute.Add(Delegates[i]);
					newValues.Add(newValue);
					oldValues.Add(oldValue);
				}
			}
		}
		else
		{
			ValidBound = false;
		}

		if (!ValidBound)
		{
			Delegates[i].Clear();
			Delegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}

	//We process the temp delegate array to check if if they are still valid since the execution of previous delegate
	for (size_t i = 0; i < DelegatesTempToExecute.Num(); i++)
	{
		ArrayStruct = AttributeValueChangeDelegates.Find(Data.Attribute);
		if (ArrayStruct)
		{
			Delegates = ArrayStruct->Delegates;
		}

		if (Delegates.Contains(DelegatesTempToExecute[i]))
		{
			DelegatesTempToExecute[i].Execute(DelegatesTempToExecute[i].GetUObject(), newValues[i], oldValues[i], Data.ChangeDatas);
		}
	}

	if (atLeastOneRemoved)
	{
		AfterRemoveAttributeValueChange(Data.Attribute);
	}
}

void UPKM_AbilitySystemComponent::BindFunctionToOnActiveGameplayEffectAdded(FGameplayEffectAddedDelegate InDelegate, TSubclassOf<UGameplayEffect> EffectClassFilter, bool allowChildClass)
{
	if (!OnActiveGameplayEffectAddedDelegateHandle.IsValid())//OnGameplayEffectAppliedDelegateToSelf ou OnActiveGameplayEffectAddedDelegateToSelf
	{
		OnActiveGameplayEffectAddedDelegateHandle = OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UPKM_AbilitySystemComponent::OnActiveGameplayEffectAdd);
	}

	OnActiveGameplayEffectAddedDelegates.Add(FGameplayEffectClassFilter(InDelegate, EffectClassFilter, allowChildClass));
}

void UPKM_AbilitySystemComponent::UnBindFunctionToOnActiveGameplayEffectAdded(FGameplayEffectAddedDelegate InDelegate)
{
	bool atLeastOneRemoved = false;
	//Remove from array by delegate
	for (size_t i = 0; i < OnActiveGameplayEffectAddedDelegates.Num(); i++)
	{
		if (OnActiveGameplayEffectAddedDelegates[i].Delegate == InDelegate)
		{
			OnActiveGameplayEffectAddedDelegates[i].Delegate.Clear();
			OnActiveGameplayEffectAddedDelegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}
	if (atLeastOneRemoved)
	{
		RemoveOnActiveGameplayEffectAddedDelegateHandle();
	}
}

void UPKM_AbilitySystemComponent::UnBindFunctionToOnActiveGameplayEffectAddedForObject(const UObject* _Object)
{
	bool atLeastOneRemoved = false;
	//Remove from array by bounded object
	for (size_t i = 0; i < OnActiveGameplayEffectAddedDelegates.Num(); i++)
	{
		if (OnActiveGameplayEffectAddedDelegates[i].Delegate.IsBoundToObject(_Object))
		{
			OnActiveGameplayEffectAddedDelegates[i].Delegate.Clear();
			OnActiveGameplayEffectAddedDelegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}
	if (atLeastOneRemoved)
	{
		RemoveOnActiveGameplayEffectAddedDelegateHandle();
	}
}

void UPKM_AbilitySystemComponent::RemoveOnActiveGameplayEffectAddedDelegateHandle()
{
	if (OnActiveGameplayEffectAddedDelegates.Num() == 0)
	{
		if (OnActiveGameplayEffectAddedDelegateHandle.IsValid())
		{
			OnGameplayEffectAppliedDelegateToSelf.Remove(OnActiveGameplayEffectAddedDelegateHandle);
			OnActiveGameplayEffectAddedDelegateHandle.Reset();
		}
	}
}

void UPKM_AbilitySystemComponent::OnActiveGameplayEffectAdd(UAbilitySystemComponent* AbilitySystemComp, const FGameplayEffectSpec& Data, FActiveGameplayEffectHandle Handle)
{
	TSubclassOf<UGameplayEffect> EffectClass = nullptr;
	if (Data.Def)
	{
		EffectClass = Data.Def->GetClass();
	}

	TArray<FGameplayEffectClassFilter> DelegatesTempToExecute;
	bool atLeastOneRemoved = false;
	for (size_t i = 0; i < OnActiveGameplayEffectAddedDelegates.Num(); i++)
	{
		if (OnActiveGameplayEffectAddedDelegates[i].Delegate.IsBound())
		{
			bool filterValid = true;
			if (OnActiveGameplayEffectAddedDelegates[i].EffectClassFilter != nullptr)
			{
				if (OnActiveGameplayEffectAddedDelegates[i].EffectClassFilter != EffectClass)
				{
					if (OnActiveGameplayEffectAddedDelegates[i].allowChildClass)
					{
						if (!EffectClass.Get()->IsChildOf(OnActiveGameplayEffectAddedDelegates[i].EffectClassFilter))
						{
							filterValid = false;
						}
					}
					else
					{
						filterValid = false;
					}
				}
			}

			if (filterValid)
			{
				DelegatesTempToExecute.Add(OnActiveGameplayEffectAddedDelegates[i]);
			}
		}
		else
		{
			OnActiveGameplayEffectAddedDelegates[i].Delegate.Clear();
			OnActiveGameplayEffectAddedDelegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}

	//We process the temp delegate array to check if they are still valid since the execution of previous delegate
	for (size_t i = 0; i < DelegatesTempToExecute.Num(); i++)
	{
		if (OnActiveGameplayEffectAddedDelegates.Contains(DelegatesTempToExecute[i]))
		{
			DelegatesTempToExecute[i].Delegate.Execute(DelegatesTempToExecute[i].Delegate.GetUObject(), EffectClass, Handle, true);
		}
	}

	if (atLeastOneRemoved)
	{
		RemoveOnActiveGameplayEffectAddedDelegateHandle();
	}
}

void UPKM_AbilitySystemComponent::OnActiveGameplayEffectRemove(const FActiveGameplayEffect& EffectRemoved)
{
	uint32 handleId = GetTypeHash(EffectRemoved.Handle);
	if (handleId != -1)
	{
		if (GameplayEffectActorComponents.Contains(handleId))
		{
			UPKM_GameplayEffectActorComponent* GameplayEffectActorComponent = GameplayEffectActorComponents.FindAndRemoveChecked(handleId);
			if (GameplayEffectActorComponent)
			{
				GameplayEffectActorComponent->DestroyComponent();
			}
		}
	}

	TSubclassOf<UGameplayEffect> EffectClass = nullptr;
	if (EffectRemoved.Spec.Def)
	{
		//FActiveGameplayEffectHandle = EffectRemoved.Handle;
		EffectClass = EffectRemoved.Spec.Def->GetClass();
	}

	TArray<FGameplayEffectClassFilter> DelegatesTempToExecute;
	bool atLeastOneRemoved = false;
	for (size_t i = 0; i < OnActiveGameplayEffectAddedDelegates.Num(); i++)
	{
		if (OnActiveGameplayEffectAddedDelegates[i].Delegate.IsBound())
		{
			bool filterValid = true;
			if (OnActiveGameplayEffectAddedDelegates[i].EffectClassFilter != nullptr)
			{
				if (OnActiveGameplayEffectAddedDelegates[i].EffectClassFilter != EffectClass)
				{
					if (OnActiveGameplayEffectAddedDelegates[i].allowChildClass)
					{
						if (!EffectClass.Get()->IsChildOf(OnActiveGameplayEffectAddedDelegates[i].EffectClassFilter))
						{
							filterValid = false;
						}
					}
					else
					{
						filterValid = false;
					}
				}
			}

			if (filterValid)
			{
				DelegatesTempToExecute.Add(OnActiveGameplayEffectAddedDelegates[i]);
			}
		}
		else
		{
			OnActiveGameplayEffectAddedDelegates[i].Delegate.Clear();
			OnActiveGameplayEffectAddedDelegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}

	//We process the temp delegate array to check if they are still valid since the execution of previous delegate
	for (size_t i = 0; i < DelegatesTempToExecute.Num(); i++)
	{
		if (OnActiveGameplayEffectAddedDelegates.Contains(DelegatesTempToExecute[i]))
		{
			DelegatesTempToExecute[i].Delegate.Execute(DelegatesTempToExecute[i].Delegate.GetUObject(), EffectClass, EffectRemoved.Handle, false);
		}
	}

	if (atLeastOneRemoved)
	{
		RemoveOnActiveGameplayEffectAddedDelegateHandle();
	}
}

FActiveGameplayEffectHandle UPKM_AbilitySystemComponent::ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{
	FActiveGameplayEffectHandle handle = Super::ApplyGameplayEffectSpecToSelf(Spec, PredictionKey);

	if (Spec.Def)
	{
		const UPKM_GameplayEffect* pkmGameplayEffect = Cast<UPKM_GameplayEffect>(Spec.Def);
		if (pkmGameplayEffect)
		{
			pkmGameplayEffect->Init(this, handle);
			//this->FindActiveGameplayEffectHandle
		}
	}
	return handle;
}

UPKM_GameplayEffectActorComponent* UPKM_AbilitySystemComponent::AddGameplayEffectActorComponent(TSubclassOf<UPKM_GameplayEffectActorComponent> GameplayEffectActorComponentClass, FActiveGameplayEffectHandle handle)
{
	if (GameplayEffectActorComponentClass)
	{
		UPKM_GameplayEffectActorComponent* GameplayEffectActorComponent = Cast<UPKM_GameplayEffectActorComponent>(GetOwner()->AddComponentByClass(GameplayEffectActorComponentClass, false, FTransform(), false));
		if (GameplayEffectActorComponent)
		{
			GameplayEffectActorComponent->Init(this, handle);

			uint32 GameplayEffectHandleId = GetTypeHash(handle);
			if (GameplayEffectHandleId != -1)
			{
				GameplayEffectActorComponents.Add(GameplayEffectHandleId, GameplayEffectActorComponent);
			}
		}
		return GameplayEffectActorComponent;
	}
	return nullptr;
}

void UPKM_AbilitySystemComponent::BindFunctionToGameplayTagEvent(FOnGameplayTagChanged Delegate, FGameplayTag Tag, bool allowChildTag, EGameplayTagEventType::Type eventType)
{
	FRegisterGameplayTagEventStruct RegisterGameplayTagEventStruct = FRegisterGameplayTagEventStruct(Tag, eventType);

	bool contains = OnGameplayTagEventDelegateHandles.Contains(RegisterGameplayTagEventStruct);
	if(!contains)
	{
		//UE_LOG(LogTemp, Log, TEXT("PKM_GAS BindFunctionToGameplayTagEvent RegisterGameplayTagEvent"));
		FDelegateHandle DelegateHandle;
		switch (eventType)
		{
		case EGameplayTagEventType::NewOrRemoved:
			DelegateHandle = RegisterGameplayTagEvent(Tag, eventType).AddUObject(this, &UPKM_AbilitySystemComponent::OnGameplayTagEventNewOrRemoved);
			break;
		case EGameplayTagEventType::AnyCountChange:
			DelegateHandle = RegisterGameplayTagEvent(Tag, eventType).AddUObject(this, &UPKM_AbilitySystemComponent::OnGameplayTagEventAnyCountChange);
			break;
		}
		OnGameplayTagEventDelegateHandles.Add(RegisterGameplayTagEventStruct, DelegateHandle);
	}

	OnGameplayTagEventDelegates.Add(FGameplayTagEventStruct(Delegate, Tag, eventType, allowChildTag));
}

void UPKM_AbilitySystemComponent::UnbindFunctionToGameplayTagEventForTag(FOnGameplayTagChanged Delegate, FGameplayTag Tag, bool checkTag)
{
	bool atLeastOneRemoved = false;
	for (size_t i = 0; i < OnGameplayTagEventDelegates.Num(); i++)
	{
		if (OnGameplayTagEventDelegates[i].Delegate == Delegate && (!checkTag || OnGameplayTagEventDelegates[i].Tag == Tag))
		{
			OnGameplayTagEventDelegates[i].Delegate.Clear();
			OnGameplayTagEventDelegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}
	if (atLeastOneRemoved)
	{
		RemoveOnGameplayTagEventDelegateHandle(Tag);
	}
}

void UPKM_AbilitySystemComponent::RemoveOnGameplayTagEventDelegateHandle(FGameplayTag Tag)
{
	//On compte si on trouve des delegate li�s a ce delegate et ce tag, si il n'y en a pas au final, on peut virer l'event de OnGameplayTagEventDelegateHandles
	int32 count = 0;
	for (size_t i = 0; i < OnGameplayTagEventDelegates.Num(); i++)
	{
		if (OnGameplayTagEventDelegates[i].Tag == Tag)
		{
			count++;
		}
	}
	if (count == 0)
	{
		FRegisterGameplayTagEventStruct RegisterGameplayTagEventStructAnyCount = FRegisterGameplayTagEventStruct(Tag, EGameplayTagEventType::AnyCountChange);
		FDelegateHandle DelegateHandle = OnGameplayTagEventDelegateHandles.FindRef(RegisterGameplayTagEventStructAnyCount);
		if (DelegateHandle.IsValid())
		{
			RegisterGameplayTagEvent(Tag, EGameplayTagEventType::AnyCountChange).Remove(DelegateHandle);
			DelegateHandle.Reset();
			OnGameplayTagEventDelegateHandles.Remove(RegisterGameplayTagEventStructAnyCount);
		}
		FRegisterGameplayTagEventStruct RegisterGameplayTagEventStructNewOrRemoved = FRegisterGameplayTagEventStruct(Tag, EGameplayTagEventType::NewOrRemoved);
		FDelegateHandle DelegateHandleNewOrRemoved = OnGameplayTagEventDelegateHandles.FindRef(RegisterGameplayTagEventStructNewOrRemoved);
		if (DelegateHandleNewOrRemoved.IsValid())
		{
			RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).Remove(DelegateHandleNewOrRemoved);
			DelegateHandleNewOrRemoved.Reset();
			OnGameplayTagEventDelegateHandles.Remove(RegisterGameplayTagEventStructNewOrRemoved);
		}
	}
}

void UPKM_AbilitySystemComponent::OnGameplayTagEventNewOrRemoved(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, EOnGameplayEffectTagCountOperation TagOperation)
{
	//FString aaa = TriggerTag.ToString();
	//UE_LOG(LogTemp, Log, TEXT("PKM_GAS OnGameplayTagEventNewOrRemoved : %s"), *aaa);
	OnGameplayTagEvent(CallbackTag, NewCount, TriggerTag, TagOperation, EGameplayTagEventType::NewOrRemoved);
}

void UPKM_AbilitySystemComponent::OnGameplayTagEventAnyCountChange(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, EOnGameplayEffectTagCountOperation TagOperation)
{
	//FString aaa = TriggerTag.ToString();
	//UE_LOG(LogTemp, Log, TEXT("PKM_GAS OnGameplayTagEventAnyCountChange : %s"), *aaa);
	OnGameplayTagEvent(CallbackTag, NewCount, TriggerTag, TagOperation, EGameplayTagEventType::AnyCountChange);
}

void UPKM_AbilitySystemComponent::OnGameplayTagEvent(const FGameplayTag CallbackTag, int32 NewCount, const FGameplayTag TriggerTag, EOnGameplayEffectTagCountOperation TagOperation, EGameplayTagEventType::Type eventType)
{
	TArray<FGameplayTagEventStruct> OnGameplayTagEventDelegatesTempToExecute;
	bool atLeastOneRemoved = false;
	for (size_t i = 0; i < OnGameplayTagEventDelegates.Num(); i++)
	{
		bool ValidBound = true;

		if (OnGameplayTagEventDelegates[i].Delegate.IsBound())
		{
			ValidBound = CheckIsNotBoundedOnDeadWidget(OnGameplayTagEventDelegates[i].Delegate.GetUObject(), true);

			if (ValidBound)
			{
				//We'll see what to do with this condition in the future, if there isn't something useful triggered on InternalCheck. Megu
				if (TagOperation != EOnGameplayEffectTagCountOperation::INTERNALCHECK)
				{
					bool valid = false;

					if (OnGameplayTagEventDelegates[i].EventType == eventType)
					{
						if (OnGameplayTagEventDelegates[i].Tag == TriggerTag)
						{
							valid = true;
						}
						else
						{
							if (OnGameplayTagEventDelegates[i].allowChildTag)
							{
								valid = TriggerTag.MatchesTag(OnGameplayTagEventDelegates[i].Tag);
							}
						}
					}
					if (valid)
					{
						//We add delegate to a temp array because delegate can be removed due to an other delegate binding flow
						OnGameplayTagEventDelegatesTempToExecute.Add(OnGameplayTagEventDelegates[i]);
					}
				}
				/*else
				{
					ensure(false); //Ensure for Megu to understand when InternalCheck is used.
				}*/
			}
		}
		else
		{
			ValidBound = false;
		}

		if (!ValidBound)
		{
			OnGameplayTagEventDelegates[i].Delegate.Clear();
			OnGameplayTagEventDelegates.RemoveAt(i);
			atLeastOneRemoved = true;
			i--;
		}
	}

	//We process the temp delegate array to check if they are still valid since the execution of previous delegate
	for (size_t i = 0; i < OnGameplayTagEventDelegatesTempToExecute.Num(); i++)
	{
		if (OnGameplayTagEventDelegates.Contains(OnGameplayTagEventDelegatesTempToExecute[i]))
		{
			//UE_LOGFMT(LogTemp, Log, "MEGU Execute OnGameplayTagEvent to {o} for {t} from {f}", ("o", GetOwner()->GetName()), ("t", TriggerTag.ToString()), ("f", GetDebugStringUObject(OnGameplayTagEventDelegatesTempToExecute[i].Delegate.GetUObject())));
			OnGameplayTagEventDelegatesTempToExecute[i].Delegate.Execute(OnGameplayTagEventDelegatesTempToExecute[i].Delegate.GetUObject(), CallbackTag, (TagOperation == EOnGameplayEffectTagCountOperation::ADDED), NewCount, TriggerTag);
		}
	}

	if (atLeastOneRemoved)
	{
		RemoveOnGameplayTagEventDelegateHandle(CallbackTag);
	}
}

bool UPKM_AbilitySystemComponent::CheckIsNotBoundedOnDeadWidget(UObject* ObjectBound, bool CallForTag)
{
#if WITH_EDITOR || UE_BUILD_DEVELOPMENT
	//Usefull check to force GC to remove the widget after being removed from parent and the unbinding has not been done.
	//Set with WITH_EDITOR or UE_BUILD_DEVELOPMENT to optimize the game with UE_BUILD_SHIPPING.
	UWidget* BoundedWidget = Cast<UWidget>(ObjectBound);
	if (BoundedWidget)
	{
		if (!BoundedWidget->IsConstructed()) //return false after being removed from parent
		{
			//You've called BindFunctionToGameplayTagEvent in your widget, but forgot the unbind in its Destroy(). JeromeC
			if (CallForTag)
			{
				ensureAlwaysMsgf(false, TEXT("Please call UnbindFunctionToGameplayTagEventForTag in widget (%s)'s Destroy()."), *BoundedWidget->GetFName().ToString());
			}
			else
			{
				ensureAlwaysMsgf(false, TEXT("Please call UnBindFunctionToAttributeValueChange in widget (%s)'s Destroy()."), *BoundedWidget->GetFName().ToString());
			}
			return false;
		}
	}
#endif
	return true;
}

FString UPKM_AbilitySystemComponent::GetDebugStringUObject(UObject* object)
{
	if (object)
	{
		UActorComponent* actorComponent = Cast<UActorComponent>(object);
		if (actorComponent)
		{
			return actorComponent->GetName() + " " + actorComponent->GetOwner()->GetName();
		}
		else
		{
			return object->GetName();
		}
	}
	else
	{
		return "nullptr or ispendingkill";
	}
}

void UPKM_AbilitySystemComponent::SetValueToOwnerMaterialInstances(const FName ParameterName, const FVector value)
{
	AActor* owner = GetOwner();
	if (owner)
	{
		/*TArray<UPrimitiveComponent*> Components;
		owner->GetComponents<UPrimitiveComponent>(Components);
		for (UPrimitiveComponent* Component : Components)
		{
			int32 num = Component->GetNumMaterials();
			for (size_t i = 0; i < num; i++)
			{
				UMaterialInstanceDynamic* Mat = Cast<UMaterialInstanceDynamic>(Component->GetMaterial(i));
				if (Mat)
				{
					Mat->SetScalarParameterValue(ParameterName, value);
				}
			}
		}*/
		TArray<UMeshComponent*> Components;
		owner->GetComponents<UMeshComponent>(Components);
		for (UMeshComponent* Component : Components)
		{
			Component->SetVectorParameterValueOnMaterials(ParameterName, value);
		}
	}
}

void UPKM_AbilitySystemComponent::AddTimelineGameplayEffectCompColors(UPKM_TimelineGameplayEffectComp* Component)
{
	if (Component)
	{
		TimelineGameplayEffectCompColor.Add(Component);
		SetComponentTickEnabled(true);
	}
}

void UPKM_AbilitySystemComponent::RemoveTimelineGameplayEffectCompColors(UPKM_TimelineGameplayEffectComp* Component)
{
	TimelineGameplayEffectCompColor.Remove(Component);
}

void UPKM_AbilitySystemComponent::ComputeTimelineGameplayEffectCompColors()
{
	bool colorSet = false;
	if (TimelineGameplayEffectCompColor.Num() > 0)
	{
		float dividerFloat = 0.0f;
		FVector colorAvg = FVector::ZeroVector;
		for (size_t i = 0; i < TimelineGameplayEffectCompColor.Num(); i++)
		{
			if (TimelineGameplayEffectCompColor[i])
			{
				FLinearColor color = TimelineGameplayEffectCompColor[i]->ColorOnOwner;
				float valueColor = TimelineGameplayEffectCompColor[i]->cachedValue;
				colorAvg += FVector(color.R * valueColor, color.G * valueColor, color.B * valueColor);
				dividerFloat += valueColor;
			}
			else
			{
				TimelineGameplayEffectCompColor.RemoveAt(i);
			}
		}
		int32 divider = TimelineGameplayEffectCompColor.Num();
		if (dividerFloat > 0)
		{
			//colorAvg = colorAvg / dividerFloat;
			SetValueToOwnerMaterialInstances(ColorParameterNameTimelineGameplayEffectComp, colorAvg);
			TimelineGameplayEffectCompColorCountCached = divider;
			colorSet = true;
		}
	}
	if(!colorSet)//if no color set, check if there is color at previous tick, if there is then reset the color
	{
		if (TimelineGameplayEffectCompColorCountCached > 0)
		{
			SetValueToOwnerMaterialInstances(ColorParameterNameTimelineGameplayEffectComp, FVector::ZeroVector);
			TimelineGameplayEffectCompColorCountCached = 0;
		}
	}
}

bool UPKM_AbilitySystemComponent::SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration)
{
	if (!Handle.IsValid())
	{
		return false;
	}

	FActiveGameplayEffect* AGE = const_cast<FActiveGameplayEffect*>(GetActiveGameplayEffect(Handle));
	if (!AGE)
		return false;

	if (NewDuration > 0)
	{
		AGE->Spec.Duration = NewDuration;
	}
	else
	{
		AGE->Spec.Duration = 0.01f;
	}
	ActiveGameplayEffects.CheckDuration(Handle);

	AGE->EventSet.OnTimeChanged.Broadcast(AGE->Handle, AGE->StartWorldTime, AGE->GetDuration());
	OnGameplayEffectDurationChange(*AGE);

	return true;
}