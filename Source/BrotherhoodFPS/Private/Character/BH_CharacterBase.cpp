﻿// Copyright Chukwuyenum Opone @officialyenum 2024


#include "Character/BH_CharacterBase.h"

#include "Character/Animation/BH_AnimInstanceBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABH_CharacterBase::ABH_CharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

float ABH_CharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float NewHealth = Health - DamageAmount;
	Health = FMath::Clamp(NewHealth, 0, MaxHealth);
	return DamageAmount;
}

// Called when the game starts or when spawned
void ABH_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void ABH_CharacterBase::PerformLineTrace()
{
	AActor* Camera = UGameplayStatics::GetPlayerCameraManager(this,0);
	FVector Loc = Camera->GetActorLocation();
	FVector FV = Camera->GetActorForwardVector() * 5000;
	FVector LOCFV = Loc + FV;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Loc, LOCFV, ECC_Visibility);
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, HitResult.ImpactPoint,FRotator(0.F),FVector(1));
	if (ICombatInterface* Actor = Cast<ICombatInterface>(HitResult.GetActor()))
	{
		ApplyDamageToEnemy(HitResult.GetActor());
	}
	Ammo--;
}

void ABH_CharacterBase::ApplyDamageToEnemy(AActor* Actor)
{
}

void ABH_CharacterBase::AddAmmo(int32 NewAmmo)
{
	Ammo += NewAmmo;
}

void ABH_CharacterBase::AddHealth(float NewHealth)
{
	Health += FMath::Clamp(NewHealth, 0, MaxHealth);
}
