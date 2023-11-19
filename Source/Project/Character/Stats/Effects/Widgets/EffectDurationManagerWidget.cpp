// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectDurationManagerWidget.h"
#include "GameplayAbilitieTest/Character/Stats/Effects/PKM_GameplayEffect.h"

void UEffectDurationManagerWidget::Init(UPKM_AbilitySystemComponent* AbilitySystemComp)
{
	TimerDelegateUpdate = FTimerDelegate::CreateUObject(this, &UEffectDurationManagerWidget::UpdateWidgets);

	AbilitySystemComponent = AbilitySystemComp;
	if (AbilitySystemComponent)
	{
		DelegateEffectAdded.BindDynamic(this, &UEffectDurationManagerWidget::OnEffectAdded);
		AbilitySystemComponent->BindFunctionToOnActiveGameplayEffectAdded(DelegateEffectAdded, UGameplayEffect::StaticClass(), true);
	}
}

void UEffectDurationManagerWidget::NativeDestruct()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->UnBindFunctionToOnActiveGameplayEffectAdded(DelegateEffectAdded);
	}
	UWorld* worldRef = GetWorld();
	if (worldRef)
	{
		worldRef->GetTimerManager().ClearTimer(TimerHandleUpdate);
	}
	Super::NativeDestruct();
	
}

void UEffectDurationManagerWidget::OnEffectAdded(TSubclassOf<UGameplayEffect> EffectClassFilter, FActiveGameplayEffectHandle handle, bool added)
{
	if (added)
	{
		if (EffectClassFilter->IsChildOf(UPKM_GameplayEffect::StaticClass()))
		{
			TSubclassOf<UPKM_GameplayEffect> PKMEffectClassFilter = { (TSubclassOf<UPKM_GameplayEffect>)EffectClassFilter };
			if (PKMEffectClassFilter)
			{
				//check instant
				EGameplayEffectDurationType DurationPolicy = UPKM_GameplayEffect::GetClassVariableDurationPolicy(PKMEffectClassFilter);
				if (DurationPolicy != EGameplayEffectDurationType::Instant)
				{
					EEffectWidgetVisibilityType WidgetVisibilityType = UPKM_GameplayEffect::GetClassVariableWidgetVisibilityType(PKMEffectClassFilter);
					if (WidgetVisibilityType != EEffectWidgetVisibilityType::INVISIBLE)
					{
						TSoftObjectPtr<UTexture> Icon = UPKM_GameplayEffect::GetClassVariableIcon(PKMEffectClassFilter);

						float StartEffectTime;
						float Duration;
						AbilitySystemComponent->GetGameplayEffectStartTimeAndDuration(handle, StartEffectTime, Duration);

						if (EffectDurationWidgetClass)
						{
							//this->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("MyButtonName"));
							UEffectDurationWidget* widgetCreated = CreateWidget<UEffectDurationWidget>(GetWorld(), EffectDurationWidgetClass);

							EffectDurationWidgets.Add(handle.Handle, widgetCreated);

							AddEffectDurationWidgetToParentWidget(widgetCreated, WidgetVisibilityType);

							widgetCreated->Init(Icon, Duration, StartEffectTime);

							if (EffectDurationWidgets.Num() == 1)
							{
								UWorld* worldRef = GetWorld();
								if (worldRef)
								{
									worldRef->GetTimerManager().SetTimer(TimerHandleUpdate, TimerDelegateUpdate, UpdateFrequency, true);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		UEffectDurationWidget* widgetRemoved;
		if (EffectDurationWidgets.RemoveAndCopyValue(handle.Handle, widgetRemoved))
		{
			if (widgetRemoved)
			{
				widgetRemoved->RemoveFromParent();

				if (EffectDurationWidgets.Num() == 0)
				{
					UWorld* worldRef = GetWorld();
					if (worldRef)
					{
						worldRef->GetTimerManager().ClearTimer(TimerHandleUpdate);
					}
				}
			}
		}
	}
}

void UEffectDurationManagerWidget::UpdateWidgets()
{
	UWorld* worldRef = GetWorld();
	if (worldRef)
	{
		float gameTime = worldRef->GetTimeSeconds();
		for (const TPair<int32, UEffectDurationWidget*>& widget : EffectDurationWidgets)
		{
			if (widget.Value->HadToUpdate)
			{
				widget.Value->Update(gameTime);
			}
		}
	}
}