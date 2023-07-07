// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EffectDurationWidget.h"
#include "GameplayAbilitieTest/Character/Stats/PKM_AbilitySystemComponent.h"
#include "EffectDurationManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYABILITIETEST_API UEffectDurationManagerWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeDestruct();
	
public : 

	TMap<int32, UEffectDurationWidget*> EffectDurationWidgets;
	UPKM_AbilitySystemComponent* AbilitySystemComponent;
	FGameplayEffectAddedDelegate DelegateEffectAdded;

	UFUNCTION(BlueprintCallable)
		void Init(UPKM_AbilitySystemComponent* AbilitySystemComp);

	UFUNCTION()
		void OnEffectAdded(TSubclassOf<UGameplayEffect> EffectClassFilter, FActiveGameplayEffectHandle handle, bool added);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void AddEffectDurationWidgetToParentWidget(UEffectDurationWidget* EffectDurationWidget);
	virtual void AddEffectDurationWidgetToParentWidget_Implementation(UEffectDurationWidget* EffectDurationWidget) {};

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UEffectDurationWidget> EffectDurationWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		float UpdateFrequency = 0.1f;

	FTimerHandle TimerHandleUpdate;
	FTimerDelegate TimerDelegateUpdate;

	UFUNCTION()
		void UpdateWidgets();
};
