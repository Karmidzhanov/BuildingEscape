#pragma once

#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "Components/ActorComponent.h"
#include "Grabber.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties	
	UGrabber();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	
	//how far ahead of the player can we reach in cm
	const float Reach = 100.f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	//Ray-cast and grab what's in reach
	void Grab();

	//Call when grab is relaesd
	void Release();

	//find attached physiscs handle
	void FindPhysicsHandleComponent();

	//set up (assumed) attached input component
	void SetupInputComponent();

	//Return hit for first physiscs body in reach

	const FHitResult GetFirstPhysicsBodyInreach();

	FVector GetLineTraceStart();
	FVector GetLineTraceEnd();
	
};
