// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/GemBase.h"
#include "BaseClass/MMPickupActorBase.h"
#include "Engine/DamageEvents.h"

AGemBase::AGemBase()
{
	GemCavityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gem Cavity Mesh"));
	GemCavityMesh->SetupAttachment(RootComponent);
	GemCavityMesh->bHiddenInGame = true;
	GemCavityMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MainGemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Gem Mesh"));
	MainGemMesh->SetupAttachment(GemCavityMesh);
	MainGemMesh->bHiddenInGame = false;
	MainGemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGemBase::SetHarvestState()
{
	if (!bIsHarvested)
	{
		return;
	}

	MainGemMesh->DestroyComponent();
	GemCavityMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GemCavityMesh->bHiddenInGame = false;
	GemCavityMesh->SetVisibility(true, true);
	MarkComponentsRenderStateDirty();
}

void AGemBase::Harvest()
{
	bIsHarvested = true;
	SetHarvestState();
	SpawnPickups();
	OnHarvestedBP();
}

void AGemBase::SpawnPickups()
{
	if (!IsValid(GemPickupActor))
	{
		return;
	}

	for (int i = 0; i < NumOfGemsToSpawn; i++)
	{
		FVector GemLoc = this->GetActorLocation();
		FTransform SpawnTrans = SpawnActorTransforms[i];
		SpawnTrans.SetLocation(SpawnTrans.GetLocation() + GemLoc);
		AMMPickupActorBase* Crystal = GetWorld()->SpawnActor<AMMPickupActorBase>(GemPickupActor, SpawnTrans);
		if (IsValid(Crystal))
		{
			Crystal->SetActorTransform(SpawnTrans);
			OnHarvestedBP();
		}
	}
}

void AGemBase::OnHarvestedBP_Implementation()
{
	OnHarvestedBP();
}

float AGemBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) /**/
{
	if (bIsHarvested /* || !DamageCauser->Tags.Contains("HarvestGem")*/)
	{
		return 0.0f;
	}

	Health -= DamageAmount;
	if (Health > 0.0f)
	{
		return 0.0f;
	}

	Harvest();
	return 0.0f;
}
