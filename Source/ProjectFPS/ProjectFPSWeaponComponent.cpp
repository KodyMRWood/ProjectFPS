// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectFPSWeaponComponent.h"
#include "ProjectFPSCharacter.h"
#include "ProjectFPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UProjectFPSWeaponComponent::UProjectFPSWeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UProjectFPSWeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (!_canFire)
	{
		return;
	}


	// Keep logic in case we want to switch it later / get animation in
	//if (_currentAmmoInClip <= 0)
	//{
	//	// Play empty sound and queue reload animation
	//
	//	Reload();
	//	return;
	//}

	_fireTimer = 0;

	

	// Try and fire a projectile
	
	//if (ProjectileClass != nullptr)
	//{
	//	UWorld* const World = GetWorld();
	//	if (World != nullptr)
	//	{
	//		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	//		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	//		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	//		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	//
	//		//Set Spawn Collision Handling Override
	//		FActorSpawnParameters ActorSpawnParams;
	//		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	//
	//		// Spawn the projectile at the muzzle
	//		World->SpawnActor<AProjectFPSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	//	}
	//}

	FHitResult rayHit;

	// TODO KWood add muzzle offset (but i dont think that is neccessary right now because if player is too close it wont hit
	FVector rayStart = GetOwner()->GetActorLocation();

	FVector rayEnd = rayStart + Character->GetActorForwardVector() * 10000.0f;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(GetOwner());
	collisionParams.AddIgnoredActor(Character);


	bool isHit = GetWorld()->LineTraceSingleByChannel(rayHit, rayStart, rayEnd, ECollisionChannel::ECC_Visibility, collisionParams);

	DrawDebugLine(GetWorld(), rayStart, rayEnd, FColor::Red, false, 0.5, 0, 1);

	if (isHit)
	{
		// TODO KWood add damage to hit actor
		AActor* hitActor = rayHit.GetActor();
		if (hitActor != nullptr)
		{
			FString name = hitActor->GetName();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, name);
			}
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	_currentAmmoInClip = FMath::Clamp(_currentAmmoInClip - 1 , 0, _clipSize);

}

void UProjectFPSWeaponComponent::Reload()
{
	// Play animation

	if(_currentAmmoInClip == _clipSize)
	{
		return;
	}


	// Can move this to a method that will be called from animation notify
	int amountToReload = _clipSize - _currentAmmoInClip;

	if (_currentTotalAmmo >= amountToReload)
	{
		_currentTotalAmmo -= amountToReload;
		_currentAmmoInClip += amountToReload;
	}
	else if (_currentTotalAmmo < amountToReload)
	{
		_currentAmmoInClip += _currentTotalAmmo;
		_currentTotalAmmo = 0;
	}
	else if (_currentTotalAmmo <= 0)
	{
		_currentTotalAmmo = 0;
	}

}

void UProjectFPSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	_currentTotalAmmo = _maxAmmo;

	_fireTimerMax =  _fireRate / 60.0f;
}

void UProjectFPSWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
	if (_fireTimer <= _fireTimerMax)
	{
		_canFire = false;
		_fireTimer += DeltaTime;
		
		FString time = FString::SanitizeFloat(DeltaTime);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(_fireTimer));
		}
	}
	else
	{
		_canFire = true;
	}
}

bool UProjectFPSWeaponComponent::AttachWeapon(AProjectFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UProjectFPSWeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UProjectFPSWeaponComponent::Fire);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UProjectFPSWeaponComponent::Reload);
		}
	}

	return true;
}

void UProjectFPSWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}