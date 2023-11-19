// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Particles/ParticleSystem.h"
#include "AbilitySystemComponent.h"
#include "PKM_Types.generated.h"

UENUM(BlueprintType)
enum class EPKM_AttributesType : uint8
{
	VALUE = 0 UMETA(DisplayName = "Value"),
	MINIMUM = 1 UMETA(DisplayName = "Min"),
	MAXIMUM = 2 UMETA(DisplayName = "Max"),
};

DECLARE_DYNAMIC_DELEGATE_FourParams(FAttributeValueChangeDelegate, UObject*, ObjectBound, float, newValue, float, oldValue, UAttributeChangeDatasObject*, changeDatas);
DECLARE_DYNAMIC_DELEGATE_FourParams(FGameplayEffectAddedDelegate, UObject*, ObjectBound, TSubclassOf<UGameplayEffect>, EffectClassFilter, FActiveGameplayEffectHandle, handle, bool, added);
DECLARE_DYNAMIC_DELEGATE_FiveParams(FOnGameplayTagChanged, UObject*, ObjectBound, const FGameplayTag, Tag, bool, TagAdded, int32, Count, const FGameplayTag, TriggerTag);

UENUM(BlueprintType)
enum class EAttachmentComponentSocket : uint8
{
	MOUTH = 0 UMETA(DisplayName = "Mouth"),
	BODY = 1 UMETA(DisplayName = "Body"),
	HAND = 2 UMETA(DisplayName = "Hand"),
	ORIGIN = 3 UMETA(DisplayName = "Origin"),
	FXTOPHEAD = 4 UMETA(DisplayName = "FXTopHead"),
	FOREHEAD = 5 UMETA(DisplayName = "Forehead"),
};

UENUM(BlueprintType)
enum class EAttachmentComponentType : uint8
{
	MESHCOMPONENTBYINTERFACE = 0 UMETA(DisplayName = "MeshComponentByInterface"),
	ROOTCOMPONENT = 1 UMETA(DisplayName = "RootComponent"),
};

USTRUCT(BlueprintType)
struct FFXAttachment
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UFXSystemAsset* ParticlesTemplate = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ScaleParticle = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAttachmentComponentSocket SocketParticle = EAttachmentComponentSocket::BODY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAttachmentRule LocationRuleParticle = EAttachmentRule::KeepRelative;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAttachmentRule RotationRuleParticle = EAttachmentRule::KeepRelative;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAttachmentRule ScaleRuleParticle = EAttachmentRule::KeepRelative;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAttachmentComponentType AttachmentComponentTypeParticle = EAttachmentComponentType::MESHCOMPONENTBYINTERFACE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeOffsetParticle = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ComponentOverrideWorldRotationOnTickBool = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ComponentOverrideWorldRotationOnTickBool", EditConditionHides))
		FRotator ComponentOverrideWorldRotationOnTick = FRotator::ZeroRotator;

	FFXAttachment() {}

	FFXAttachment(UFXSystemAsset* _ParticlesTemplate, FVector _ScaleParticle, EAttachmentComponentSocket _SocketParticle, EAttachmentRule _LocationRuleParticle, EAttachmentRule _RotationRuleParticle, EAttachmentRule _ScaleRuleParticle, EAttachmentComponentType _AttachmentComponentTypeParticle, FVector _RelativeOffsetParticle, bool _ComponentOverrideWorldRotationOnTickBool, FRotator _ComponentOverrideWorldRotationOnTick)
	{
		ParticlesTemplate = _ParticlesTemplate;
		ScaleParticle = _ScaleParticle;
		SocketParticle = _SocketParticle;
		LocationRuleParticle = _LocationRuleParticle;
		RotationRuleParticle = _RotationRuleParticle;
		ScaleRuleParticle = _ScaleRuleParticle;
		AttachmentComponentTypeParticle = _AttachmentComponentTypeParticle;
		RelativeOffsetParticle = _RelativeOffsetParticle;
		ComponentOverrideWorldRotationOnTickBool = _ComponentOverrideWorldRotationOnTickBool;
		ComponentOverrideWorldRotationOnTick = _ComponentOverrideWorldRotationOnTick;
	}
};