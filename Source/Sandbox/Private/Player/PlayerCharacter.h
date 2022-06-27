#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include <Interfaces/Player/PlayerInterface.h>
#include <Structs/PlayerStats/Str_PlayerStats.h>
#include <Enums/WeaponEnums/WeaponEnums.h>
#include "PlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class APlayerCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	FORCEINLINE class UCameraComponent* GetPlayerCamera() { return Camera; }
	FORCEINLINE class AWeaponBase* GetCurrentWeapon() { return CurrentWeapon; }
	FORCEINLINE USkeletalMeshComponent* GetPlayerArms() { return Arms; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PawnClientRestart() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual APlayerCharacter* GetPlayerRef_Implementation() override;

protected:

#pragma region INPUT
	//Input Actions//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* WeaponFireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* WeaponReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UInputAction* EquipAction;

	//Mapping Contexts//
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = InputMappings)
	UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = InputMappings)
	int32 BaseMappingPriority = 0;

	//Input Functions//
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void CrouchPressed();
	void FirePressed();
	void FireReleased();
	void Interact();
	void Reload();

#pragma endregion

private:
	void InteractableScanner();

	void SpawnWeapon();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SK_Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Arms;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AWeaponBase> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> WeaponToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montages, meta = (AllowPrivateAccess = "true"))
	TArray< TObjectPtr<class UAnimMontage> > WeaponFireMontage;

private:
	UPROPERTY()
	TObjectPtr<UAnimInstance> PlayerAnimInstance;

private:
	float ScanTimer;

	int32 CurrentWeaponIndex;

	EHasWeapon HasWeapon;

	FTimerHandle ScanTimerHandle;
};
