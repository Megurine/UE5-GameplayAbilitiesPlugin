// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttributeChangeDatasObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYABILITIES_API UAttributeChangeDatasObject : public UObject
{
	GENERATED_BODY()
	
public:

	UAttributeChangeDatasObject();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Datas")
		AActor* Instigator = nullptr;
};
