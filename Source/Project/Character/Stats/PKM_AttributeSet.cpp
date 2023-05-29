// Fill out your copyright notice in the Description page of Project Settings.


#include "PKM_AttributeSet.h"
#include "AbilitySystemComponent.h"


void UPKM_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{

}

const FGameplayAttribute UPKM_AttributeSet::GetAttributeByEnum(EPKM_Attributes attribute, EPKM_AttributesType type) const
{
	switch (attribute)
	{
	case EPKM_Attributes::HP:
		switch (type)
		{
		case EPKM_AttributesType::VALUE:
			return GetHpAttribute();
			break;
		case EPKM_AttributesType::MINIMUM:
			return GetHpMinAttribute();
			break;
		case EPKM_AttributesType::MAXIMUM:
			return GetHpMaxAttribute();
			break;
		}
		break;
	case EPKM_Attributes::ENERGY:
		switch (type)
		{
		case EPKM_AttributesType::VALUE:
			return GetEnergyAttribute();
			break;
		case EPKM_AttributesType::MINIMUM:
			return GetEnergyMinAttribute();
			break;
		case EPKM_AttributesType::MAXIMUM:
			return GetEnergyMaxAttribute();
			break;
		}
		break;
	case EPKM_Attributes::SPEED:
		switch (type)
		{
		case EPKM_AttributesType::VALUE:
			return GetSpeedAttribute();
			break;
		case EPKM_AttributesType::MINIMUM:
			return GetSpeedMinAttribute();
			break;
		case EPKM_AttributesType::MAXIMUM:
			return GetSpeedMaxAttribute();
			break;
		}
		break;
	}
	return NULL;
}

void UPKM_AttributeSet::InitFromMetaDataTable(const UDataTable* DataTable)
{
	Super::InitFromMetaDataTable(DataTable);

	AttributeCouple.Add("Hp", FGameplayAttributeCouple(EPKM_Attributes::HP, EPKM_AttributesType::VALUE));
	AttributeCouple.Add("HpMin", FGameplayAttributeCouple(EPKM_Attributes::HP, EPKM_AttributesType::MINIMUM));
	AttributeCouple.Add("HpMax", FGameplayAttributeCouple(EPKM_Attributes::HP, EPKM_AttributesType::MAXIMUM));

	AttributeCouple.Add("Energy", FGameplayAttributeCouple(EPKM_Attributes::ENERGY, EPKM_AttributesType::VALUE));
	AttributeCouple.Add("EnergyMin", FGameplayAttributeCouple(EPKM_Attributes::ENERGY, EPKM_AttributesType::MINIMUM));
	AttributeCouple.Add("EnergyMax", FGameplayAttributeCouple(EPKM_Attributes::ENERGY, EPKM_AttributesType::MAXIMUM));

	AttributeCouple.Add("Speed", FGameplayAttributeCouple(EPKM_Attributes::SPEED, EPKM_AttributesType::VALUE));
	AttributeCouple.Add("SpeedMin", FGameplayAttributeCouple(EPKM_Attributes::SPEED, EPKM_AttributesType::MINIMUM));
	AttributeCouple.Add("SpeedMax", FGameplayAttributeCouple(EPKM_Attributes::SPEED, EPKM_AttributesType::MAXIMUM));
}

FPKMAttributeCachedValues UPKM_AttributeSet::ComputeAttributeCachedValues() const
{
	return FPKMAttributeCachedValues(
		GetHpAttribute().GetNumericValue(this),
		GetHpMaxAttribute().GetNumericValue(this),
		GetEnergyAttribute().GetNumericValue(this),
		GetEnergyMaxAttribute().GetNumericValue(this)
	);
}