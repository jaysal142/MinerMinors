// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/MMCharacter.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/GrabForceComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "MMPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

UCLASS(abstract)
class MINORMINERS_API AMMPlayerCharacter : public AMMCharacter
{
	GENERATED_BODY()
	
private:

#pragma region COMPONENT

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FlashlightArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* Spotlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grab, meta = (AllowPrivateAccess = "true"))
	UGrabForceComponent* Grabber = nullptr;

	UPROPERTY(EditAnywhere, Category = Material, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* FlickerMaterial;
	UPROPERTY(EditAnywhere, Category = Material, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* FlashlightMaterial;
	
	UPROPERTY(EditAnywhere, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* FlashlightOnAudio;
	UPROPERTY(EditAnywhere, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* FlashlightOffAudio;
	UPROPERTY(EditAnywhere, Category = Audio, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* MyAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractionTrigger;

#pragma endregion

#pragma region INPUT

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SneakAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* GrabAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseAction;

#pragma endregion

#pragma region VARIABLE

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bEnableRayTrace = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> InteractableActors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float InteractionTraceLength = 200;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	AActor* InteractionActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	float GrabDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DEBUG, meta = (AllowPrivateAccess = "true"))
	bool DEBUG_SHOW_INTERACTION_TRACE = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bInventoryIsShown = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	bool bFlashlightOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int Index = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	bool bIsMining = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	bool bIsPickaxeEquipped = false;

#pragma endregion

	void TraceForInteraction();

	bool BlockCharacterInput() const;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void PlayerJump();

	void SprintOn();
	void SprintOff();

	void SneakOn();
	void SneakOff();

	void GrabOn();
	void GrabOff();

	void MineOn();
	void MineOff();

	void OnInteract();

	void FlashlightOn();
	void FlashlightOff();

	virtual void BeginPlay() override;

public:

	AMMPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetIndex(int num);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoDropItem();
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoUseItem();

	UFUNCTION(BlueprintNativeEvent, Category = "MyEvents")
	void OnDropItemCompleted(); // Declaration
	void OnDropItemCompleted_Implementation(); // Declaration

	UFUNCTION(BlueprintNativeEvent, Category = "MyEvents")
	void OnUseItemCompleted();
	void OnUseItemCompleted_Implementation();


	/*
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return ThirdPersonCamera; } */
	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	UFUNCTION()
	void OnInteractionTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInteractionTriggerOverlapEnd(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
	void UpdateInteractionText();
	void UpdateInteractionText_Implementation();

	void PlayMySoundAtLocation(USoundBase* SoundToPlay, FVector Location);
	void StopMySound();

};
