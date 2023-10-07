// Fill out your copyright notice in the Description page of Project Settings.

#include "PKM_GameplayEffectActorComponent.h"
#include "GameplayAbilitieTest/Character/Stats/PKM_AbilitySystemComponent.h"
#include "GameplayAbilitieTest/FX/FXBlueprintFunctionLibrary.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_TimelineGameplayEffectComp.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayAbilitieTest/Tools/Interfaces/PKM_GetComponentInterface.h"


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
		FString NameFX = "StartFX" + GetName();
		FXComponentStart = UFXBlueprintFunctionLibrary::CreatePKMFXComponent(GetOwner(), FName(*NameFX), ParticleSettingsStart);
	}

	ProcessCameraShake(ShakeClassStart, ShakeSocketStart, ShakeRadiusStart);
	PlaySound(SoundStart);

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
		FString NameFX = "PeriodicFX" + GetName();
		FXComponentPeriodic = UFXBlueprintFunctionLibrary::CreatePKMFXComponent(GetOwner(), FName(*NameFX), ParticleSettingsPeriodic);
	}

	ProcessCameraShake(ShakeClassPeriodic, ShakeSocketPeriodic, ShakeRadiusPeriodic);
	PlaySound(SoundPeriodic);
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
		FString NameFX = "EndFX" + GetName();
		UFXSystemComponent* FXComponentEnd = UFXBlueprintFunctionLibrary::CreatePKMFXComponent(GetOwner(), FName(*NameFX), ParticleSettingsEnd);
		UFXBlueprintFunctionLibrary::AskDestroyAfterDeactivateDelayed(FXComponentEnd, DeactiveParticleDelayEndOnEnd, DestroyParticleDelayEndOnEnd);
	}

	ProcessCameraShake(ShakeClassEnd, ShakeSocketEnd, ShakeRadiusEnd);
	PlaySound(SoundEnd);

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

void UPKM_GameplayEffectActorComponent::ProcessCameraShake(TSubclassOf<class UCameraShakeBase> ShakeClass, EAttachmentComponentSocket ShakeSocket, float ShakeRadius)
{
	if (ShakeClass)
	{
		AActor* ownerRef = GetOwner();
		if (ownerRef)
		{
			FVector locationShake = FVector::ZeroVector;

			if (ownerRef->Implements<UPKM_GetComponentInterface>())
			{
				UMeshComponent* MeshComponent = IPKM_GetComponentInterface::Execute_GetPrincipalMesh(ownerRef);
				if (MeshComponent)
				{
					locationShake = MeshComponent->GetSocketLocation(IPKM_GetComponentInterface::Execute_GetBoneNameFromAttachmentComponentSocket(ownerRef, ShakeSocket));
				}
				else
				{
					locationShake = ownerRef->GetRootComponent()->GetSocketLocation(IPKM_GetComponentInterface::Execute_GetBoneNameFromAttachmentComponentSocket(ownerRef, ShakeSocket));
				}
			}
			else
			{
				locationShake = ownerRef->GetRootComponent()->GetComponentLocation();
			}

			UGameplayStatics::PlayWorldCameraShake(this, ShakeClass, locationShake, 0.0f, ShakeRadius, 2.0f, false);
		}
	}
}

void UPKM_GameplayEffectActorComponent::PlaySound(TSoftObjectPtr<USoundBase> SoundToPlay)
{
	USoundBase* SoundToPlayLoaded = SoundToPlay.LoadSynchronous();
	if (SoundToPlayLoaded)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlayLoaded, GetOwner()->GetActorLocation(), FRotator(), FMath::RandRange(0.8f, 1.0f), FMath::RandRange(0.7f, 1.3f));
	}
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