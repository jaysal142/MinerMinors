// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GrabForceComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PrimitiveComponent.h"

#pragma region PUBLIC

TMap<TWeakObjectPtr<UPrimitiveComponent>, FSharedTarget> UGrabForceComponent::SharedTargets;

UGrabForceComponent::UGrabForceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics; // apply after game tick, before/with physics
}

void UGrabForceComponent::StartGrab(UPrimitiveComponent* InComp, const FVector& HitPointWS, bool bMakeLead)
{
	if (!InComp) return;
	if (!InComp->IsSimulatingPhysics()) return;

	GrabbedComp = InComp;
	GrabbedComp->WakeAllRigidBodies();

	// object grab location
	const FTransform Trans = GrabbedComp->GetComponentTransform();
	LocalgrabOffset = Trans.InverseTransformPosition(HitPointWS);

	// boost damping to feel heavier
	GrabbedComp->SetLinearDamping(3.f);
	GrabbedComp->SetAngularDamping(6.f);

	// initialize shared target
	FVector P = FVector::ZeroVector; 
	FQuat R = FQuat::Identity;
	GetAnchor(P, R);
	UpdateSharedTarget(/*bAsLeas = */bMakeLead, P, R); // acts as private target until UNCOMMENTED
}

void UGrabForceComponent::StopGrab()
{
	if (GrabbedComp)
	{
		// restore damping
		GrabbedComp->SetLinearDamping(0.2f);
		GrabbedComp->SetAngularDamping(0.2f);
	}
	GrabbedComp = nullptr;
}

#pragma endregion

#pragma region PROTECTED

void UGrabForceComponent::BeginPlay()
{
	Super::BeginPlay();
}

static UCameraComponent* FindFirstCamera(const AActor* Owner)
{
	if (!Owner) return nullptr;
	TArray<UCameraComponent*> Cams;
	Owner->GetComponents<UCameraComponent>(Cams);
	return Cams.Num() ? Cams[0] : nullptr;
}

void UGrabForceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!GrabbedComp) return;

	// compute/refresh anchor (lead updates the shared target; helpers read it)
	FVector MyAnchorPos = FVector::ZeroVector; 
	FQuat MyAnchorRot = FQuat::Identity;
	GetAnchor(MyAnchorPos, MyAnchorRot);
	UpdateSharedTarget(/*bAsLead = */true, MyAnchorPos, MyAnchorRot); // no lead for now -- UNCOMMENT for co-op

	// use shared target for multiplayer pull point
	const FSharedTarget ST = GetSharedTarget();
	ApplyForces(DeltaTime, ST.Pos, ST.Rot);
}

#pragma endregion

#pragma region PRIVATE

void UGrabForceComponent::GetAnchor(FVector& OutPos, FQuat OutRot) const
{
	OutPos = FVector::ZeroVector;
	OutRot = FQuat::Identity;

	const AActor* Owner = GetOwner();
	if (!Owner) return;

	if (const UCameraComponent* Cam = FindFirstCamera(Owner))
	{
		OutPos = Cam->GetComponentLocation() + Cam->GetForwardVector() * HoldDistance;
		OutRot = Cam->GetComponentQuat();
		return;
	}

	OutPos = Owner->GetActorLocation() + Owner->GetActorForwardVector() * HoldDistance;
	OutRot = Owner->GetActorQuat();
}

void UGrabForceComponent::ApplyForces(float Dt, const FVector& TargetPos, const FQuat& TargetRot)
{
	if (!GrabbedComp) return;

	const float Mass = GrabbedComp->GetMass();
	const FVector ComWS = GrabbedComp->GetCenterOfMass();

	// -- position PD --
	const FVector X = ComWS;
	const FVector V = GrabbedComp->GetPhysicsLinearVelocity();

	const float Ratio = Strength / FMath::Max(1.f, Mass);
	const float K = Kbase * Ratio;
	const float C = Cbase * (0.5 + 0.5f / FMath::Max(0.1f, Ratio));

	FVector F = (K * (TargetPos - X)) - (C * V);

	// perplayer hard cap
	const float MaxF = Strength * ForcePerStrength;
	F = ClampMagnitude(F, MaxF);

	// optional object level ultimate cap
	F = ClampMagnitude(F, ObjectMaxForce);

	GrabbedComp->AddForce(F, NAME_None, /*bAccelChange = */false);

	// -- orientation PD --
	const FQuat ObjQ = GrabbedComp->GetComponentQuat();
	FQuat Delta = TargetRot * ObjQ.Inverse();
	FVector Axis;
	float Angle = 0.f;
	QuatToAxisAngle(Delta, Axis, Angle);

	const FVector W = GrabbedComp->GetPhysicsAngularVelocityInRadians();

	const float Krot = KrotBase * Ratio;
	const float Crot = CrotBase * (0.5f + 0.5f / FMath::Max(0.1f, Ratio));

	FVector T = (Krot * Axis * Angle) - (Crot * W);
	const float MaxT = Strength * TorquePerStrength;
	T = ClampMagnitude(T, MaxT);
	T = ClampMagnitude(T, ObjectMaxTorque);

	GrabbedComp->AddTorqueInRadians(T, NAME_None, /*bAccelChange = */false);
}

void UGrabForceComponent::QuatToAxisAngle(const FQuat& Q, FVector& OutAxis, float& OutAngle)
{
	FQuat N = Q;
	if (!N.IsNormalized()) N.Normalize();
	N.ToAxisAndAngle(OutAxis, OutAngle);
	if (!OutAxis.IsNearlyZero()) OutAxis.Normalize();
	// map to smallest angle representation
	if (OutAngle > PI)
	{
		OutAngle = 2 * PI - OutAngle;
		OutAxis *= -1.f;
	}
}

FVector UGrabForceComponent::ClampMagnitude(const FVector& V, float Max)
{
	const float S2 = V.SizeSquared();
	if (S2 <= Max * Max)
	{
		return V;
	}
	return V.GetSafeNormal() * Max;
}

void UGrabForceComponent::UpdateSharedTarget(bool bAsLead, const FVector& TargetPos, const FQuat& TargetRot)
{
	if (!GrabbedComp) return;
	FSharedTarget& ST = SharedTargets.FindOrAdd(GrabbedComp);
	if (bAsLead)
	{
		ST.Pos = TargetPos;
		ST.Rot = TargetRot.IsNormalized() ? TargetRot : TargetRot.GetNormalized();
		ST.Lead = GetOwner();
	}
}

FSharedTarget UGrabForceComponent::GetSharedTarget() const
{
	if (!GrabbedComp) return {};
	if (const FSharedTarget* Found = SharedTargets.Find(GrabbedComp))
	{
		return *Found;
	}
	return {};
}

#pragma endregion
