// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Grabber.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	//Setting the PhysicsHandle and InputComponent Respectively
	FindPhysicsHandle();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//For moving the grabbed object changing its location every frame
	if(PhysicsHandle->GrabbedComponent)
	{
		//GetPlayerReach() returns the distance between us and the object grabbed
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}

//Gets the Reach of the player i.e. till what distance player can grab some actor
FVector UGrabber::GetPlayerReach()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	//Maps the location and rotation of the Actor respectively
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	//Location(Vector) tells the postion & Rotation(Converted to Vector) tells the angles of eyes of player
	//Rotation(Converted to Vector) is a unit vector so multiplied by reach we want player to have
	//Together they form Line coming of center of player of length Reach and direction is where player is looking
	return PlayerViewPointLocation+PlayerViewPointRotation.Vector()*Reach;
}

//Gets the Location of Player
FVector UGrabber::GetPlayerLocation()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
	return PlayerViewPointLocation;
}

//Sets PhysicsHandle
void UGrabber::FindPhysicsHandle()
{
	//FindComponentByClass() returns the component of type UPhysicsHandleComponent
	PhysicsHandle=GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no physics handle"), *GetOwner()->GetName());
	}	
}

//Sets InputComponent
void UGrabber::SetupInputComponent()
{
	//FindComponentByClass() returns the component of type UInputComponent
	InputComponent=GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		//Binds the function with (event, type(pressed or released),Objects(that will make use of the event) , function)
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

//Fires when "Grab" event is fired and key is pressed
//Grabs the Actors
void UGrabber::Grab()
{
	//Getting the object hit by an imaginary line
	//originates from center and extends to certain length
	FHitResult HitObject= GetPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab= HitObject.GetComponent();

	//If an Actor is hit, grab the actor
	if(HitObject.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, GetPlayerReach());
	}

}

//Fires when "Grab" event is fired and key is released
//Releases the Actors
void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();
}

//Gets the Actor(in which physics is appliead) in reach or hit by the line
FHitResult UGrabber::GetPhysicsBodyInReach()
{	
	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	//It makes the imaginary line and make interaction possible
	GetWorld()->LineTraceSingleByObjectType(
		Hit,//Object which is hit will be mapped here
		GetPlayerLocation(),//where line starts
		GetPlayerReach(),//where line ends
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),//Type of Actors Line can interact with (here physics)
		TraceParams//Actors to ignore when line hits it (here ourselves because line generates inside of us)
	);

	return Hit;

	/*
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0,255,0),
		0.f,
		0,
		5.f
	);
	*/
}