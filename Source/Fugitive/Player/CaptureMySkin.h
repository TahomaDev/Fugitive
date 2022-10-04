// MageHunter @2021 Bardak Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "CaptureMySkin.generated.h"

UCLASS()
class FUGITIVE_API ACaptureMySkin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACaptureMySkin();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Scene;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* Light;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* Capture2D;

};
