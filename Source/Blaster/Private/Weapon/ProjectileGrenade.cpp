// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileGrenade.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>
#include "Character/BlasterCharacter.h"

AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	SpawnTrailSystem();
	StartDestroyTimerSafe();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);
}

void AProjectileGrenade::Destroyed()
{
	ExplodeDamage();

	Super::Destroyed();
}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ImpactResult.GetActor());

	if (IsValid(BlasterCharacter) && HasAuthority())
	{
		CallMulticastPlayHitEffect();
	}

	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}
