// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StatlineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataType/FCoreStat.h"
#include "DataType/ECoreStat.h"

float StaminaRecoveryTimeAccumulator = 0.0f;

void UStatlineComponent::TickStats(const float& DeltaTime)
{
	TickStamina(DeltaTime);
	TickPower(DeltaTime);
	Health.TickStat(DeltaTime);
	Strength.TickStat(DeltaTime);
}

void UStatlineComponent::TickStamina(const float& DeltaTime)
{
	if (CurrentStaminaExhaustion > 0.0)
	{
		CurrentStaminaExhaustion -= DeltaTime;
		return;
	}

	if (bIsSprinting && IsValidSprinting())
	{
		Stamina.TickStat(0 - (DeltaTime * SprintCostMultiplier));
		if (Stamina.GetCurrent() <= 0.0)
		{
			SetSprinting(false);
			CurrentStaminaExhaustion = SecondsForStaminaExhaustion;
		}
		return;
	}

	StaminaRecoveryTimeAccumulator += DeltaTime;

	float RecoveryMultiplier = FMath::Clamp(FMath::Pow(StaminaRecoveryTimeAccumulator, 1.5f), 1.0f, 6.5f);
	Stamina.TickStat(DeltaTime * RecoveryMultiplier);
}

void UStatlineComponent::TickPower(const float& DeltaTime)
{
	if (Power.GetCurrent() <= 0.0 || bIsSneaking)
	{
		return;
	}

	Power.TickStat(DeltaTime);
}

bool UStatlineComponent::IsValidSprinting()
{
	return MovementComp->Velocity.Length() > WalkSpeed && !MovementComp->IsFalling();
}

UStatlineComponent::UStatlineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatlineComponent::BeginPlay()
{
	Super::BeginPlay();
	MovementComp->MaxWalkSpeed = WalkSpeed;
}

void UStatlineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TickType != ELevelTick::LEVELTICK_PauseTick)
	{
		TickStats(DeltaTime);
	}
}

float UStatlineComponent::GetStatPercentile(const ECoreStat Stat) const
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		return Health.Percentile();
	case ECoreStat::CS_STAMINA:
		return Stamina.Percentile();
	case ECoreStat::CS_POWER:
		return Power.Percentile();
	case ECoreStat::CS_STRENGTH:
		return Strength.Percentile();
	default:
		break;
	}
	return -1;
}

float UStatlineComponent::GetStatMax(const ECoreStat Stat) const
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		return Health.GetMax();
	case ECoreStat::CS_STAMINA:
		return Stamina.GetMax();
	case ECoreStat::CS_POWER:
		return Power.GetMax();
	case ECoreStat::CS_STRENGTH:
		return Strength.GetMax();
	default:
		break;
	}
	return -1;
}

bool UStatlineComponent::CanPower() const
{
	return Power.GetCurrent() > 0.0;
}

bool UStatlineComponent::CanSprint() const
{
	return Stamina.GetCurrent() > 0.0;
}

void UStatlineComponent::SetSprinting(const bool& IsSprinting)
{
	bIsSprinting = IsSprinting;
	if (bIsSneaking && !bIsSprinting)
	{
		return;
	}

	bIsSneaking = false;
	MovementComp->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void UStatlineComponent::SetSneaking(const bool& IsSneaking)
{
	bIsSneaking = IsSneaking;
	if (bIsSprinting && !bIsSneaking)
	{
		return;
	}

	bIsSprinting = false;
	MovementComp->MaxWalkSpeed = bIsSneaking ? SneakSpeed : WalkSpeed;
}

bool UStatlineComponent::CanJump()
{
	return Stamina.GetCurrent() >= JumpCost;
}

void UStatlineComponent::HasJumped()
{
	Stamina.Adjust(0 - JumpCost);
}

void UStatlineComponent::AdjustStat(const ECoreStat& Stat, const float& Amount)
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		Health.Adjust(Amount);
		break;
	case ECoreStat::CS_STAMINA:
		Stamina.Adjust(Amount);
		break;
	case ECoreStat::CS_POWER:
		Power.Adjust(Amount);
		break;
	case ECoreStat::CS_STRENGTH:
		Strength.Adjust(Amount);
		break;
	default:
		break;
	}
	return;
}

void UStatlineComponent::ConsumableAdjustStat(const ECoreStat& Stat, const float& Amount)
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		Health.ConsumableAdjust(Amount);
		break;
	case ECoreStat::CS_STAMINA:
		Stamina.ConsumableAdjust(Amount);
		break;
	case ECoreStat::CS_POWER:
		Power.ConsumableAdjust(Amount);
		break;
	case ECoreStat::CS_STRENGTH:
		Strength.ConsumableAdjust(Amount);
		break;
	default:
		break;
	}
	return;
}

void UStatlineComponent::SetAdjustModifier(const ECoreStat& Stat, const float& Amount)
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		Health.SetAmountModifier(Amount);
		break;
	case ECoreStat::CS_STAMINA:
		Stamina.SetAmountModifier(Amount);
		break;
	case ECoreStat::CS_POWER:
		Power.SetAmountModifier(Amount);
		break;
	case ECoreStat::CS_STRENGTH:
		Strength.SetAmountModifier(Amount);
		break;
	default:
		break;
	}
	return;
}

void UStatlineComponent::SetStatMax(const ECoreStat Stat, const float& Amount)
{
	switch (Stat)
	{
	case ECoreStat::CS_HEALTH:
		return Health.SetMax(Amount);
	case ECoreStat::CS_STAMINA:
		return Stamina.SetMax(Amount);
	case ECoreStat::CS_POWER:
		return Power.SetMax(Amount);
	case ECoreStat::CS_STRENGTH:
		return Strength.SetMax(Amount);
	default:
		break;
	}
	return;
}
