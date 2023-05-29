// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "PKM_GameplayTagAssetInterface.generated.h"

#define MAKE_TAG_STRING(string) FGameplayTag::RequestGameplayTag(FName(*string))
#define MAKE_TAG(string) FGameplayTag::RequestGameplayTag(FName(string))

class UPKM_AbilitySystemComponent;

//CannotImplementInterfaceInBlueprint remplacé par NotBlueprintable en 5.0 
//https://docs.unrealengine.com/5.0/en-US/unreal-engine-5_0-release-notes/
//Mais cela force à override toutes les fonctions et a les mettres en UFUNCTION()
//A la place j'ai trouvé une interface dans UE4.27 qui avait Blueprintable ET CannotImplementInterfaceInBlueprint :
//UE_5.0\Engine\Source\Runtime\AIModule\Classes\BehaviorTree\BlackboardAssetProvider.h (UBlackboardAssetProvider)
//Elle passe simplement le Blueprintable en BlueprintType.

UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class UPKM_GameplayTagAssetInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEPLAYABILITIETEST_API IPKM_GameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = GameplayTags)
		void GetTagContainer(FGameplayTagContainer& _TagContainer) const;
	virtual void GetTagContainer_Implementation(FGameplayTagContainer& _TagContainer) const { };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = GameplayTags)
		void SetTagContainer(FGameplayTagContainer& _TagContainer);
	virtual void SetTagContainer_Implementation(FGameplayTagContainer& _TagContainer) { };
};

//Not needed, remplacé par IAbilitySystemInterface
/*UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class UPKM_ActorGameplayTagAssetInterface : public UInterface
{
	GENERATED_BODY()
};


class GAMEPLAYABILITIETEST_API IPKM_ActorGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = GameplayTags)
		UTagHandlerComponent* GetTagHandlerComponent() const;
};*/

UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class UPKM_ComponentGameplayTagAssetInterface : public UInterface
{
	GENERATED_BODY()
};


class GAMEPLAYABILITIETEST_API IPKM_ComponentGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = GameplayTags)
		void GetTagContainer(FGameplayTagContainer& _TagContainer);
	virtual void GetTagContainer_Implementation(FGameplayTagContainer& _TagContainer) { };

};


UCLASS()
class GAMEPLAYABILITIETEST_API UGameplayTagFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static bool GetPKMTagContainer(UObject* object, FGameplayTagContainer& _TagContainer, UPKM_AbilitySystemComponent*& _AbilitySystemComp);

	/**
	 * Add the specified tag to the container
	 */
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static void AddPKMGameplayTag(UObject* object, FGameplayTag TagToAdd);

	/**
	 * Add the specified tags to the container
	 */
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static void AddPKMGameplayTags(UObject* object, FGameplayTagContainer TagsToAdd);

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static bool HasMatchingPKMGameplayTag(UObject* object, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static bool HasAllMatchingPKMGameplayTags(UObject* object, const FGameplayTagContainer& _TagContainer);

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static bool HasAnyMatchingPKMGameplayTags(UObject* object, const FGameplayTagContainer& _TagContainer);

	/**
	 * Removes tag in TagToRemove from this container
	 */
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static void RemovePKMTag(UObject* object, FGameplayTag TagToRemove);

	/**
	 * Removes all tags in TagsToRemove from this container
	 */
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static void RemovePKMTags(UObject* object, FGameplayTagContainer TagsToRemove);
		
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static TArray<FGameplayTag> FindPKMGameplayTagsMatching(UObject* object, FGameplayTag _TagToCheck);

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		static FGameplayTag FindPKMGameplayTagMatching(UObject* object, FGameplayTag _TagToCheck);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = GameplayTags)
		static FGameplayTag MakeTagBP(UPARAM(ref) const FString& stringTag) { return MAKE_TAG(stringTag); };
};