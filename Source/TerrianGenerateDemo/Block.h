// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class TERRIANGENERATEDEMO_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

	UPROPERTY(EditAnywhere, Category = "Setup")
	int32 BlockID = 0;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* meshComponent = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void Initialize(int32 ID);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
