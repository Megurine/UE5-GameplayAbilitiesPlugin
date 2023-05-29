// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "PKM_TimelineGameplayEffectComp.generated.h"

class UPKM_GameplayEffectActorComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEPLAYABILITIETEST_API UPKM_TimelineGameplayEffectComp : public UActorComponent
{
	GENERATED_BODY()

public:

	UPKM_TimelineGameplayEffectComp();

	void Init(UPKM_GameplayEffectActorComponent* _GameplayEffectActorComponent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UPKM_GameplayEffectActorComponent* GameplayEffectActorComponent = nullptr;

protected:
	// Called when the game starts
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool NeedToAddComponentToAbilitySystem();
	virtual bool NeedToAddComponentToAbilitySystem_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void BeginPlayAfterInit();
	virtual void BeginPlayAfterInit_Implementation() {};

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Effect(float value);
	virtual void Effect_Implementation(float value) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
		UCurveFloat* interpolationCurve = nullptr;

	UPROPERTY(BlueprintReadOnly)
		float cachedValue = 0.0f;

#pragma region Material

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle))
		bool SetColorOnOwner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SetColorOnOwner"))
		FLinearColor ColorOnOwner = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle))
		bool SetPostProcessMaterialOnOwner = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SetPostProcessMaterialOnOwner"))
		UMaterialInterface* PostProcessMaterialOnOwner = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName PostProcessMaterialOnOwnerParameter;

	void UpdatePostProcessMaterial();

	UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "SetPostProcessMaterialOnOwner"))
		UMaterialInstanceDynamic* PostProcessMaterialOnOwnerInstance = nullptr;

private :

	UCameraComponent* OwnerCameraForPostProcessMaterial = nullptr;

#pragma endregion Material
};

