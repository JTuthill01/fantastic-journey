#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Interfaces/Interact/InteractInterface.h>
#include <Kismet/GameplayStatics.h>
#include <Weapons/WeaponBase/WeaponBase.h>

// Sets default values
APlayerCharacter::APlayerCharacter() : ScanTimer(0.25F), CurrentWeaponIndex(0), HasWeapon(EHasWeapon::EHW_NoWeapon)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetRootComponent());
	Camera->bUsePawnControlRotation = true;

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(Camera);
	Arms->SetCastShadow(false);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ScanTimerHandle, this, &APlayerCharacter::InteractableScanner, ScanTimer, true);

	SpawnWeapon();

	PlayerAnimInstance = Arms->GetAnimInstance();
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		/*if (WeaponFireAction)
		{
			PlayerEnhancedInputComponent->BindAction(WeaponFireAction, ETriggerEvent::Started, this, &APlayerCharacter::FirePressed);
			PlayerEnhancedInputComponent->BindAction(WeaponFireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireReleased);
		}*/

		if (CrouchAction)
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchPressed);

		if (MovementAction)
			PlayerEnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		if (LookAction)
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		if (InteractAction)
			PlayerEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

		if (WeaponFireAction)
			PlayerEnhancedInputComponent->BindAction(WeaponFireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FirePressed);

		if (WeaponReloadAction)
			PlayerEnhancedInputComponent->BindAction(WeaponReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::Reload);
	}
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();
			// Add each mapping context, along with their priority values. Higher values out prioritize lower values.
			Subsystem->AddMappingContext(BaseMappingContext, BaseMappingPriority);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.F)
	{
		AddMovementInput(GetActorForwardVector(), Value[1]);
		AddMovementInput(GetActorRightVector(), Value[0]);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value[1] * -1);
	AddControllerYawInput(Value[0]);
}

void APlayerCharacter::CrouchPressed()
{
}

void APlayerCharacter::FirePressed()
{
	if (IsValid(CurrentWeapon))
	{
		switch (HasWeapon)
		{
		case EHasWeapon::EHW_NoWeapon:
			break;

		case EHasWeapon::EHW_HasWeapon:

			if (IsValid(CurrentWeapon) && IsValid(PlayerAnimInstance))
			{
				if (CurrentWeapon->MagHasAmmo())
				{
					if (CurrentWeapon->GetCanFire() && WeaponFireMontage.IsValidIndex(CurrentWeaponIndex))
					{
						CurrentWeapon->WeaponFire();

						PlayerAnimInstance->Montage_Play(WeaponFireMontage[CurrentWeaponIndex]);
					}
				}
			}

			break;

		default:
			break;
		}
	}
}

void APlayerCharacter::FireReleased()
{
}

void APlayerCharacter::Reload()
{
	if (IsValid(CurrentWeapon) && IsValid(PlayerAnimInstance))
	{
		if (CurrentWeapon->HasAmmoForReload())
		{
			switch (CurrentWeapon->GetFireType())
			{
			case EWeaponFireType::EWFT_None:
				break;

			case EWeaponFireType::EWFT_Hitscan:
				break;

			case EWeaponFireType::EWFT_Projectile:
				break;

			case EWeaponFireType::EWFT_SpreadScan:
				break;

			default:
				break;
			}
		}
	}
}

void APlayerCharacter::Interact()
{
	FHitResult HitResult;

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetComponentRotation().Vector() * 400.F);

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(CurrentWeapon);
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	const bool bCanInteract = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, TraceObjects, true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	if (bCanInteract)
	{
		if (HitResult.GetActor())
		{
			if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
				IInteractInterface::Execute_InteractWithObject(HitResult.GetActor());
		}
	}
}

void APlayerCharacter::InteractableScanner()
{
	FHitResult HitResult;

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetComponentRotation().Vector() * 400.F);

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(CurrentWeapon);
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	const bool bIsInteractable = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, TraceObjects, true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	if (bIsInteractable)
	{
		if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
			IInteractInterface::Execute_InteractableFound(HitResult.GetActor());
	}
}

void APlayerCharacter::SpawnWeapon()
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Location = Arms->GetComponentLocation();
	FRotator Rotation = Arms->GetComponentRotation();

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponToSpawn, Location, Rotation, Params);

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->AttachToComponent(Arms, FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->GetSocketName());

		HasWeapon = EHasWeapon::EHW_HasWeapon;

		CurrentWeapon->SetWeaponStats(CurrentWeapon->GetCurrentWeaponEnumName());

		CurrentWeaponIndex = CurrentWeapon->GetWeaponIndex();
	}
}

APlayerCharacter* APlayerCharacter::GetPlayerRef_Implementation() { return this; }
