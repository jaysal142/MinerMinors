// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemBase.h"
#include "Engine/Texture2D.h"
#include "InventoryComponent.generated.h"

class UItemBase;


USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	// Empty when ItemClass == nullptr or Count == 0
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UItemBase> ItemClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;

	// Cached from ItemBase default object on first add
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxStack = 1;

	// UI (Cached once to avoid poking CDO constantly)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UTexture2D* Icon = nullptr;

	bool IsEmpty() const { return ItemClass == nullptr || Count <= 0; }
	int32 SpaceLeft() const { return FMath::Max(0, MaxStack - Count); }
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINORMINERS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumSlots = 6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FInventorySlot> Slots;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	UFUNCTION(BlueprintCallable)
	bool TryAddItem(TSubclassOf<UItemBase> ItemClass, int32 Quantity, int32& OutRemainder);

	UFUNCTION(BlueprintCallable)
	bool RemoveFromSlot(int32 SlotIndex, int32 Quantity);

	// Query helpers for UI
	UFUNCTION(BlueprintPure)
	FInventorySlot GetSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure)
	TArray<FInventorySlot> GetAllSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UItemBase> GetItemClass(FInventorySlot Slot) const { return Slot.ItemClass; }

	UFUNCTION(BlueprintCallable)
	bool UseItemAtIndex(const int& Index);

	UFUNCTION(BlueprintCallable)
	void SetOwner(class AMMCharacter* Character);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AMMCharacter* Owner;

	int32 FindStackableSlot(TSubclassOf<UItemBase> ItemClass, int32& OutMaxStack) const;
	int32 FindEmptySlot(TSubclassOf<UItemBase> ItemClass) const;
	void CachItemUIAndMaxStack(FInventorySlot& Slot, TSubclassOf<UItemBase> ItemClass);
	void BroadcastChange();

};
