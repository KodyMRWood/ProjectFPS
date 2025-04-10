// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProjectFPSWeaponComponent.generated.h"

class AProjectFPSCharacter;
class UInputAction;
class UInputMappingContext;


/// ---- Enums ---- ///
// Measured in shots per second
UENUM(BlueprintType)
enum class EFireType : uint8
{	
	SINGLESHOT = 1,
	BURSTFIRE = 2,
	AUTOMATIC = 3,
	NONE = 0
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )


class PROJECTFPS_API UProjectFPSWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	/// ---- Classes ---- ///
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	/// ---- Variables ---- ///
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AProjectFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;
	
	/// ---- Methods ---- ///
	/** Sets default values for this component's properties */
	UProjectFPSWeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AProjectFPSCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/// ---- Variables ---- ///

	// Ammo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	int _maxAmmo = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	int _currentTotalAmmo = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	int _clipSize = 30;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	int _currentAmmoInClip = 30;


	// Fire rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	// Measured in shots per second
	float _fireRate = 11;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	float _fireTimer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	float _fireTimerMax = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	bool _canFire = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GunVariables, meta = (AllowPrivateAccess = "true"))
	EFireType _fireType = EFireType::AUTOMATIC;


	/// ---- Methods ---- ///
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/// ---- Variables ---- ///
	/** The Character holding this weapon*/
	AProjectFPSCharacter* Character;
};
