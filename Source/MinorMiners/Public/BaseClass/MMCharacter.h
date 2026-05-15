// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MMCharacter.generated.h"

UCLASS()
class MINORMINERS_API AMMCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStatlineComponent* Statline;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* Inventory;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CanJump() const;
	void HasJumped();

	bool CanPower() const;

	bool CanSprint() const;
	void SetSprinting(const bool& IsSprinting);

	void SetSneaking(const bool& IsSneaking);

public:	

	AMMCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Ref")
	class AMMCharacter* GetCharacter() { return this; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	UStatlineComponent* GetStatline() const { return Statline; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryComponent* GetInventory() const { return Inventory; }

};
