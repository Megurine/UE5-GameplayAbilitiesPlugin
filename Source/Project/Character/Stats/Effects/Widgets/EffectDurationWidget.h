// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "Materials/MaterialInterface.h"
//#include "Materials/MaterialInstanceDynamic.h"
#include "EffectDurationWidget.generated.h"

/**
 * 
 */
UCLASS(meta = (DisableNativeTick))
class GAMEPLAYABILITIETEST_API UEffectDurationWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		void Init(TSoftObjectPtr<UTexture> Icon, float _Duration, float _StartTime);

	UPROPERTY(BlueprintReadOnly)
		float Duration;

	UPROPERTY(BlueprintReadOnly)
		float StartTime;

	UPROPERTY(BlueprintReadOnly)
		float remainingTime;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* MaterialCooldown = nullptr;

	UPROPERTY(BlueprintReadOnly)
		UMaterialInstanceDynamic* MaterialCooldownInstance = nullptr;

	UPROPERTY(BlueprintReadOnly)
		bool HadToUpdate;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateVisiblityUpdateWidgets();
	virtual void UpdateVisiblityUpdateWidgets_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateIcon(UTexture* _icon);
	virtual void UpdateIcon_Implementation(UTexture* _icon) {};

	void Update(float GameTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void UpdateBP();
	virtual void UpdateBP_Implementation() {};

};
