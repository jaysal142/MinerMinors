// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType/FSharedTarget.h"
#include "GrabForceComponent.generated.h"

class UPrimitiveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINORMINERS_API UGrabForceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UGrabForceComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float HoldDistance = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float Kbase = 600.f;				// position spring base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float Cbase = 60.f;					// position damping base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float ForcePerStrength = 3000.f;	// N per strength point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float KrotBase = 800.f;				// rotation spring base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float CrotBase = 80.f;				// rotation damping base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float TorquePerStrength = 2000.f;	// N*m per strength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float ObjectMaxForce = 1e9f;		// final safety clamp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab|Tuning")
	float ObjectMaxTorque = 1e9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab")
	float Strength = 50.f;

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void StartGrab(UPrimitiveComponent* InComp, const FVector& HitPointWS, bool bMakeLead = true);
	UFUNCTION(BlueprintCallable, Category = "Grab")
	void StopGrab();
	UFUNCTION(BlueprintCallable, Category = "Grab")
	bool IsGrabbing() const { return GrabbedComp != nullptr; }
	UFUNCTION(BlueprintCallable)
	void SetStrength(float InStrength)
	{
		Strength = FMath::Max(0.f, InStrength);
	}

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY() UPrimitiveComponent* GrabbedComp = nullptr;
	FVector LocalgrabOffset = FVector::ZeroVector; // where on object was grabbed (local space)

	void GetAnchor(FVector& OutPos, FQuat OutRot) const;

	void ApplyForces(float Dt, const FVector& TargetPos, const FQuat& TargetRot);

	static void QuatToAxisAngle(const FQuat& Q, FVector& OutAxis, float& OutAngle);
	static FVector ClampMagnitude(const FVector& V, float Max);

	// shared target for co-op
	static TMap<TWeakObjectPtr<UPrimitiveComponent>, FSharedTarget> SharedTargets; // server-only
	void UpdateSharedTarget(bool bAsLead, const FVector& TargetPos, const FQuat& TargetRot);
	FSharedTarget GetSharedTarget() const;
		
};
