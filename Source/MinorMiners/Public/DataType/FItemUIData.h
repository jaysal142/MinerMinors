// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FItemUIData.generated.h"

USTRUCT(BlueprintType)
struct FItemUIData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemIdx = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText ItemName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText ItemDescription;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTexture2D* ItemIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int StackSize = 0;

	FItemUIData() {};

	FItemUIData(const int& Idx, const FText& Name, const FText& Description, UTexture2D* Icon, const int& Count)
	{
		StackSize = Count;
		ItemIdx = Idx;
		ItemName = Name;
		ItemDescription = Description;
		ItemIcon = Icon;
	};

};
