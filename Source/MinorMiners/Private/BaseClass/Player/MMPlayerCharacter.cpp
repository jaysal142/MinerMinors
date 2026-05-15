// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseClass/Player/MMPlayerCharacter.h"
#include "BaseClass/MMPickupActorBase.h"

#include "Camera/CameraComponent.h"
#include "Components/StatlineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InventoryComponent.h"

#include "Engine/EngineTypes.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

#include "InputActionValue.h"
#include "Interface/InteractionInterface.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


FTimerHandle TimerHandle;

AMMPlayerCharacter::AMMPlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	/*
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;
	*/
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), FName("head"));
	FirstPersonCamera->SetRelativeLocationAndRotation(FVector(5.0f, 10.0f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->bEditableWhenInherited = true;

	Grabber = CreateDefaultSubobject<UGrabForceComponent>(TEXT("Grabber"));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FlickerFunctionMaterialAsset(TEXT("/Game/ThirdPerson/Effects/Flashlight/M_Flicker"));
	if (FlickerFunctionMaterialAsset.Succeeded())
	{
		FlickerMaterial = FlickerFunctionMaterialAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FlashlightFunctionMaterialAsset(TEXT("/Game/ThirdPerson/Effects/Flashlight/M_Flashlight"));
	if (FlashlightFunctionMaterialAsset.Succeeded())
	{
		FlashlightMaterial = FlashlightFunctionMaterialAsset.Object;
	}

	FlashlightArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FlashlightArm"));
	FlashlightArm->SetupAttachment(RootComponent);
	FlashlightArm->TargetArmLength = 0.0f;
	FlashlightArm->bUsePawnControlRotation = true;
	FlashlightArm->AddRelativeLocation(FVector(15.0f, 0.0f, 50.0f));
	FlashlightArm->bEnableCameraRotationLag = true;

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->bEditableWhenInherited = true;
	Spotlight->SetupAttachment(FlashlightArm);
	Spotlight->AddRelativeLocation(FVector(15.0f, 0.0f, 25.0f));
	Spotlight->AttenuationRadius = 800.0f;
	Spotlight->InnerConeAngle = 30.0f;
	Spotlight->OuterConeAngle = 50.0f;

	InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Trigger Volume"));
	InteractionTrigger->SetupAttachment(RootComponent);
	InteractionTrigger->SetRelativeScale3D(FVector(10));
	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMMPlayerCharacter::OnInteractionTriggerOverlapBegin);
	InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &AMMPlayerCharacter::OnInteractionTriggerOverlapEnd);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;
}

void AMMPlayerCharacter::TraceForInteraction()
{
	if (BlockCharacterInput())
	{
		return;
	}

	FCollisionQueryParams LTParams = FCollisionQueryParams(FName(TEXT("InteractionTrace")), true, this);
	LTParams.bReturnPhysicalMaterial = false;
	LTParams.bReturnFaceIndex = false;

	GetWorld()->DebugDrawTraceTag = DEBUG_SHOW_INTERACTION_TRACE ? TEXT("InteractionTrace") : TEXT("NONE");

	FHitResult LTHit(ForceInit);
	FVector LTStart = FirstPersonCamera->GetComponentLocation();
	float SearchLength = 100;
	SearchLength += InteractionTraceLength;
	FVector LTEnd = (FirstPersonCamera->GetForwardVector() * SearchLength) + LTStart;

	GetWorld()->LineTraceSingleByChannel(LTHit, LTStart, LTEnd, ECC_Visibility, LTParams);

	if (!LTHit.bBlockingHit || !LTHit.GetActor()->Implements<UInteractionInterface>())
	{
		InteractionActor = nullptr;
		UpdateInteractionText_Implementation();
		return;
	}
	InteractionActor = LTHit.GetActor();
	UpdateInteractionText_Implementation(); 
}

bool AMMPlayerCharacter::BlockCharacterInput() const
{
	return false;
}

void AMMPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMMPlayerCharacter::PlayerJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMMPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMMPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMMPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMMPlayerCharacter::SprintOn);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMMPlayerCharacter::SprintOff);
		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Started, this, &AMMPlayerCharacter::SneakOn);
		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Completed, this, &AMMPlayerCharacter::SneakOff);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Started, this, &AMMPlayerCharacter::GrabOn);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &AMMPlayerCharacter::GrabOff);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AMMPlayerCharacter::OnInteract);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Completed, this, &AMMPlayerCharacter::DoDropItem);
		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Completed, this, &AMMPlayerCharacter::DoUseItem);
	}
}

