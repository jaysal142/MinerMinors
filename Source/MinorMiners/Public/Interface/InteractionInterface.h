// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class MINORMINERS_API IInteractionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetInteractionText();
	virtual FText GetInteractionText_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(class AMMCharacter* Caller);
	virtual void Interact_Implementation(class AMMCharacter* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsInteractable() const;
	virtual bool IsInteractable_Implementation() const;

};
