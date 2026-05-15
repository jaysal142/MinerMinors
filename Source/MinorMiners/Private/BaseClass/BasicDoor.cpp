// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/BasicDoor.h"

// Sets default values
ABasicDoor::ABasicDoor()
{

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasicDoor::BeginPlay()
{
	Super::BeginPlay();
	BaseYaw = GetActorRotation().Yaw;
}

// Called every frame
void ABasicDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldOpenOrClose)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = CurrentRotation;

		NewRotation.Yaw += YawIncrease;
		SetActorRotation(NewRotation);

		const float FromClosed = FMath::FindDeltaAngleDegrees(BaseYaw, NewRotation.Yaw);

		if ((YawIncrease > 0.f && FromClosed >= OpenAngle) || (YawIncrease < 0.f && FromClosed <= 0.f))
		{
			ShouldOpenOrClose = false;

			FRotator Clamped = NewRotation;
			Clamped.Yaw = BaseYaw + FMath::Clamp(FromClosed, 0.f, OpenAngle);
			SetActorRotation(Clamped);

			bIsOpen = (FromClosed >= OpenAngle);
		}
	}

}

void ABasicDoor::OpenDoor()
{
	const float FromClosed = FMath::FindDeltaAngleDegrees(BaseYaw, GetActorRotation().Yaw);
	if (FromClosed < OpenAngle)
	{
		YawIncrease = 2.0f;
		ShouldOpenOrClose = true;
	}
}

void ABasicDoor::CloseDoor()
{
	const float FromClosed = FMath::FindDeltaAngleDegrees(BaseYaw, GetActorRotation().Yaw);
	if (FromClosed > 0.f)
	{
		YawIncrease = -2.0f;
		ShouldOpenOrClose = true;
	}
}

FText ABasicDoor::GetInteractionText_Implementation()
{
	return bIsOpen ? FText::FromString("Close") : FText::FromString("Open");
}

void ABasicDoor::Interact_Implementation(class AMMCharacter* Caller)
{
	Execute_Interact(this, Caller);
}

bool ABasicDoor::IsInteractable_Implementation() const
{
	return true;
}
