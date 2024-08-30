﻿// Copyright Chukwuyenum Opone @officialyenum 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BH_Drone.generated.h"

UCLASS()
class BROTHERHOODFPS_API ABH_Drone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABH_Drone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
