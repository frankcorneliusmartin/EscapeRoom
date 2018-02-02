// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!PressurePlate)
		UE_LOG(LogTemp, Error, TEXT("%s is missing a pressureplate component!"), *(this->GetName()));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// poll the trigger voluem
	if (GetTotalMassOfActorsOnPlate() >= OpenWeight) { 
		OnOpen.Broadcast();
	} else {
		OnClose.Broadcast();
	}

}

float UOpenDoor::GetTotalMassOfActorsOnPlate() 
{
	float totalMass = 0.f;
	if (!PressurePlate) { return totalMass; }
	
	// get all overlapping actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// itterate through all actors to add masses
	for (const auto* Actor : OverlappingActors)
	{
		totalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	UE_LOG(LogTemp, Warning, TEXT("%f kg is on the pressureplate which triggers at %f"), totalMass, OpenWeight);
	
	return totalMass;
}