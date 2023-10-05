// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponSpawnPoint.h"
#include "Weapon/Weapon.h"

AWeaponSpawnPoint::AWeaponSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void AWeaponSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		SpawnWeapon();
	}
}

void AWeaponSpawnPoint::SpawnWeapon()
{
	if (WeaponClass)
	{
		SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorTransform());

		if (HasAuthority() && SpawnedWeapon)
		{
			SpawnedWeapon->PickedUpDelegate.AddDynamic(this, &AWeaponSpawnPoint::StartSpawnWeaponTimer);
		}
	}
}

void AWeaponSpawnPoint::SpawnWeaponTimerFinished()
{
	if (HasAuthority())
	{
		SpawnWeapon();
	}
}

void AWeaponSpawnPoint::StartSpawnWeaponTimer(AActor* PickedUpWeapon)
{
	GetWorldTimerManager().SetTimer(SpawnWeaponTimer, this, &AWeaponSpawnPoint::SpawnWeaponTimerFinished, SpawnWeaponTime);
}

void AWeaponSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

