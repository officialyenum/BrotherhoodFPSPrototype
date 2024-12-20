﻿// Copyright Chukwuyenum Opone @officialyenum 2024

#pragma once

#include "CoreMinimal.h"
#include "BH_CharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "BH_Friendly.generated.h"

UCLASS()
class BROTHERHOODFPS_API ABH_Friendly : public ABH_CharacterBase, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABH_Friendly();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void ApplyDamageToEnemy(AActor* Actor) override;
	UFUNCTION()
	virtual void TakeHitDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
							AController* InstigatedBy, AActor* DamageCauser) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
