// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearStoryProgressionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EStoryPhase : uint8
{
    // Major unskippable phases only. Sub-phases are handled by independent actors
    // Advancing to a new phase will likely cause a memo reset to represent the new objectives and clean old ones
    ArriveAtHouse = 0, // Between getting out of the car and entering the back door
    StartBasementFire, // player can find key to back door without trying to unlock it first
    TagItems,
    LeaveHouse, // Includes snowed-in sequence
    GoToBed,
    GetWater,
    WatchTV,
    PowerOutage,
    FindFlashlight, // optional?
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJournalEntryRequested, FDataTableRowHandle, RowHandle);

UCLASS()
class SCARESPACE_API ULinearStoryProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintAssignable, Category = "Journal")
    FOnJournalEntryRequested OnJournalEntryRequested;

    UFUNCTION(BlueprintCallable, Category = "Journal")
    void AddJournalEntry(FDataTableRowHandle RowHandle);

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
