// Copyright Francisco Santos, 2020.
#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

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
	
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw += InitialYaw;

	FindPressurePlate();
	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent() {
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is Missing AudioComponent!"), *GetOwner()->GetName());
	}	
}
void UOpenDoor::FindPressurePlate() {
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressure plate set"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() >= MassToOpenDoor)
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

	CloseDoorSound = false;
	if (!AudioComponent) { return; }	
	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
	
}

void UOpenDoor::CloseDoor(float DeltaTime) {
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DoorCloseSpeed * DeltaTime);
	CurrentRotation.Yaw = CurrentYaw;	

	GetOwner()->SetActorRotation(CurrentRotation);

	OpenDoorSound = false;
	if (!AudioComponent) { return; }	
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const {
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;

	if(!PressurePlate){return TotalMass;}
	
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}	

	return TotalMass;
}