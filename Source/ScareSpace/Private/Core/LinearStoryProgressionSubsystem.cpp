// Copyright Carter Wooton


#include "Core/LinearStoryProgressionSubsystem.h"

void ULinearStoryProgressionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CurrentPhase = EStoryPhase::ArriveAtHouse;
}

EStoryPhase ULinearStoryProgressionSubsystem::GetCurrentPhase() const
{
    return CurrentPhase;
}

void ULinearStoryProgressionSubsystem::AdvanceToPhase(EStoryPhase NextPhase)
{
    // Prevent backward progression or redundant calls in a linear sequence
    if (NextPhase > CurrentPhase)
    {
        CurrentPhase = NextPhase;
        OnStoryPhaseAdvanced.Broadcast(CurrentPhase);
    }
}

bool ULinearStoryProgressionSubsystem::ValidateAndLoadPhase(uint8 LoadedStateValue)
{
    // Ensure the loaded value does not exceed the maximum valid enum definition
    if (LoadedStateValue < static_cast<uint8>(EStoryPhase::MAX))
    {
        CurrentPhase = static_cast<EStoryPhase>(LoadedStateValue);
        OnStoryPhaseAdvanced.Broadcast(CurrentPhase);
        return true;
    }

    // Fallback if save data is corrupt or manipulated
    CurrentPhase = EStoryPhase::ArriveAtHouse;
    return false;
}