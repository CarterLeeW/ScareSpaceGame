// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearStoryProgressionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EStoryPhase : uint8
{
    ArriveAtHouse = 0,
    ReadJournal,
    FindSpareKey,
    EnterHouse,
    StartBasementFire,
    SeeFigureOnStairs,
    TagItems,
    SnowedIn,
    GoToBed,
    GetWater,
    WatchTV,
    PowerOutage,
    FindFlashlight,
    CheckGenerator,
    FindGeneratorFuel,
    AnswerPhone,
    FindCabinetKey,
    RetrieveGrimoire,
    FleeToSubBasement,
    PerformRitual,

    // Required for bounds checking during save/load
    MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryPhaseAdvanced, EStoryPhase, NewPhase);

UCLASS()
class SCARESPACE_API ULinearStoryProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintAssignable, Category = "Progression")
    FOnStoryPhaseAdvanced OnStoryPhaseAdvanced;

    UFUNCTION(BlueprintPure, Category = "Progression")
    EStoryPhase GetCurrentPhase() const;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AdvanceToPhase(EStoryPhase NextPhase);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool ValidateAndLoadPhase(uint8 LoadedStateValue);

private:
    EStoryPhase CurrentPhase;
};
