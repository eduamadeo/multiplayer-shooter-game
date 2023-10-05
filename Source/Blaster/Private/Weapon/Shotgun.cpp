// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Shotgun.h"
#include <Engine/SkeletalMeshSocket.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
#include "Character/BlasterCharacter.h"
#include <Kismet/KismetMathLibrary.h>
#include "Character/BlasterCharacter.h"
#include "PlayerController/BlasterPlayerController.h"
#include "BlasterComponents/LagCompensationComponent.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		// Maps hit character to number of times hit
		TMap<ABlasterCharacter*, uint32> HitMap;
		TMap<ABlasterCharacter*, uint32> HeadShotHitMap;
		for (const FVector_NetQuantize& HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			if (FireHit.bBlockingHit)
			{
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
				if (BlasterCharacter)
				{
					const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

					if (bHeadShot)
					{
						if (HeadShotHitMap.Contains(BlasterCharacter))
						{
							HeadShotHitMap[BlasterCharacter]++;
						}
						else
						{
							HeadShotHitMap.Emplace(BlasterCharacter, 1);
						}
					}
					else
					{
						if (HitMap.Contains(BlasterCharacter))
						{
							HitMap[BlasterCharacter]++;
						}
						else
						{
							HitMap.Emplace(BlasterCharacter, 1);
						}
					}

					if (ImpactParticles)
					{
						UGameplayStatics::SpawnEmitterAtLocation(
							GetWorld(),
							ImpactParticles,
							FireHit.ImpactPoint,
							FireHit.ImpactNormal.Rotation()
						);
					}
					if (HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
					}
				}
			}
		}

		TArray<ABlasterCharacter*> HitCharacters;
		TMap<ABlasterCharacter*, float> DamageMap;
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key))
				{
					DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				}
				else
				{
					DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);
				}

				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						DamagePair.Key,
						DamagePair.Value,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
			if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensationComponent() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensationComponent()->ShotgunServerScoreRequest(
					HitCharacters, 
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket == nullptr)
	{
		return;
	}

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;

		HitTargets.Add(FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()));
	}
}
