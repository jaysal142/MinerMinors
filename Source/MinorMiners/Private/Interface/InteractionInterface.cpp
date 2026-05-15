// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/InteractionInterface.h"

FText IInteractionInterface::GetInteractionText_Implementation()
{
	return FText();
}

void IInteractionInterface::Interact_Implementation(AMMCharacter* Caller)
{
	
}

bool IInteractionInterface::IsInteractable_Implementation() const
{
	return false;
}
