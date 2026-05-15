// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataType/FExtractItem.h"
#include "DataType/FItemUIData.h"
#include "ItemBase.generated.h"

class AMMPickupActorBase;

UCLASS()
class MINORMINERS_API UItemBase : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FText ItemDescription;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int CurrentStackSize = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FExtractItem> ResourceTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* WorldMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float ItemWeight = 1.f;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TSubclassOf<AMMPickupActorBase> PickupActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 MaxStackSize = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	bool isGem = false;

	UFUNCTION(BlueprintCallable)
	virtual void OnUse(class AMMCharacter* Caller) {};

	UFUNCTION(BlueprintCallable)
	float GetStackWeight() const { return ItemWeight * CurrentStackSize; };
	UFUNCTION(BlueprintCallable)
	float GetItemWeight() const { return ItemWeight; };
	UFUNCTION(BlueprintCallable)
	FItemUIData GetUIData(const int& Idx) const { return FItemUIData(Idx, ItemName, ItemDescription, Icon, CurrentStackSize); };
	UFUNCTION(BlueprintCallable)
	TArray<FExtractItem> GetExtractData() const { return ResourceTags; };
	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetWorldPickupMesh() const { return WorldMesh; };
	UFUNCTION(BlueprintCallable)
	int GetCurrentStack() const { return CurrentStackSize; };

	UFUNCTION(BlueprintPure)
	int32 GetMaxStack() const { return FMath::Max(1, MaxStackSize); };
	UFUNCTION(BlueprintPure)
	const FText& GetDisplayName() const { return ItemName; };
	UFUNCTION(BlueprintPure)
	UTexture2D* GetIconTexture() const { return Icon; };
	UFUNCTION(BlueprintPure)
	bool GetGemStatus() const { return isGem; };

	UFUNCTION(BlueprintCallable)
	int AddToStack(const int& Amount);
	UFUNCTION(BlueprintCallable)
	int RemoveFromStack(const int& Amount);
	UFUNCTION(BlueprintCallable)
	void SetStackSize(const int& Amount) { CurrentStackSize = Amount; };

};
