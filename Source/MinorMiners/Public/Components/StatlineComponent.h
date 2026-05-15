// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType/FCoreStat.h"
#include "StatlineComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINORMINERS_API UStatlineComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "Sprint", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, Category = "Sprint", meta = (AllowPrivateAccess = "true"))
	float SprintCostMultiplier = 20.f;
	UPROPERTY(EditAnywhere, Category = "Sprint", meta = (AllowPrivateAccess = "true"))
	float SecondsForStaminaExhaustion = 3.f;
	UPROPERTY(EditAnywhere, Category = "Sprint", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsSneaking = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SneakSpeed = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float JumpCost = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 175.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentStaminaExhaustion = 0.f;

	void TickStats(const float& DeltaTime);
	void TickStamina(const float& DeltaTime);
	void TickPower(const float& DeltaTime);
	bool IsValidSprinting();

protected:

	virtual void BeginPlay() override;

public:

	UStatlineComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FCoreStat Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FCoreStat Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FCoreStat Power = FCoreStat(100, 100, -0.5);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FCoreStat Strength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Refs")
	class UCharacterMovementComponent* MovementComp = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetMovementCompReference(UCharacterMovementComponent* InMove)
	{
		MovementComp = InMove;
	}

	UFUNCTION(BlueprintCallable)
	float GetStatPercentile(const ECoreStat Stat) const;
	UFUNCTION(BlueprintCallable)
	float GetStatMax(const ECoreStat Stat) const;

	UFUNCTION(BlueprintCallable)
	bool CanPower() const;
	UFUNCTION(BlueprintCallable)
	bool CanSprint() const;
	UFUNCTION(BlueprintCallable)
	void SetSprinting(const bool& IsSprinting);
	UFUNCTION(BlueprintCallable)
	void SetSneaking(const bool& IsSneaking);
	UFUNCTION(BlueprintCallable)
	bool CanJump();
	UFUNCTION(BlueprintCallable)
	void HasJumped();

	UFUNCTION(BlueprintCallable)
	void AdjustStat(const ECoreStat& Stat, const float& Amount);
	UFUNCTION(BlueprintCallable)
	void ConsumableAdjustStat(const ECoreStat& Stat, const float& Amount);
	UFUNCTION(BlueprintCallable)
	void SetAdjustModifier(const ECoreStat& Stat, const float& Amount);
	UFUNCTION(BlueprintCallable)
	void SetStatMax(const ECoreStat Stat, const float& Amount);

};
