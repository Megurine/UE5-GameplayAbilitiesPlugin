// Fill out your copyright notice in the Description page of Project Settings.


#include "PKM_GameplayTagAssetInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitieTest/Character/Stats/PKM_AbilitySystemComponent.h"

bool UGameplayTagFunctionLibrary::GetPKMTagContainer(UObject* object, FGameplayTagContainer& _TagContainer, UPKM_AbilitySystemComponent*& _AbilitySystemComp)
{
	_AbilitySystemComp = nullptr;
	if (object)
	{
		if (UPKM_AbilitySystemComponent* AbilitySystemComp = Cast<UPKM_AbilitySystemComponent>(object))
		{
			AbilitySystemComp->GetOwnedGameplayTags(_TagContainer);
			_AbilitySystemComp = AbilitySystemComp;
			return true;
		}
		else if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(object))
		{
			UPKM_AbilitySystemComponent* AbilitySystemComp2 = Cast<UPKM_AbilitySystemComponent>(ASI->GetAbilitySystemComponent());
			if (AbilitySystemComp2)
			{
				AbilitySystemComp2->GetOwnedGameplayTags(_TagContainer);
				_AbilitySystemComp = AbilitySystemComp2;
				return true;
			}
		}
		else if (object->Implements<UPKM_ComponentGameplayTagAssetInterface>())
		{
			IPKM_ComponentGameplayTagAssetInterface::Execute_GetTagContainer(object, _TagContainer);
			return true;
		}
		else if (object->Implements<UPKM_GameplayTagAssetInterface>())
		{
			IPKM_GameplayTagAssetInterface::Execute_GetTagContainer(object, _TagContainer); //GetTagContainer
			return true;
		}
		else if (AActor* actor = Cast<AActor>(object))
		{
			UPKM_AbilitySystemComponent* AbilitySystemComp2 = Cast<UPKM_AbilitySystemComponent>(actor->GetComponentByClass(UPKM_AbilitySystemComponent::StaticClass()));
			if (AbilitySystemComp2)
			{
				AbilitySystemComp2->GetOwnedGameplayTags(_TagContainer);
				_AbilitySystemComp = AbilitySystemComp2;
				return true;
			}
		}
	}
	return false;
}

void UGameplayTagFunctionLibrary::AddPKMGameplayTag(UObject* object, FGameplayTag TagToAdd)
{
	if (object)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp = nullptr;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				AbilitySystemComp->AddLooseGameplayTag(TagToAdd);
			}
			else
			{
				TagContainer.AddTag(TagToAdd);
			}
		}
	}
}

void UGameplayTagFunctionLibrary::AddPKMGameplayTags(UObject* object, FGameplayTagContainer TagsToAdd)
{
	if (object && TagsToAdd.Num() > 0)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp = nullptr;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				AbilitySystemComp->AddLooseGameplayTags(TagsToAdd);
			}
			else
			{
				for (FGameplayTag TagToAdd : TagsToAdd)
				{
					TagContainer.AddTag(TagToAdd);
				}
			}
		}
	}
}

bool UGameplayTagFunctionLibrary::HasMatchingPKMGameplayTag(UObject* object, FGameplayTag TagToCheck)
{
	bool bResult = false;
	if (object)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp = nullptr;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				bResult = AbilitySystemComp->HasMatchingGameplayTag(TagToCheck);
			}
			else
			{
				bResult = TagContainer.HasTag(TagToCheck);
			}
		}
	}
	return bResult;
}

bool UGameplayTagFunctionLibrary::HasAllMatchingPKMGameplayTags(UObject* object, const FGameplayTagContainer& _TagContainer)
{
	bool bResult = false;
	if (object)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				bResult = AbilitySystemComp->HasAllMatchingGameplayTags(_TagContainer);
			}
			else
			{
				bResult = TagContainer.HasAll(_TagContainer);
			}
		}
	}
	return bResult;
}

bool UGameplayTagFunctionLibrary::HasAnyMatchingPKMGameplayTags(UObject* object, const FGameplayTagContainer& _TagContainer)
{
	bool bResult = false;
	if (object)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				bResult = AbilitySystemComp->HasAnyMatchingGameplayTags(_TagContainer);
			}
			else
			{
				bResult = TagContainer.HasAny(_TagContainer);
			}
		}
	}
	return bResult;
}

void UGameplayTagFunctionLibrary::RemovePKMTag(UObject* object, FGameplayTag TagToRemove)
{
	if (object)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				AbilitySystemComp->RemoveLooseGameplayTag(TagToRemove);
			}
			else
			{
				TagContainer.RemoveTag(TagToRemove);
			}
		}
	}
}

void UGameplayTagFunctionLibrary::RemovePKMTags(UObject* object, FGameplayTagContainer TagsToRemove)
{
	if (object && TagsToRemove.Num() > 0)
	{
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp = nullptr;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			if (AbilitySystemComp)
			{
				AbilitySystemComp->RemoveLooseGameplayTags(TagsToRemove);
			}
			else
			{
				for (FGameplayTag TagToRemove : TagsToRemove)
				{
					TagContainer.RemoveTag(TagToRemove);
				}
			}
			
		}
	}
}

TArray<FGameplayTag> UGameplayTagFunctionLibrary::FindPKMGameplayTagsMatching(UObject* object, FGameplayTag _TagToCheck)
{
	TArray<FGameplayTag> ret;
	if (object)
	{
		TArray<FGameplayTag> arrayTag;
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			TagContainer.GetGameplayTagArray(arrayTag);

			for (auto tag : arrayTag)
			{
				if (tag.MatchesTag(_TagToCheck))
				{
					ret.Add(tag);
				}
			}
		}
	}
	return ret;
}

FGameplayTag UGameplayTagFunctionLibrary::FindPKMGameplayTagMatching(UObject* object, FGameplayTag _TagToCheck)
{
	if (object)
	{
		TArray<FGameplayTag> arrayTag;
		FGameplayTagContainer TagContainer;
		UPKM_AbilitySystemComponent* AbilitySystemComp;
		if (GetPKMTagContainer(object, TagContainer, AbilitySystemComp))
		{
			TagContainer.GetGameplayTagArray(arrayTag);

			for (auto tag : arrayTag)
			{
				if (tag.MatchesTag(_TagToCheck))
				{
					return tag;
				}
			}
		}
	}
	return FGameplayTag::EmptyTag;
}