// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseClass/MMActor.h"
#include "GemBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class MINORMINERS_API AGemBase : public AMMActor
{
	GENERATED_BODY()

private:

	UPROPERTY(SaveGame)
	bool bIsHarvested = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Health = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MainGemMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* GemCavityMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<class AMMPickupActorBase> GemPickupActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int NumOfGemsToSpawn = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FTransform> SpawnActorTransforms;

	AGemBase();

	void SetHarvestState();
	void Harvest();
	void SpawnPickups();

protected:

public:

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser); /**/

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnHarvestedBP();
	void OnHarvestedBP_Implementation();

};
