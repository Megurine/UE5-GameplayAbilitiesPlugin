// Fill out your copyright notice in the Description page of Project Settings.


#include "PKM_TimelineGameplayEffectComp.h"
#include "GameplayAbilitieTest/Character/Stats/PKM_AbilitySystemComponent.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_GameplayEffectActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMaterialLibrary.h"

UPKM_TimelineGameplayEffectComp::UPKM_TimelineGameplayEffectComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPKM_TimelineGameplayEffectComp::Init(UPKM_GameplayEffectActorComponent* _GameplayEffectActorComponent)
{
	GameplayEffectActorComponent = _GameplayEffectActorComponent;

	if (NeedToAddComponentToAbilitySystem())
	{
		if (GameplayEffectActorComponent)
		{
			if (GameplayEffectActorComponent->PKMAbilitySystemComponent)
			{
				GameplayEffectActorComponent->PKMAbilitySystemComponent->AddTimelineGameplayEffectCompColors(this);
			}
		}
	}
	if (SetPostProcessMaterialOnOwner && PostProcessMaterialOnOwner)
	{
		AActor* owner = GetOwner();
		if (owner)
		{
			UCameraComponent* CameraComponent = Cast<UCameraComponent>(owner->GetComponentByClass(UCameraComponent::StaticClass()));
			if (CameraComponent)
			{
				OwnerCameraForPostProcessMaterial = CameraComponent;
				PostProcessMaterialOnOwnerInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(OwnerCameraForPostProcessMaterial, PostProcessMaterialOnOwner);
				if (PostProcessMaterialOnOwnerInstance)
				{
					int32 blendValue = 0.0;
					if (!PostProcessMaterialOnOwnerParameter.IsNone())
					{
						blendValue = 1.0f;
						UpdatePostProcessMaterial();
					}

					OwnerCameraForPostProcessMaterial->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(blendValue, PostProcessMaterialOnOwnerInstance));
				}
			}
		}
	}

	BeginPlayAfterInit();
}

bool UPKM_TimelineGameplayEffectComp::NeedToAddComponentToAbilitySystem_Implementation()
{
	return SetColorOnOwner;
}

void UPKM_TimelineGameplayEffectComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GameplayEffectActorComponent)
	{
		float value = 1.0f - GameplayEffectActorComponent->GetRemaininTimePercent();

		if (interpolationCurve)
		{
			value = interpolationCurve->GetFloatValue(value);
		}

		cachedValue = value;

		UpdatePostProcessMaterial();

		Effect(value);
	}
	else
	{
		DestroyComponent();
	}
}

void UPKM_TimelineGameplayEffectComp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NeedToAddComponentToAbilitySystem())
	{
		if (GameplayEffectActorComponent)
		{
			if (GameplayEffectActorComponent->PKMAbilitySystemComponent)
			{
				GameplayEffectActorComponent->PKMAbilitySystemComponent->RemoveTimelineGameplayEffectCompColors(this);
			}
		}
	}

	if (OwnerCameraForPostProcessMaterial)
	{
		if (PostProcessMaterialOnOwnerInstance)
		{
			TArray<FWeightedBlendable> blendables = OwnerCameraForPostProcessMaterial->PostProcessSettings.WeightedBlendables.Array;
			for (size_t i = 0; i < blendables.Num(); i++)
			{
				if (UMaterialInstanceDynamic* ObjetcMat = Cast<UMaterialInstanceDynamic>(blendables[i].Object))
				{
					if (ObjetcMat == PostProcessMaterialOnOwnerInstance)
					{
						OwnerCameraForPostProcessMaterial->PostProcessSettings.WeightedBlendables.Array.RemoveAt(i);
						break;
					}
				}
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UPKM_TimelineGameplayEffectComp::UpdatePostProcessMaterial()
{
	if (PostProcessMaterialOnOwnerInstance)
	{
		if (PostProcessMaterialOnOwnerParameter.IsNone())
		{
			if (OwnerCameraForPostProcessMaterial)
			{
				for (size_t i = 0; i < OwnerCameraForPostProcessMaterial->PostProcessSettings.WeightedBlendables.Array.Num(); i++)
				{
					if (UMaterialInstanceDynamic* ObjetcMat = Cast<UMaterialInstanceDynamic>(OwnerCameraForPostProcessMaterial->PostProcessSettings.WeightedBlendables.Array[i].Object))
					{
						if (ObjetcMat == PostProcessMaterialOnOwnerInstance)
						{
							OwnerCameraForPostProcessMaterial->PostProcessSettings.WeightedBlendables.Array[i].Weight = cachedValue;
							break;
						}
					}
				}
			}
		}
		else
		{
			PostProcessMaterialOnOwnerInstance->SetScalarParameterValue(PostProcessMaterialOnOwnerParameter, cachedValue);
		}
	}
}