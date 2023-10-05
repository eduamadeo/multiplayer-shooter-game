// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/TeamsGameMode.h"
#include "CaptureTheFlag/TeamFlag.h"
#include "CaptureTheFlagGameMode.generated.h"

class AFlag;
class AFlagZone;

UCLASS()
class BLASTER_API ACaptureTheFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()

public:

	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) override;
	void FlagCaptured(AFlag* Flag, AFlagZone* Zone);
	void FlagCaptured(ATeamFlag* Flag, AFlagZone* Zone);
	
};
