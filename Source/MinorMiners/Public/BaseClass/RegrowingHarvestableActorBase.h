// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/MMHarvestableActorBase.h"
#include "RegrowingHarvestableActorBase.generated.h"

/**
 * 
 */
UCLASS()
class MINORMINERS_API ARegrowingHarvestableActorBase : public AMMHarvestableActorBase
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int NumberOfDaysToRegrow = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int DaysSinceLastHarvest = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ItemRegrowAmount = 1;

	ARegrowingHarvestableActorBase();

	void ResetHarvest();

protected:

	virtual void BeginPlay();

public:

	void OnDayChange();
	
};
