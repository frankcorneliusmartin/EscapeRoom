// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
	FindPhysicsHandle();
}

// look for input component (which is only available at run-time)
void UGrabber::SetupInputComponent()
{
	InputHandle = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputHandle) {
		InputHandle->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputHandle->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Input Handle missing on %s"), *(GetOwner()->GetName()));
	}
}

// Look for attached Physics Handle
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
		UE_LOG(LogTemp, Error, TEXT("Physics Handle Missing on %s"), *(GetOwner()->GetName()));
}

// grabs actor that is in reach
void UGrabber::Grab() {
	
	/// Line trace and reach any actor with the physics body chanel set
	FHitResult InReachObject = GetFirstPhysicsBodyInReach();
	AActor* ActorHit = InReachObject.GetActor();
	UPrimitiveComponent* ComponentToGrab = InReachObject.GetComponent();

	/// if we hit something then attach the physics handle
	if (ActorHit) {
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab, 
			NAME_None, 
			ComponentToGrab->GetOwner()->GetActorLocation(), 
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}

	return;

}

// release currently grabbed item
void UGrabber::Release() {
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

// check if the player has hit something
FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	/// get ray where the player can grab things
	FLine GrabLine = GetGrabRay();

	/// ray cast out to reach distance
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GrabLine.Begin,
		GrabLine.End,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	return Hit;
}

// returns the ray along which the player can grab something
FLine UGrabber::GetGrabRay() const{
	
	/// get line between player and reachdistance in viewpoint direction
	FVector PlayerPosition;
	FRotator PlayerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerPosition,
		OUT PlayerRotation
	);
	FVector LineTraceEnd = PlayerPosition + this->GrabDistance * PlayerRotation.Vector();

	/// store begin and endpoint of the ray
	FLine Ray;
	Ray.Begin = PlayerPosition;
	Ray.End = LineTraceEnd;

	return Ray;
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }

	/// check if a component is grabbed
	if (PhysicsHandle->GrabbedComponent) {
		FLine GrabLine = GetGrabRay();
		PhysicsHandle->SetTargetLocation(GrabLine.End);
	}
}