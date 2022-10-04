// MageHunter @2021 Bardak Studio. All Rights Reserved


#include "CaptureMySkin.h"


#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACaptureMySkin::ACaptureMySkin()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	SpringArm->TargetArmLength = 250.f;
	SpringArm->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(RootComponent);

	Capture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture2D"));
	
	Capture2D->SetupAttachment(SpringArm);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetWorldLocation(FVector(-5.f, 95.f, 130.f));
	Light->SetupAttachment(RootComponent);
}



