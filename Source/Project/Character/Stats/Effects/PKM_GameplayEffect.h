// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayAbilitieTest/Character/Stats/PKM_AbilitySystemComponent.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_GameplayEffectActorComponent.h"
#include "PKM_GameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYABILITIETEST_API UPKM_GameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public :

	void Init(UPKM_AbilitySystemComponent* AbilitySystemComp, FActiveGameplayEffectHandle handle) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSoftClassPtr<UPKM_GameplayEffectActorComponent> GameplayEffectComponentClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
		TSoftObjectPtr<UTexture> Icon = nullptr;

	virtual void EventAtStart(UAbilitySystemComponent* AbilitySystemComponent) const override;
	virtual void EventAtPeriod(UAbilitySystemComponent* AbilitySystemComponent) const override;
	virtual void EventAtEnd(UAbilitySystemComponent* AbilitySystemComponent) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventAtStartBP(UAbilitySystemComponent* AbilitySystemComponent) const;
	virtual void EventAtStartBP_Implementation(UAbilitySystemComponent* AbilitySystemComponent) const {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventAtPeriodBP(UAbilitySystemComponent* AbilitySystemComponent) const;
	virtual void EventAtPeriodBP_Implementation(UAbilitySystemComponent* AbilitySystemComponent) const {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventAtEndBP(UAbilitySystemComponent* AbilitySystemComponent) const;
	virtual void EventAtEndBP_Implementation(UAbilitySystemComponent* AbilitySystemComponent) const {};

};
