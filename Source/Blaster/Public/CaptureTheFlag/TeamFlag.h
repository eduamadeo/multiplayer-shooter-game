// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blaster/BlasterTypes/Team.h"
#include "TeamFlag.generated.h"

UENUM(BlueprintType)
enum class EFlagState : uint8
{
	EFS_Initial UMETA(DisplayName = "Initial State"),
	EFS_Equipped UMETA(DisplayName = "Equipped"),
	EFS_Dropped UMETA(DisplayName = "Dropped"),

	EFS_MAX UMETA(DisplayName = "DefaultMAX"),
};

class USphereComponent;

UCLASS()
class BLASTER_API ATeamFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	ATeamFlag();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	void SetFlagState(EFlagState State);
	void Drop();
	void ResetFlag();
	bool IsEquipped();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnFlagStateSet();
	void OnEquipped();
	void OnDropped();
	void OnInitOrReset();

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FlagMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* AreaSphere;

	FTransform InitialTransform;

	UPROPERTY(ReplicatedUsing = OnRep_FlagState, VisibleAnywhere)
	EFlagState FlagState;

	UPROPERTY(EditAnywhere)
	ETeam Team;

	UFUNCTION()
	void OnRep_FlagState();

public:

	FORCEINLINE FTransform GetInitialTransform() const { return InitialTransform; }
	FORCEINLINE ETeam GetTeam() const { return Team; }

};
