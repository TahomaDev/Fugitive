// MageHunter @2021 Bardak Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToCPP.generated.h"

/**
 * 
 */
UCLASS()
class FUGITIVE_API UBTTask_MoveToCPP : public UBTTask_MoveTo
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void ChangeMoveState(APawn* ControlledPawn, bool Value);

	UBehaviorTreeComponent* Comp;
	uint8* MyNodeMemory;
};