#pragma region MovementFunctions

void AMMPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (BlockCharacterInput())
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMMPlayerCharacter::Look(const FInputActionValue& Value)
{
	if (BlockCharacterInput())
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMMPlayerCharacter::PlayerJump()
{
	if (BlockCharacterInput())
	{
		return;
	}

	if (CanJump())
	{
		HasJumped();
	}
}

void AMMPlayerCharacter::SprintOn()
{
	if (BlockCharacterInput())
	{
		return;
	}

	if (CanSprint())
	{
		SetSprinting(true);
	}
}

void AMMPlayerCharacter::SprintOff()
{
	if (BlockCharacterInput())
	{
		return;
	}

	SetSprinting(false);
}

void AMMPlayerCharacter::SneakOn()
{
	if (BlockCharacterInput())
	{
		return;
	}

	SetSneaking(true);

	if (Spotlight && bFlashlightOn)
	{
		FlashlightOff();
	}
}

void AMMPlayerCharacter::SneakOff()
{
	if (BlockCharacterInput())
	{
		return;
	}

	SetSneaking(false);

	if (Spotlight && !bFlashlightOn)
	{
		FlashlightOn();
	}
}

void AMMPlayerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMMPlayerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

#pragma endregion

#pragma region InteractionFunctions

void AMMPlayerCharacter::GrabOn()
{
	if (BlockCharacterInput())
	{
		return;
	}

	if (bIsPickaxeEquipped)
	{
		MineOn();
		return;
	}

	FVector CamL;
	FRotator CamR;
	GetController()->GetPlayerViewPoint(CamL, CamR);

	const FVector Start = CamL;
	const FVector End = Start + CamR.Vector() * 400.f;

	FHitResult Hit;
	FCollisionQueryParams Q;
	Q.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, FCollisionObjectQueryParams(ECC_PhysicsBody), Q);
	if (Hit.bBlockingHit && Hit.GetComponent())
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (AMMPickupActorBase* Pickup = Cast<AMMPickupActorBase>(HitActor))
			{
				if (Pickup->bDisableGrabbing)
				{
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(
							-1,                 // Unique key to prevent duplicate messages (-1 for no key)
							5.0f,               // Duration the message will display (in seconds)
							FColor::Green,        // Color of the text
							FString::Printf(TEXT("Too delicate... this should go in my bag.")) // The message to display, using FString::Printf for formatting
						);
					}
					return; // don’t grab this one
				}
			}
		}

		Grabber->StartGrab(Hit.GetComponent(), Hit.ImpactPoint, /*lead*/ true);
	}
}

void AMMPlayerCharacter::GrabOff()
{
	if (BlockCharacterInput())
	{
		return;
	}

	if (bIsPickaxeEquipped)
	{
		MineOff();
		return;
	}

	Grabber->StopGrab();
}

void AMMPlayerCharacter::MineOn()
{
	if (!bIsPickaxeEquipped || BlockCharacterInput())
	{
		return;
	}

	bIsMining = true;
}

void AMMPlayerCharacter::MineOff()
{
	bIsMining = false;
}

void AMMPlayerCharacter::OnInteract()
{
	if (BlockCharacterInput())
	{
		return;
	}
	if (InteractionActor == nullptr)
	{
		return;
	}
	IInteractionInterface* Interaction = Cast<IInteractionInterface>(InteractionActor);
	if (Interaction == nullptr)
	{
		return;
	}
	Interaction->Execute_Interact(InteractionActor, this);
	UpdateInteractionText_Implementation();
}

// Index Setter
void AMMPlayerCharacter::SetIndex(int index)
{
	Index = index;
}

