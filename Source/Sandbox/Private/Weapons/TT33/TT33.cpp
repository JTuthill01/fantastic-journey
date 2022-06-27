#include "Weapons/TT33/TT33.h"
#include "NiagaraFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>

ATT33::ATT33() = default;

void ATT33::WeaponFire()
{
	Super::WeaponFire();

	EjectTransform = WeaponMesh->GetSocketTransform("AmmoEject");

	EjectTransform.GetRotation() = EjectQuat;

	FireTransform = WeaponMesh->GetSocketTransform("Fire_FX_Slot");

	FireTransform.GetRotation() = FireQuat;

	if (WeaponAnimInstance)
	{
		UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AmmoEject, EjectTransform.GetTranslation(), EjectQuat.Rotator());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireFX, FireTransform.GetTranslation(), FireQuat.Rotator());

		WeaponFireTimer = WeaponAnimInstance->Montage_Play(WeaponFireMontage);

		GetWorldTimerManager().SetTimer(WeaponFireTimerHandle, this, &ATT33::ResetCanFire, WeaponFireTimer, false);
	}
}

void ATT33::WeaponReload()
{
}

void ATT33::StopFire()
{
}

void ATT33::ResetCanFire()
{
	bCanFire = true;
	bCanReload = true;

	GetWorldTimerManager().ClearTimer(WeaponFireTimerHandle);
}
