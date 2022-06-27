#include "Components/Health/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent() = default;

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetStats();

	TObjectPtr<AActor> MyOwner = GetOwner();

	if (IsValid(MyOwner))
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
		return;

	HCStats.CurrentHealth = FMath::Clamp(HCStats.CurrentHealth - Damage, 0.F, HCStats.MaxHealth);
}

void UHealthComponent::SetStats()
{
	HCStats.MaxHealth = 100.F;
	HCStats.CurrentHealth = HCStats.MaxHealth;

	HCStats.MaxArmor = 100;
	HCStats.CurrentArmor = HCStats.MaxArmor;
}


