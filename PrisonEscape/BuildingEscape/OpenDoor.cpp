// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"

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

	//initializing the Yaws 
	InitialYaw=GetOwner()->GetActorRotation().Yaw;
	CurrentYaw=InitialYaw;
	TargetYaw+=InitialYaw;	

	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Actor has OpenDoor component, but the PressurePlate is not defined"), *GetOwner()->GetName())
	}

	ActorThatOpens =GetWorld()->GetFirstPlayerController()->GetPawn();//Assigning the default pawn 

	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Checking every frame that conditions are met or not
	//Codition-If the mass of Actor(s) is greater than 40 or not
	if(TotalMassOfOverlappingActors()>40.f)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened=GetWorld()->GetTimeSeconds();
	}
	else
	{
		//Closes the door after some time
		if(GetWorld()->GetTimeSeconds()-DoorLastOpened> DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
		
	}
}

//Opens the Door
void UOpenDoor::OpenDoor(float DeltaTime)
{
	//Linear Interpolation i.e. takes the current yaw to target yaw
	CurrentYaw=FMath::Lerp(CurrentYaw, TargetYaw, DeltaTime*0.8f);
	FRotator DoorRotation=GetOwner()->GetActorRotation();//Get all Rotation of door
	DoorRotation.Yaw=CurrentYaw;//yaw is changed to Current Yaw
	GetOwner()->SetActorRotation(DoorRotation);//Set the new door rotation

	//Plays the Audio
	if(!AudioComponent){return;}
	if(!IsOpenDoor)
	{
		AudioComponent->Play();
	}
	
}

//Closes the door
void UOpenDoor::CloseDoor(float DeltaTime)
{
	//Linear Interpolation i.e. takes the current yaw back to initial yaw
	CurrentYaw=FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime*2.f);
	FRotator DoorRotation=GetOwner()->GetActorRotation();
	DoorRotation.Yaw=CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

//Gets the total mass on pressure plate
float UOpenDoor::TotalMassOfOverlappingActors()
{
	float TotalMass=0.f;
	TArray<AActor*> OverlappingActors;

	//getting the array of actors present on pressure plate
	PressurePlate->GetOverlappingActors(OverlappingActors);

	//Calculating the total mass of each actor 
	for(AActor* Actor:OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}

//Gets the Audio Component 
void UOpenDoor::FindAudioComponent()
{
	AudioComponent=GetOwner()->FindComponentByClass<UAudioComponent>();
}