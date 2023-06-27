// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitieTest/FX/PKM_ParticleSystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitieTest/PKM_Types.h"
#include "PKM_GameplayEffectActorComponent.generated.h"

class UPKM_AbilitySystemComponent;
class UPKM_TimelineGameplayEffectComp;


UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEPLAYABILITIETEST_API UPKM_GameplayEffectActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPKM_GameplayEffectActorComponent();

protected:
	// Called when the game starts
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly)
		UPKM_AbilitySystemComponent* PKMAbilitySystemComponent = nullptr;

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

	void Init(UPKM_AbilitySystemComponent* _PKMAbilitySystemComponent, FActiveGameplayEffectHandle handle);
	void PeriodicExecute();
	void ClearFXComponentPeriodic();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void BeginPlayAfterInit();
	virtual void BeginPlayAfterInit_Implementation() {};

#pragma region Particles

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Start")
		FFXAttachment ParticleSettingsStart = FFXAttachment();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Start")
		float DeactiveParticleDelayStartOnEnd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Start")
		float DestroyParticleDelayStartOnEnd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UFXSystemComponent* FXComponentStart = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|End")
		FFXAttachment ParticleSettingsEnd = FFXAttachment();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|End")
		float DeactiveParticleDelayEndOnEnd = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|End")
		float DestroyParticleDelayEndOnEnd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Periodic")
		FFXAttachment ParticleSettingsPeriodic = FFXAttachment();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Periodic")
		float DeactiveParticleDelayPeriodicOnEnd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Periodic")
		float DestroyParticleDelayPeriodicOnEnd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UFXSystemComponent* FXComponentPeriodic = nullptr;

#pragma endregion Particles

#pragma region ShakeCam

	void ProcessCameraShake(TSubclassOf<class UCameraShakeBase> ShakeClass, EAttachmentComponentSocket ShakeSocket, float ShakeRadius);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|Start")
		TSubclassOf<class UCameraShakeBase> ShakeClassStart = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|Start")
		EAttachmentComponentSocket ShakeSocketStart = EAttachmentComponentSocket::BODY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|Start")
		float ShakeRadiusStart = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|End")
		TSubclassOf<class UCameraShakeBase> ShakeClassEnd = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|End")
		EAttachmentComponentSocket ShakeSocketEnd = EAttachmentComponentSocket::BODY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|End")
		float ShakeRadiusEnd = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|Periodic")
		TSubclassOf<class UCameraShakeBase> ShakeClassPeriodic = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|Periodic")
		EAttachmentComponentSocket ShakeSocketPeriodic = EAttachmentComponentSocket::BODY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake|Periodic")
		float ShakeRadiusPeriodic = 4000;

#pragma endregion ShakeCam

#pragma region Sound

	void PlaySound(TSoftObjectPtr<USoundBase> SoundToPlay);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Start")
		TSoftObjectPtr<USoundBase> SoundStart = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|End")
		TSoftObjectPtr<USoundBase> SoundEnd = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Periodic")
		TSoftObjectPtr<USoundBase> SoundPeriodic = nullptr;

#pragma endregion Sound

#pragma region Time

	UPROPERTY(BlueprintReadOnly, Category = "Time")
		float Duration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
		float StartTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
		float RemainingTime = 0.0f;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time")
		float GetRemaininTimePercent() const;	

	UPROPERTY(VisibleDefaultsOnly, Category = "Time")
		FString TimelineWarning = "Don't forget to use an EventTick()\nif you want your TimelineGameplayEffectComp\nto to work properly.\n(You can leave it empty)";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		TArray<TSubclassOf<UPKM_TimelineGameplayEffectComp>> TimelineGameplayEffectCompsToCreate;

private:

	TArray<UPKM_TimelineGameplayEffectComp*> TimelineGameplayEffectCompsCreated;

#pragma endregion Particles
};
