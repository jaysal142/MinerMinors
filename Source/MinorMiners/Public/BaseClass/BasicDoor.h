// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "BasicDoor.generated.h"

UCLASS()
class MINORMINERS_API ABasicDoor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsOpen = false;

	bool ShouldOpenOrClose;
	float YawIncrease;

	float BaseYaw = 0.f;
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenAngle = 90.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	virtual void BeginPlay() override;

public:	

	ABasicDoor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void OpenDoor();
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void CloseDoor();

	FText GetInteractionText_Implementation() override;
	void Interact_Implementation(class AMMCharacter* Caller) override;
	bool IsInteractable_Implementation() const override;

};
