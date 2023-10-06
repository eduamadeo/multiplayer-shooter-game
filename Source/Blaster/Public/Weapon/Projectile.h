// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UParticleSystem;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	
	void CallMulticastPlayHitEffect();

	/*
	* Used with server-side rewind
	*/

	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000.f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 80.f;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
#endif

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void StartDestroyTimerSafe();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void TornOff() override;
	void PlayImpactFXs(bool bHasHitCharacter);

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactCharacterParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailSystemComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY()
	class UParticleSystemComponent* TracerComp;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayHitEffect(bool bHasHitCharacter);

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

public:
	

};
