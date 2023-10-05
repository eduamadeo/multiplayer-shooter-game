// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawnPoint.generated.h"

class AWeapon;

UCLASS()
class BLASTER_API AWeaponSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponSpawnPoint();

protected:
	virtual void BeginPlay() override;

	void SpawnWeapon();
	void SpawnWeaponTimerFinished();

	UFUNCTION()
	void StartSpawnWeaponTimer(AActor* PickedUpWeapon);

	UPROPERTY()
	AWeapon* SpawnedWeapon;

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	FTimerHandle SpawnWeaponTimer;

	UPROPERTY(EditAnywhere)
	float SpawnWeaponTime = 20.0f;

public:
	virtual void Tick(float DeltaTime) override;

};
