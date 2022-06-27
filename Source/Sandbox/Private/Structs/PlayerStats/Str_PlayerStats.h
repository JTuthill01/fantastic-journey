#pragma once

#include "Str_PlayerStats.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStats
{
    GENERATED_BODY()

    public:
        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float CurrentHealth;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float MaxHealth;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 CurrentArmor;

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 MaxArmor;

};
