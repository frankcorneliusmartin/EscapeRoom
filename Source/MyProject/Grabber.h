// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Grabber.generated.h"

struct FLine {
	FVector Begin, End;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetupInputComponent();

	void FindPhysicsHandle();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
		float GrabDistance = 100;
	
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputHandle = nullptr;
	
	// grab what is in range of ray-cast
	void Grab();

	// rease current holding item
	void Release();
	
	// return hit for first physics body in reach
	FHitResult GetFirstPhysicsBodyInReach() const;

	// returns the ray along the player can grab things
	FLine GetGrabRay() const;
	
};
