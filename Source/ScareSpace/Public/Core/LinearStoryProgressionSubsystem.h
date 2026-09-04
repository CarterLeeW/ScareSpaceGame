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
    FeedCat,
    BringInMail,
    GoToBed,
    GetWater,
    WatchTV,
    CheckGenerator,
    FindGeneratorFuel,
    StartGenerator,
    AnswerPhone1,
    FindCabinetKey,
    RetrieveGrimoire,
    AnswerPhone2,
    UnlockSubBasementDoor,
    FleeMonster,
    ExplorePrison,

    // Required for bounds checking during save/load
    MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryPhaseAdvanced, EStoryPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJournalItemAddRequest, FDataTableRowHandle, RowHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJournalItemRemoveRequest, FDataTableRowHandle, RowHandle);

UCLASS()
class SCARESPACE_API ULinearStoryProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintAssignable, Category = "Journal")
    FOnJournalItemAddRequest OnJournalItemAddRequested;

    UFUNCTION(BlueprintCallable, Category = "Journal")
    void AddJournalEntry(FDataTableRowHandle RowHandle);

    UPROPERTY(BlueprintAssignable, Category = "Journal")
    FOnJournalItemRemoveRequest OnJournalItemRemoveRequested;

    UFUNCTION(BlueprintCallable, Category = "Journal")
    void RemoveJournalEntry(FDataTableRowHandle RowHandle);

    UPROPERTY(BlueprintAssignable, Category = "Progression")
    FOnStoryPhaseAdvanced OnStoryPhaseAdvanced;

    UFUNCTION(BlueprintPure, Category = "Progression")
    EStoryPhase GetCurrentPhase() const;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AdvanceToPhase(EStoryPhase NextPhase);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void SetPhaseForce(EStoryPhase NextPhase);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool ValidateAndLoadPhase(uint8 LoadedStateValue);

private:
    EStoryPhase CurrentPhase;
};
