// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Components/StatlineComponent.h"
#include "UpgradeItemBase.generated.h"



UCLASS(Blueprintable)
class MINORMINERS_API UUpgradeItemBase : public UItemBase
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<ECoreStat, float> StatAdjustments;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsMaxUpgrade;

public:

	void OnUse(class AMMCharacter* Caller) override;

};
