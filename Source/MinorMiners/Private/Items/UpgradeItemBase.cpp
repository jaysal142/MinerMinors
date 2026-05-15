// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/UpgradeItemBase.h"
#include "BaseClass/MMCharacter.h"

void UUpgradeItemBase::OnUse(AMMCharacter* Caller)
{
	UStatlineComponent* statline = Caller->GetStatline();

	if (bIsMaxUpgrade)
	{
		for (TPair<ECoreStat, float>& affect : StatAdjustments)
		{
			statline->SetStatMax(affect.Key, affect.Value);
		}
		return;
	}
	
	for (TPair<ECoreStat, float>& affect : StatAdjustments)
	{
		statline->SetAdjustModifier(affect.Key, affect.Value);
	}
}