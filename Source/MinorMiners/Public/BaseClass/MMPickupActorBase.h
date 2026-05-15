// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/MMActor.h"
#include "Interface/InteractionInterface.h"
#include "MMPickupActorBase.generated.h"

UCLASS()
class MINORMINERS_API AMMPickupActorBase : public AMMActor, public IInteractionInterface
{
	GENERATED_BODY()

private:

	AMMPickupActorBase();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WorldMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<class UItemBase> InventoryItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ItemCount = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText InteractionText = FText::FromString("DEFAULT");

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	bool bDisableGrabbing = false;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void InitFromItem(TSubclassOf<UItemBase> InItemClass, int32 InQuantity)
	{
		InventoryItem = InItemClass;
		ItemCount = InQuantity;
	}

	UFUNCTION(BlueprintCallable)
	FText GetInteractionText_Implementation() override;
	void Interact_Implementation(class AMMCharacter* Caller) override;
	bool IsInteractable_Implementation() const override;

};
