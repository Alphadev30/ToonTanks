// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameModeBase.h"
#include "ToonTanks/Pawns/PawnnTurret.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/PlayerController/PlayerControllerBase.h"


void ATankGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    // Get references and game win/lose conditions. 

    // Call HandleGameStart() to initialise the start countdown, turret activation, pawn check etc. 
    HandleGameStart();
    
    if(PlayerControllerRef){

        PlayerControllerRef->SetPlayerEnabledState(false);
    }

}

void ATankGameModeBase::ActorDied(AActor *DeadActor)
{
    // Check what type of Actor died. If Turret, tally. If Player -> go to lose condition.
    UE_LOG(LogTemp, Warning, TEXT("A Pawn Died")); 

    if(DeadActor == PlayerTank)
    {
        PlayerTank->HandleDestruction();
        HandleGameOver(false);
    }
    else if(APawnnTurret* DestroyedTurret = Cast<APawnnTurret>(DeadActor))
    {
        DestroyedTurret->HandleDestruction();

        if(--TargetTurrets == 0)
        {
            HandleGameOver(true);
        }
    }


}

void ATankGameModeBase::HandleGameStart() 
{
    // Initialise the start countdown, turret activation, pawn check etc. 
    // Call Blueprint version GameStart();

    // Get Turret and Player Pawn details.
    TargetTurrets = GetTargetTurretCount();
    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));

    PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));

    // BlueprintImplementableEvent
    GameStart();
    
    // Setting the player Enabled input to true: 
    if (PlayerControllerRef)
    {
        PlayerControllerRef->SetPlayerEnabledState(false);

        FTimerHandle PlayerEnableHandle;
        FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef, &APlayerControllerBase::SetPlayerEnabledState, true);
        GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
    }


}

void ATankGameModeBase::HandleGameOver(bool PlayerWon) 
{
    // See if the player has destroyed all the turrets, show win result.
    // else if turret destroyed player, show lose result. 
    // Call blueprint version GameOver(bool).

    // BlueprintImplementableEvent
    GameOver(PlayerWon);
    
}

int32 ATankGameModeBase::GetTargetTurretCount() 
{
    TArray<AActor *> TurretActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawnnTurret::StaticClass(), TurretActors);
    return TurretActors.Num();
}

