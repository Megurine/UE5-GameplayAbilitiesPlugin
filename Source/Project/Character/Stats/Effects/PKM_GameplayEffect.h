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

	virtual void EventAtStart(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const override;
	virtual void EventAtPeriod(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const override;
	virtual void EventAtEnd(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventAtStartBP(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const;
	virtual void EventAtStartBP_Implementation(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventAtPeriodBP(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const;
	virtual void EventAtPeriodBP_Implementation(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventAtEndBP(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const;
	virtual void EventAtEndBP_Implementation(UAbilitySystemComponent* AbilitySystemComponentTarget, AActor* Instigator) const {};

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static TSoftObjectPtr<UTexture> GetClassVariableIcon(TSubclassOf<UPKM_GameplayEffect> _effectClass);
};
