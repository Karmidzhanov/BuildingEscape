#include "BuildingEscape.h"
#include "Grabber.h"

#define OUT   //empty macro to flag the out parameters of GetPlayerViewPoint

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
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

//Look for attached Physics Handle
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)	UE_LOG(LogTemp, Error, TEXT("Physics handle is missing on %s !"), *GetOwner()->GetName());	
}

//Look for attached Input Component (only appears at run time)
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s is missing a Input Component!"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{	
	//LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInreach();
	auto ComponentToGrab = HitResult.GetComponent();  //gets the mesh component in our case
	auto ActorHit = HitResult.GetActor();

	//If we hit something then attach a physiscs handle
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,  //no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true   //allow rotation
		);
	}
}

//release physics handle
void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}
//Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!PhysicsHandle) { return; }
	// if the physiscs handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		//move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}
//Returns current end of reach line, in this case the playerviewpoint
FVector UGrabber::GetLineTraceStart()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation;
}

//Returns current end of reach line
FVector UGrabber::GetLineTraceEnd()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);	
	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

const FHitResult UGrabber::GetFirstPhysicsBodyInreach()
{	
	///Set up query paramaters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());	

	///Line-trace (AKA Ray-cast) out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}