// Drop Item Logic
void AMMPlayerCharacter::DoDropItem()
{
	if (!GetInventory()) return;

	if (Index < 0 || Index >= GetInventory()->NumSlots) return;

	const FInventorySlot CurrentSlot = GetInventory()->GetSlot(Index);
	if (CurrentSlot.IsEmpty()) return;

	const UItemBase* ItemCDO = CurrentSlot.ItemClass ? CurrentSlot.ItemClass->GetDefaultObject<UItemBase>() : nullptr;
	if (!ItemCDO || !ItemCDO->PickupActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drop failed: no PickupActorClass on item %s"), *GetNameSafe(CurrentSlot.ItemClass));
		return;
	}

	const int32 DropAmount = 1;

	const FVector CamLocation = FirstPersonCamera ? FirstPersonCamera->GetComponentLocation() : GetActorLocation();
	const FRotator CamRotation = FirstPersonCamera ? FirstPersonCamera->GetComponentRotation() : GetActorRotation();
	const FVector SpawnLocation = CamLocation + CamRotation.Vector() * 100.f;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	AMMPickupActorBase* PickupActor = GetWorld()->SpawnActor<AMMPickupActorBase>(ItemCDO->PickupActorClass, SpawnLocation, CamRotation, Params);
	if (!PickupActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Drop spawn failed for %s"), *GetNameSafe(ItemCDO->PickupActorClass));
		return;
	}

	PickupActor->InitFromItem(CurrentSlot.ItemClass, DropAmount);

	GetInventory()->RemoveFromSlot(Index, DropAmount);

	OnDropItemCompleted_Implementation();

	UE_LOG(LogTemp, Log, TEXT("Dropped %d of %s"), DropAmount, *GetNameSafe(CurrentSlot.ItemClass));
}

// Use Item Logic
void AMMPlayerCharacter::DoUseItem()
{
	if (!GetInventory()) return;

	if (Index < 0 || Index >= GetInventory()->NumSlots) return;

	const FInventorySlot CurrentSlot = GetInventory()->GetSlot(Index);
	if (CurrentSlot.IsEmpty()) return;

	const TSubclassOf<class UItemBase> ItemClass = GetInventory()->GetItemClass(CurrentSlot);
	if (!ItemClass) return;
	const UItemBase* Def = ItemClass ? GetDefault<UItemBase>(ItemClass) : nullptr;

	if (Def->GetGemStatus())
	{
		return;
	}

	GetInventory()->SetOwner(GetCharacter());

	GetInventory()->UseItemAtIndex(Index);
	GetInventory()->RemoveFromSlot(Index, 1);

	OnUseItemCompleted_Implementation();

	UE_LOG(LogTemp, Log, TEXT("Used %d of %s"), 1, *GetNameSafe(CurrentSlot.ItemClass));
}

void AMMPlayerCharacter::OnDropItemCompleted_Implementation()
{
	OnDropItemCompleted();
}

void AMMPlayerCharacter::OnUseItemCompleted_Implementation()
{
	OnUseItemCompleted();
}

void AMMPlayerCharacter::OnInteractionTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UInteractionInterface>())
	{
		return;
	}
	InteractableActors.Add(OtherActor);
	bEnableRayTrace = true;
}

void AMMPlayerCharacter::OnInteractionTriggerOverlapEnd(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (!OtherActor->Implements<UInteractionInterface>())
	{
		return;
	}
	InteractableActors.Remove(OtherActor);
	bEnableRayTrace = InteractableActors.Num() > 0;
	if (!bEnableRayTrace)
	{
		UpdateInteractionText_Implementation();
	}
}

void AMMPlayerCharacter::UpdateInteractionText_Implementation()
{
	UpdateInteractionText();
}

#pragma endregion

void AMMPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	FlashlightOn();
}

void AMMPlayerCharacter::Tick(float DeltaTime)
{
	if (bEnableRayTrace)
	{
		TraceForInteraction();
	}
	if (!CanPower() && bFlashlightOn)
	{
		FlashlightOff();
	}
}

void AMMPlayerCharacter::FlashlightOn()
{
	if (CanPower())
	{
		PlayMySoundAtLocation(FlashlightOnAudio, Spotlight->GetRelativeLocation());
		Spotlight->SetLightFunctionMaterial(FlickerMaterial);
		Spotlight->SetVisibility(true);
		bFlashlightOn = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				Spotlight->SetLightFunctionMaterial(FlashlightMaterial);
			}, 1.0f, false);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("NO POWER!!"));
		}
	}
}

void AMMPlayerCharacter::FlashlightOff()
{
	StopMySound();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FlashlightOffAudio, Spotlight->GetComponentLocation(), 1.0f, 1.0f, 0.0f);
	Spotlight->SetVisibility(false);
	bFlashlightOn = false;
}

void AMMPlayerCharacter::PlayMySoundAtLocation(USoundBase* SoundToPlay, FVector Location)
{
	MyAudioComponent = UGameplayStatics::CreateSound2D(this, SoundToPlay); // Or CreateSoundAtLocation
	if (MyAudioComponent)
	{
		MyAudioComponent->SetWorldLocation(Location); // Set the desired location
		MyAudioComponent->Play();
	}
}

void AMMPlayerCharacter::StopMySound()
{
	if (MyAudioComponent && MyAudioComponent->IsPlaying())
	{
		MyAudioComponent->Stop();
	}
}