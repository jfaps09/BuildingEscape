// Copyright Francisco Santos, 2020.
#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"



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
	
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressure plate set"), *GetOwner()->GetName());
	}

	//ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = 0.f;
	}
	else if (DoorLastOpened >= DoorCloseDelay)
	{
		CloseDoor(DeltaTime);
	}
		
	DoorLastOpened += DeltaTime;
}

void UOpenDoor::OpenDoor(float DeltaTime) {
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	CurrentYaw = FMath::Lerp(CurrentYaw, TargetYaw, DoorOpenSpeed * DeltaTime);
	CurrentRotation.Yaw = CurrentYaw;	

	GetOwner()->SetActorRotation(CurrentRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime) {
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DoorCloseSpeed * DeltaTime);
	CurrentRotation.Yaw = CurrentYaw;	

	GetOwner()->SetActorRotation(CurrentRotation);
}

