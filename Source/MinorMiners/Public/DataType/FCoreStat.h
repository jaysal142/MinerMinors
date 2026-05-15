// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCoreStat.generated.h"

USTRUCT(BlueprintType)
struct FCoreStat
{
	GENERATED_USTRUCT_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float Current = 100;
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float Max = 100;
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float PerSecondTick = 1;
	UPROPERTY(EditAnywhere, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	float AmountModifier = 0;

public:

	FCoreStat()
	{
	}

	FCoreStat(const float& current, const float& max, const float& tick)
	{
		Current = current;
		Max = max;
		PerSecondTick = tick;
	}

	float GetCurrent() const
	{
		return Current;
	}

	float GetMax() const
	{
		return Max;
	}

	float GetTick() const
	{
		return PerSecondTick;
	}

	void SetMax(const float& InMax)
	{
		Max = FMath::Max(0.f, (Max + InMax));
		if (Current > Max)
		{
			Current = Max;
		}
	}

	void SetAmountModifier(const float& InAmount)
	{
		AmountModifier = FMath::Max(0.f, (AmountModifier + InAmount));
	}

	void SetTick(float InTick)
	{
		PerSecondTick = InTick;
	}

	void ClampToMax()
	{
		Current = FMath::Clamp(Current, 0.f, Max);
	}

	void Adjust(const float& Amount)
	{
		Current = FMath::Clamp(Current + Amount, 0.f, Max);
	}

	void ConsumableAdjust(const float& Amount)
	{
		float ModifiedAmount = Amount + AmountModifier;
		ModifiedAmount = Max * (ModifiedAmount / 100);
		Current = FMath::Clamp(Current + ModifiedAmount, 0.f, Max);
	}

	void AdjustTick(const float& NewTick)
	{
		PerSecondTick = NewTick;
	}

	void TickStat(float DeltaSeconds)
	{
		if (PerSecondTick != 0.f)
		{
			Adjust(PerSecondTick * DeltaSeconds);
		}
	}

	float Percentile() const
	{
		return Current / Max;
	}

};
