// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/RegrowingHarvestableActorBase.h"

ARegrowingHarvestableActorBase::ARegrowingHarvestableActorBase()
{

}

void ARegrowingHarvestableActorBase::ResetHarvest()
{
	bIsHarvested = false;
	ItemCount = ItemRegrowAmount;
	HarvestMesh->bHiddenInGame = false;
	MarkComponentsRenderStateDirty();
}

void ARegrowingHarvestableActorBase::BeginPlay()
{
	// TODO: subscribe to DayChange Delegate
}

void ARegrowingHarvestableActorBase::OnDayChange()
{
	DaysSinceLastHarvest++;
	if (DaysSinceLastHarvest < NumberOfDaysToRegrow)
	{
		return;
	}

	ResetHarvest();
}
