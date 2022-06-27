#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Structs/PlayerStats/Str_PlayerStats.h>
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:
	void SetStats();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"))
	FPlayerStats HCStats;
};
