// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/MMPickupActorBase.h"
#include "Components/InventoryComponent.h"
#include "BaseClass/MMCharacter.h"
#include "Items/ItemBase.h"
#include "Engine/Engine.h"

AMMPickupActorBase::AMMPickupActorBase()
{
	WorldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("World Mesh"));
	WorldMesh->SetupAttachment(RootComponent);
}

FText AMMPickupActorBase::GetInteractionText_Implementation()
{
	return InteractionText;
}

void AMMPickupActorBase::Interact_Implementation(class AMMCharacter* Caller)
{
	if (!IsValid(Caller)) return;
	if (!IsValid(InventoryItem)) return;

	UInventoryComponent* Inventory = Caller->GetInventory();
	if (!Inventory) return;

	int32 Remainder = 0;
	const bool bAddedany = Inventory->TryAddItem(InventoryItem, ItemCount, Remainder);

	if (bAddedany && Remainder == 0)
	{
		Destroy();
		return;
	}

	if (bAddedany && Remainder > 0)
	{
		// some fit, some didnt (leave remainder in world)
		ItemCount = Remainder;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 2.0f, FColor::Yellow,
				FString::Printf(TEXT("Inventory partial: %d couldn't fit"), Remainder));
		}
		return;
	}

	// Inventory full
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.0f, FColor::Red, TEXT("Inventory full"));
	}
}

bool AMMPickupActorBase::IsInteractable_Implementation() const
{
	return true;
}
