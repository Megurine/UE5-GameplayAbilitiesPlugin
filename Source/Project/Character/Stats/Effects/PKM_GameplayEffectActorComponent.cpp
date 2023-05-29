// Fill out your copyright notice in the Description page of Project Settings.

#include "PKM_GameplayEffectActorComponent.h"
#include "GameplayAbilitieTest/Character/Stats/PKM_AbilitySystemComponent.h"
#include "GameplayAbilitieTest/FX/FXBlueprintFunctionLibrary.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_TimelineGameplayEffectComp.h"



UPKM_GameplayEffectActorComponent::UPKM_GameplayEffectActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPKM_GameplayEffectActorComponent::Init(UPKM_AbilitySystemComponent* _PKMAbilitySystemComponent, FActiveGameplayEffectHandle handle)
{
	ActiveGameplayEffectHandle = handle;
	PKMAbilitySystemComponent = _PKMAbilitySystemComponent;

	if (ParticleSettingsStart.ParticlesTemplate)
	{
		FXComponentStart = UFXBlueprintFunctionLibrary::CreatePKMFXComponent(GetOwner(), TEXT("StartFX"), ParticleSettingsStart);
	}

	if (PKMAbilitySystemComponent)
	{
		PKMAbilitySystemComponent->GetGameplayEffectStartTimeAndDuration(ActiveGameplayEffectHandle, StartTime, Duration);
	}

	for (size_t i = 0; i < TimelineGameplayEffectCompsToCreate.Num(); i++)
	{
		if (TimelineGameplayEffectCompsToCreate[i])
		{
			UPKM_TimelineGameplayEffectComp* TimelineGameplayEffectCompToCreated = Cast<UPKM_TimelineGameplayEffectComp>(GetOwner()->AddComponentByClass(TimelineGameplayEffectCompsToCreate[i], false, FTransform(), false));
			if (TimelineGameplayEffectCompToCreated)
			{
				TimelineGameplayEffectCompToCreated->Init(this);
				TimelineGameplayEffectCompsCreated.Add(TimelineGameplayEffectCompToCreated);
			}
		}
	}

	BeginPlayAfterInit();
}

void UPKM_GameplayEffectActorComponent::PeriodicExecute()
{
	ClearFXComponentPeriodic();

	if (ParticleSettingsPeriodic.ParticlesTemplate)
	{
		FXComponentPeriodic = UFXBlueprintFunctionLibrary::CreatePKMFXComponent(GetOwner(), TEXT("PeriodicFX"), ParticleSettingsPeriodic);
	}
}

void UPKM_GameplayEffectActorComponent::ClearFXComponentPeriodic()
{
	if (FXComponentPeriodic)
	{
		UFXBlueprintFunctionLibrary::AskDestroyAfterDeactivateDelayed(FXComponentPeriodic, DeactiveParticleDelayPeriodicOnEnd, DestroyParticleDelayPeriodicOnEnd);
		FXComponentPeriodic = nullptr;
	}
}

void UPKM_GameplayEffectActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FXComponentStart)
	{
		UFXBlueprintFunctionLibrary::AskDestroyAfterDeactivateDelayed(FXComponentStart, DeactiveParticleDelayStartOnEnd, DestroyParticleDelayStartOnEnd);
	}

	ClearFXComponentPeriodic();

	if (ParticleSettingsEnd.ParticlesTemplate)
	{
		UFXSystemComponent* FXComponentEnd = UFXBlueprintFunctionLibrary::CreatePKMFXComponent(GetOwner(), TEXT("EndFX"), ParticleSettingsEnd);
		UFXBlueprintFunctionLibrary::AskDestroyAfterDeactivateDelayed(FXComponentEnd, DeactiveParticleDelayEndOnEnd, DestroyParticleDelayEndOnEnd);
	}

	for (UPKM_TimelineGameplayEffectComp* TimelineGameplayEffectCompCreated : TimelineGameplayEffectCompsCreated)
	{
		if (TimelineGameplayEffectCompCreated)
		{
			TimelineGameplayEffectCompCreated->DestroyComponent();
		}
	}
	TimelineGameplayEffectCompsCreated.Empty();

	Super::EndPlay(EndPlayReason);
}

void UPKM_GameplayEffectActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (UWorld* World = GetWorld())
	{
		//0.0f can be triggered multiple times
		RemainingTime = FMath::Max(0, Duration - (World->GetTimeSeconds() - StartTime));
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UPKM_GameplayEffectActorComponent::GetRemaininTimePercent() const
{
	if (Duration == -1) //infinite
	{
		return 0.0f;
	}
	else
	{
		return RemainingTime / Duration;
	}
}