// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/MMCharacter.h"
#include "Components/StatlineComponent.h"
#include "Components/InventoryComponent.h"
#include "Items/ConsumableItemBase.h"

// Sets default values
AMMCharacter::AMMCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	Statline = CreateDefaultSubobject<UStatlineComponent>(TEXT("Statline"));
	Statline->SetMovementCompReference(GetCharacterMovement());

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

// Called when the game starts or when spawned
void AMMCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool AMMCharacter::CanJump() const
{
	return Statline->CanJump();
}

void AMMCharacter::HasJumped()
{
	Statline->HasJumped();
	ACharacter::Jump();
}

bool AMMCharacter::CanPower() const
{
	return Statline->CanPower();
}

bool AMMCharacter::CanSprint() const
{
	return Statline->CanSprint();
}

void AMMCharacter::SetSprinting(const bool& IsSprinting)
{
	Statline->SetSprinting(IsSprinting);
}

void AMMCharacter::SetSneaking(const bool& IsSneaking)
{
	Statline->SetSneaking(IsSneaking);
}

// Called every frame
void AMMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
