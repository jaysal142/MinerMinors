// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/MMActor.h"
#include "Interface/InteractionInterface.h"
#include "MMHarvestableActorBase.generated.h"

UCLASS()
class MINORMINERS_API AMMHarvestableActorBase : public AMMActor, public IInteractionInterface
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PermanentMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<class UItemBase> InventoryItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText InteractionText = FText::FromString("DEFAULT");

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* HarvestMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsHarvested = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ItemCount = 1;

	AMMHarvestableActorBase();

	void UpdateHarvestState();

public:

	UFUNCTION(BlueprintCallable)
	FText GetInteractionText_Implementation() override;
	void Interact_Implementation(AMMCharacter* Caller) override;
	bool IsInteractable_Implementation() const override;

};
