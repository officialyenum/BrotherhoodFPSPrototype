﻿// Copyright Chukwuyenum Opone @officialyenum 2024


#include "Character/BH_CharacterBase.h"

#include "Character/Animation/BH_AnimInstanceBase.h"
#include "Components/CapsuleComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABH_CharacterBase::ABH_CharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);
	
	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>("GunComponent");
	GunComponent->SetupAttachment(RootComponent);
	GunComponent->SetGenerateOverlapEvents(false);
	GunComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	BurstPoint = CreateDefaultSubobject<USceneComponent>("BurstPoint");
	BurstPoint->SetupAttachment(GunComponent);
}

// Called when the game starts or when spawned
void ABH_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	SetUpWeapon();
	SetUpAnimBp();
}

// Called every frame
void ABH_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABH_CharacterBase::SetUpAnimBp()
{
	AnimBP = Cast<UBH_AnimInstanceBase>(GetMesh()->GetAnimInstance());
}

void ABH_CharacterBase::PlayFireMontage()
{
	AnimBP->IsAiming ? AnimBP->Montage_Play(AimFireMontage): AnimBP->Montage_Play(AimMontage);
}

void ABH_CharacterBase::ResetCanShoot()
{
	CanShoot = true;
}

void ABH_CharacterBase::PlaySoundAndBurstEmitterFX()
{
	FVector Loc = BurstPoint->GetComponentLocation();
	FRotator Rot = BurstPoint->GetComponentRotation();
	FVector Scale = FVector(0.25);
	UGameplayStatics::SpawnEmitterAtLocation(this, ShotBurstFX, Loc, Rot, Scale, true);
	UGameplayStatics::PlaySoundAtLocation(this, ShotSound, Loc, Rot, 0.5f);
}

void ABH_CharacterBase::FireWeapon()
{
	if (!CanShoot || Ammo <= 0)
	{
		return;
	}
	CanShoot = false;
	PlaySoundAndBurstEmitterFX();
	PlayFireMontage();
	PerformLineTrace();
	Ammo--;
	UpdatePlayerParamStats(1, 0, 0);
	// delay for 2 seconds
	// Create a timer handle
	FTimerHandle TimerHandle;

	// Bind the ResetCanShoot function to be called after 2 seconds
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABH_CharacterBase::ResetCanShoot, ShootRate, false);
}

void ABH_CharacterBase::SetUpWeapon()
{
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);
	GunComponent->AttachToComponent(GetMesh(), Rules, FName("GripPoint"));
	GunComponent->SetVisibility(true);
}

void ABH_CharacterBase::PerformLineTrace()
{
	AActor* Camera = UGameplayStatics::GetPlayerCameraManager(this,0);
	if (!Camera)
	{
		return; // Ensure Camera exists
	}
	// Get the camera location and forward vector
	FVector Loc = Camera->GetActorLocation();
	FVector FV = Camera->GetActorForwardVector() * 50000;
	FVector LOCFV;
	if (!AnimBP->IsAiming)
	{
		// Apply random inaccuracy to the forward vector
		float RandomXOffset = FMath::RandRange(-500.0f, 500.0f); // Random X offset
		float RandomZOffset = FMath::RandRange(-500.0f, 500.0f); // Random Z offset

		FVector InaccurateForwardVector = FV;
		InaccurateForwardVector.X += RandomXOffset;
		InaccurateForwardVector.Z += RandomZOffset;

		LOCFV = Loc + InaccurateForwardVector;
	}else
	{
		LOCFV = Loc + FV;
	}
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Loc, LOCFV, ECC_Visibility);
	// Spawn visual effects on impact
	if (HitResult.bBlockingHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, HitResult.ImpactPoint, FRotator::ZeroRotator, FVector(1.0f));
        
		// Apply damage if an enemy is hit
		if (HitResult.GetActor())
		{
			ApplyDamageToEnemy(HitResult.GetActor());
		}
	}
}

void ABH_CharacterBase::ReloadWeapon()
{
	CanShoot = false;
	Reload();
	AnimBP->Montage_Play(ReloadMontage);
	
	// Create a timer handle
	FTimerHandle TimerHandle;
	// Bind the ResetCanShoot function to be called after 2 seconds
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABH_CharacterBase::ResetCanShoot, ReloadTime, false);
}

void ABH_CharacterBase::ApplyDamageToEnemy(AActor* Actor)
{
}

void ABH_CharacterBase::TakeHitDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
}

void ABH_CharacterBase::AddCartridge(int32 CartridgeAmount)
{
	Cartridge = FMath::Clamp(Cartridge + CartridgeAmount, 0, MaxCartridge);
}

void ABH_CharacterBase::AddHealth(float NewHealth)
{
	Health += FMath::Clamp(NewHealth, 0, MaxHealth);
}

void ABH_CharacterBase::Reload()
{
	if (Cartridge > 0)
	{
		Cartridge--;
		UpdatePlayerParamStats(0, 1, 0);

		Ammo = FMath::Clamp(MaxAmmo, 0, MaxAmmo);
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation(), GetActorRotation(), 0.5f);
	}
}

