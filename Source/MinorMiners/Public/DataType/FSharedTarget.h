// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSharedTarget.generated.h"

USTRUCT()
struct FSharedTarget
{
	GENERATED_BODY()

	FVector Pos = FVector::ZeroVector;
	FQuat Rot = FQuat::Identity;
	TWeakObjectPtr<AActor> Lead; // who updates target this frame
	bool IsValid() const { return Rot.IsNormalized(); }
};
