// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"

int UItemBase::AddToStack(const int& Amount)
{
    int remains = 0;
    if (CurrentStackSize == MaxStackSize)
    {
        return Amount;
    }

    remains = FMath::Clamp(Amount - (MaxStackSize - CurrentStackSize), 0, Amount);
    CurrentStackSize += (Amount - remains);

    return remains;
}

int UItemBase::RemoveFromStack(const int& Amount)
{
    if (CurrentStackSize < Amount)
    {
        int removed = CurrentStackSize;
        CurrentStackSize = 0;
        return removed;
    }

    CurrentStackSize -= Amount;

    return 0;
}
