// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectDurationWidget.h"
#include "Kismet/KismetMaterialLibrary.h"

void UEffectDurationWidget::Init(TSoftObjectPtr<UTexture> Icon, float _Duration, float _StartTime)
{
	Duration = _Duration;
	StartTime = _StartTime;

	HadToUpdate = (Duration > 0);
	UpdateVisiblityUpdateWidgets();

	UTexture* IconLoaded = Icon.LoadSynchronous();
	if (IconLoaded)
	{
		UpdateIcon(IconLoaded);
	}

	UWorld* worldRef = GetWorld();
	if (worldRef)
	{
		Update(worldRef->GetTimeSeconds());
	}
}

void UEffectDurationWidget::UpdateVisiblityUpdateWidgets_Implementation()
{
	if (HadToUpdate)
	{
		MaterialCooldownInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, MaterialCooldown);
		if (MaterialCooldownInstance)
		{
			MaterialCooldownInstance->SetScalarParameterValue("startTime", StartTime);
			MaterialCooldownInstance->SetScalarParameterValue("duration", Duration);
		}
	}
}

void UEffectDurationWidget::Update(float GameTime)
{
	remainingTime = FMath::Max(0.0f, Duration - (GameTime - StartTime));

	if (MaterialCooldownInstance)
	{
		MaterialCooldownInstance->SetScalarParameterValue("percent", remainingTime / Duration);
	}

	UpdateBP();
}