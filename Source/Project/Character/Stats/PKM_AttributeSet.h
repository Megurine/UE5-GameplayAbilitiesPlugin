// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayAbilitieTest/PKM_Types.h"
#include "PKM_AttributeSet.generated.h"

UENUM(BlueprintType)
enum class EPKM_Attributes : uint8
{
	HP = 0 UMETA(DisplayName = "Hp"),
	ENERGY = 1 UMETA(DisplayName = "Energy"),
	SPEED = 2 UMETA(DisplayName = "Speed"),
};

UENUM(BlueprintType)
enum class EPKM_AddToAttributeType : uint8
{
	TOCURRENTVALUE = 0 UMETA(DisplayName = "To Current value"),
	TOBASEVALUE = 1 UMETA(DisplayName = "To Base value"),
};

USTRUCT(BlueprintType)
struct FGameplayAttributeCouple
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPKM_Attributes attribute = EPKM_Attributes::HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPKM_AttributesType type = EPKM_AttributesType::VALUE;

	FGameplayAttributeCouple() {}

	FGameplayAttributeCouple(EPKM_Attributes _attribute, EPKM_AttributesType _type)
	{
		attribute = _attribute;
		type = _type;
	}
};

USTRUCT(BlueprintType)
struct FPKMAttributeCachedValues
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HealthValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HealthMax = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EnergyValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EnergyMax = 0.0f;

	FPKMAttributeCachedValues() {}

	FPKMAttributeCachedValues(float _HealthValue, float _HealthMax, float _EnergyValue, float _EnergyMax)
	{
		HealthValue = _HealthValue;
		HealthMax = _HealthMax;
		EnergyValue = _EnergyValue;
		EnergyMax = _EnergyMax;
	}
};

/*#define GAMEPLAYATTRIBUTE_PROPERTY_BIND_VALUECHANGED(ClassName, PropertyName) \
	static FGameplayAttribute Get##PropertyName##Attribute() \
	{ \
		static FProperty* Prop = FindFieldChecked<FProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName)); \
		return Prop; \
	}*/

UCLASS()
class GAMEPLAYABILITIETEST_API UPKM_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	const FGameplayAttribute GetAttributeByEnum(EPKM_Attributes attribute, EPKM_AttributesType type = EPKM_AttributesType::VALUE) const;

	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;

	TMap<FString, FGameplayAttributeCouple> AttributeCouple;

	FPKMAttributeCachedValues ComputeAttributeCachedValues() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData Hp;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, Hp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData HpMin;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, HpMin);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData HpMax;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, HpMax);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData Energy;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, Energy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData EnergyMin;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, EnergyMin);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData EnergyMax;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, EnergyMax);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData Speed;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, Speed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData SpeedMin;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, SpeedMin);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameUnitAttributes)
		FGameplayAttributeData SpeedMax;
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UPKM_AttributeSet, SpeedMax);
};
