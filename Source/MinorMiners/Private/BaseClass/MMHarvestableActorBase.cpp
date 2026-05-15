// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/MMHarvestableActorBase.h"
#include "Components/InventoryComponent.h"
#include "BaseClass/MMCharacter.h"
#include "Items/ItemBase.h"

AMMHarvestableActorBase::AMMHarvestableActorBase()
{
	PermanentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Permanent Mesh"));
	HarvestMesh = CreateDefaultSubobject<UStaticMeshComponent> (TEXT("Harvest Mesh"));
	PermanentMesh->SetupAttachment(RootComponent);
	HarvestMesh->SetupAttachment(PermanentMesh);
}

void AMMHarvestableActorBase::UpdateHarvestState()
{
	HarvestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PermanentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HarvestMesh->bHiddenInGame = true;
	MarkComponentsRenderStateDirty();
}

FText AMMHarvestableActorBase::GetInteractionText_Implementation()
{
	return InteractionText;
}

void AMMHarvestableActorBase::Interact_Implementation(AMMCharacter* Caller)
{
}

bool AMMHarvestableActorBase::IsInteractable_Implementation() const
{
	return !bIsHarvested;
}
