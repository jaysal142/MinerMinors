// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ConsumableItemBase.h"
#include "BaseClass/MMCharacter.h"

void UConsumableItemBase::OnUse(AMMCharacter* Caller)
{
	UStatlineComponent* statline = Caller->GetStatline();

	for (TPair<ECoreStat, float>& affect : StatAdjustments)
	{
		statline->ConsumableAdjustStat(affect.Key, affect.Value);
	}
}
