// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Engine/Engine.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::UseItemAtIndex(const int& Index)
{
	const FInventorySlot InventoryContents = GetSlot(Index);
	if (Owner == nullptr)
	{
		return false;
	}

	InventoryContents.ItemClass.GetDefaultObject()->OnUse(Owner);

	return true;
}

void UInventoryComponent::SetOwner(AMMCharacter* Character)
{
	Owner = Character;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();	

	Slots.SetNum(NumSlots);
}


FInventorySlot UInventoryComponent::GetSlot(int32 SlotIndex) const
{
	if (!Slots.IsValidIndex(SlotIndex)) { return FInventorySlot{}; }
	return Slots[SlotIndex];
}

int32 UInventoryComponent::FindEmptySlot(TSubclassOf<UItemBase> ItemClass) const
{

	const UItemBase* Def = ItemClass ? GetDefault<UItemBase>(ItemClass) : nullptr;
	if (Def->GetGemStatus())
	{
		if (Slots[5].IsEmpty())
		{
			return 5;
		}
		return INDEX_NONE;
	}

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].IsEmpty())
		{
			return i;
		}
	}
	return INDEX_NONE;
}

int32 UInventoryComponent::FindStackableSlot(TSubclassOf<UItemBase> ItemClass, int32& OutMaxStack) const
{
	OutMaxStack = 1;
	const UItemBase* Def = ItemClass ? GetDefault<UItemBase>(ItemClass) : nullptr;
	if (Def)
	{
		OutMaxStack = FMath::Max(1, Def->GetMaxStack());
	}

	if (Def->GetGemStatus())
	{
		const FInventorySlot& S = Slots[5];
		const int32 EffectiveMax = (S.MaxStack > 0) ? S.MaxStack : OutMaxStack;
		if (S.ItemClass == ItemClass && S.Count < EffectiveMax)
		{
			return 5; // only return if there’s space
		}
	}

	for (int32 i = 0; i < (Slots.Num() - 1); ++i)
	{
		const FInventorySlot& S = Slots[i];
		const int32 EffectiveMax = (S.MaxStack > 0) ? S.MaxStack : OutMaxStack;
		if (S.ItemClass == ItemClass && S.Count < EffectiveMax)
		{
			return i; // only return if there’s space
		}
	}
	return INDEX_NONE;
}

void UInventoryComponent::CachItemUIAndMaxStack(FInventorySlot& Slot, TSubclassOf<UItemBase> ItemClass)
{
	Slot.ItemClass = ItemClass;
	const UItemBase* Def = IsValid(ItemClass) ? ItemClass->GetDefaultObject<UItemBase>() : nullptr;

	Slot.MaxStack = 1;
	Slot.DisplayName = FText::GetEmpty();
	Slot.Icon = nullptr;

	if (Def)
	{
		Slot.MaxStack = FMath::Max(1, Def->GetMaxStack());
		Slot.DisplayName = Def->GetDisplayName();
		Slot.Icon = Def->GetIconTexture();
	}
}

void UInventoryComponent::BroadcastChange()
{
	OnInventoryChanged.Broadcast();
}

bool UInventoryComponent::TryAddItem(TSubclassOf<UItemBase> ItemClass, int32 Quantity, int32& OutRemainder)
{
	OutRemainder = Quantity;
	if (/*!IsValid(ItemClass) ||*/ Quantity <= 0) return false;

	bool bAddedAny = false;

	// 1. Top existing stacks of same item
	while (OutRemainder > 0)
	{
		int32 MaxStackFromItem = 1;
		int32 StackIdx = FindStackableSlot(ItemClass, MaxStackFromItem);
		if (StackIdx == INDEX_NONE)
		{
			break; // no existing stack
		}

		FInventorySlot& Slot = Slots[StackIdx];
		const int32 EffectiveMax = (Slot.MaxStack > 0) ? Slot.MaxStack : MaxStackFromItem;
		const int32 Space = FMath::Max(0, EffectiveMax - Slot.Count);
		if (Space <= 0)
		{
			// Defensive: avoid infinite loop if a full stack slips through
			UE_LOG(LogTemp, Warning, TEXT("FindStackableSlot returned full stack idx %d (Count=%d, Max=%d)"),
				StackIdx, Slot.Count, EffectiveMax);
			break;
		}

		const int32 ToAdd = FMath::Min(OutRemainder, Space);
		Slot.Count += ToAdd;
		OutRemainder -= ToAdd;
		bAddedAny |= (ToAdd > 0);
	}

	// 2. Create new stack in empty slot
	while (OutRemainder > 0)
	{
		const int32 EmptyIdx = FindEmptySlot(ItemClass);
		if (EmptyIdx == INDEX_NONE)
		{
			break; // inventory full
		}

		FInventorySlot& Slot = Slots[EmptyIdx];
		if (!Slot.IsEmpty())
		{
			break; // safety catch if something breaks
		}
		
		CachItemUIAndMaxStack(Slot, ItemClass);

		// Fallback in case the cache didn’t set it (or item CDO has 0 by mistake)
		if (Slot.MaxStack <= 0)
		{
			const UItemBase* Def = ItemClass ? GetDefault<UItemBase>(ItemClass) : nullptr;
			Slot.MaxStack = Def ? FMath::Max(1, Def->GetMaxStack()) : 1;
		}

		const int32 Space = FMath::Max(0, Slot.MaxStack - Slot.Count);
		const int32 ToAdd = FMath::Min(OutRemainder, Space);
		Slot.ItemClass = ItemClass; // ensure it’s set on first add
		Slot.Count = ToAdd;
		OutRemainder -= ToAdd;
		bAddedAny = bAddedAny || (ToAdd > 0);
	}

	if (bAddedAny)
	{
		BroadcastChange();
	}
	return bAddedAny;
}

bool UInventoryComponent::RemoveFromSlot(int32 SlotIndex, int32 Quantity)
{
	if (!Slots.IsValidIndex(SlotIndex) || Quantity <= 0) return false;

	FInventorySlot& Slot = Slots[SlotIndex];
	if (Slot.IsEmpty()) return false;

	const int32 Removed = FMath::Min(Quantity, Slot.Count);
	Slot.Count -= Removed;
	if (Slot.Count <= 0)
	{
		// clear slot
		Slot = FInventorySlot{};
	}
	BroadcastChange();
	return Removed > 0;
}

