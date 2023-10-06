// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlag/FlagZone.h"
#include <Components/SphereComponent.h>
#include "Weapon/Flag.h"
#include "GameMode/CaptureTheFlagGameMode.h"
#include "CaptureTheFlag/TeamFlag.h"
#include "Character/BlasterCharacter.h"
#include "BlasterComponents/CombatComponent.h"

AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	SetRootComponent(ZoneSphere);
}

void AFlagZone::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnSphereOverlap);
	}
}

void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFlag* OverlappingFlag = Cast<AFlag>(OtherActor);
	if (OverlappingFlag && OverlappingFlag->GetTeam() != Team)
	{
		ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
		if (GameMode)
		{
			GameMode->FlagCaptured(OverlappingFlag, this);
		}
		OverlappingFlag->ResetFlag();
	}

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (!IsValid(BlasterCharacter) || !IsValid(BlasterCharacter->GetCombatComponent()))
	{
		return;
	}

	ATeamFlag* OverlappingTeamFlag = BlasterCharacter->GetCombatComponent()->GetTeamFlag();
	if (OverlappingTeamFlag && OverlappingTeamFlag->IsEquipped() && OverlappingTeamFlag->GetTeam() != Team)
	{
		ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
		if (GameMode)
		{
			GameMode->FlagCaptured(OverlappingTeamFlag, this);
		}
		BlasterCharacter->GetCombatComponent()->SetTeamFlag(nullptr);
		OverlappingTeamFlag->ResetFlag();
	}
}

