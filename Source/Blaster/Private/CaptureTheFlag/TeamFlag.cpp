// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlag/TeamFlag.h"
#include <Components/SphereComponent.h>
#include "Character/BlasterCharacter.h"
#include "BlasterComponents/CombatComponent.h"
#include <Net/UnrealNetwork.h>
#include <Components/WidgetComponent.h>

ATeamFlag::ATeamFlag()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	SetRootComponent(FlagMesh);
	FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMesh->SetSimulatePhysics(false);
	FlagMesh->SetEnableGravity(false);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(FlagMesh);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->SetSphereRadius(150.0f);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void ATeamFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamFlag, FlagState);
}

void ATeamFlag::BeginPlay()
{
	Super::BeginPlay();

	InitialTransform = GetActorTransform();
	SetFlagState(EFlagState::EFS_Initial);

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(true);
	}

	if (HasAuthority())
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ATeamFlag::OnSphereOverlap);
	}
}

void ATeamFlag::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter && !BlasterCharacter->IsElimned())
	{
		if (BlasterCharacter->GetTeam() == Team && FlagState == EFlagState::EFS_Dropped)
		{
			ResetFlag();
		}
		else if (BlasterCharacter->GetTeam() != Team && FlagState != EFlagState::EFS_Equipped)
		{
			UCombatComponent* Combat = BlasterCharacter->GetCombatComponent();
			if (Combat)
			{
				Combat->PickupFlag(this);
			}
		}
	}
}

void ATeamFlag::OnFlagStateSet()
{
	switch (FlagState)
	{
	case EFlagState::EFS_Initial:
		OnInitOrReset();
		break;
	case EFlagState::EFS_Equipped:
		OnEquipped();
		break;
	case EFlagState::EFS_Dropped:
		OnDropped();
		break;
	case EFlagState::EFS_MAX:
		break;
	default:
		break;
	}
}

void ATeamFlag::OnEquipped()
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATeamFlag::OnDropped()
{
	SetActorRotation(InitialTransform.GetRotation());
	FVector FlagLocation = GetActorLocation();
	FlagLocation.Z -= 40.f;
	SetActorLocation(FlagLocation);

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
}

void ATeamFlag::OnInitOrReset()
{
	SetActorTransform(InitialTransform);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
}

void ATeamFlag::OnRep_FlagState()
{
	OnFlagStateSet();
}

void ATeamFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeamFlag::SetFlagState(EFlagState State)
{
	FlagState = State;

	OnFlagStateSet();
}

void ATeamFlag::Drop()
{
	SetFlagState(EFlagState::EFS_Dropped);
}

void ATeamFlag::ResetFlag()
{
	SetFlagState(EFlagState::EFS_Initial);
}

bool ATeamFlag::IsEquipped()
{
	return FlagState == EFlagState::EFS_Equipped;
}

