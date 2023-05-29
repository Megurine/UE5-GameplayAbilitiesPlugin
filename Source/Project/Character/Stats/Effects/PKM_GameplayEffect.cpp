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