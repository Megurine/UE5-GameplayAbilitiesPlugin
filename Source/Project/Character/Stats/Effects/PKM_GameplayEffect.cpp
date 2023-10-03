// Fill out your copyright notice in the Description page of Project Settings.


#include "PKM_GameplayEffect.h"

void UPKM_GameplayEffect::Init(UPKM_AbilitySystemComponent* AbilitySystemComp, FActiveGameplayEffectHandle handle) const
{
	/*FString nameEffet = GetFName().ToString();
	FString handleIdText = FString::FromInt(GameplayEffectHandleId);
	UE_LOG(LogTemp, Warning, TEXT("PKM_GameplayEffect::Init - %s (HandleId : %s )"), *nameEffet, *handleIdText);*/

	UPKM_GameplayEffectActorComponent* GameplayEffectActorCompAdded = nullptr;
	TSubclassOf<UPKM_GameplayEffectActorComponent> GameplayEffectClassLoaded = GameplayEffectComponentClass.LoadSynchronous();
	if (GameplayEffectClassLoaded)
	{
		GameplayEffectActorCompAdded = AbilitySystemComp->AddGameplayEffectActorComponent(GameplayEffectClassLoaded, handle);
	}

	if (GetTypeHash(handle) == -1)
	{
		if (GameplayEffectActorCompAdded)
		{
			//Supprimer direct le component créér (mais le laisser quand meme spawn pour qu'il fasse son init et donc des choses dedans)
			GameplayEffectActorCompAdded->DestroyComponent();
		}
	}
}

void UPKM_GameplayEffect::EventAtStart(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const
{
	EventAtStartBP(AbilitySystemComponentTarget, Instigator);
}

void UPKM_GameplayEffect::EventAtPeriod(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const
{
	EventAtPeriodBP(AbilitySystemComponentTarget, Instigator);
}

void UPKM_GameplayEffect::EventAtEnd(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const
{
	EventAtEndBP(AbilitySystemComponentTarget, Instigator);
}

TSoftObjectPtr<UTexture> UPKM_GameplayEffect::GetClassVariableIcon(TSubclassOf<UPKM_GameplayEffect> _effectClass)
{
	if (UPKM_GameplayEffect* DefaultActor = Cast<UPKM_GameplayEffect>(_effectClass->GetDefaultObject(true)))
	{
		return DefaultActor->Icon;
	}
	return nullptr;
};

EEffectWidgetVisibilityType UPKM_GameplayEffect::GetClassVariableWidgetVisibilityType(TSubclassOf<UPKM_GameplayEffect> _effectClass)
{
	if (UPKM_GameplayEffect* DefaultActor = Cast<UPKM_GameplayEffect>(_effectClass->GetDefaultObject(true)))
	{
		return DefaultActor->WidgetVisibilityType;
	}
	return EEffectWidgetVisibilityType::INVISIBLE;
};

EGameplayEffectDurationType UPKM_GameplayEffect::GetClassVariableDurationPolicy(TSubclassOf<UPKM_GameplayEffect> _effectClass)
{
	if (UPKM_GameplayEffect* DefaultActor = Cast<UPKM_GameplayEffect>(_effectClass->GetDefaultObject(true)))
	{
		return DefaultActor->DurationPolicy;
	}
	return EGameplayEffectDurationType::Instant;
};