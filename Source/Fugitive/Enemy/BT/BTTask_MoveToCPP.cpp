// MageHunter @2021 Bardak Studio. All Rights Reserved


#include "BTTask_MoveToCPP.h"

#include "Fugitive/Enemy/BaseEnemyCharacter.h"

EBTNodeResult::Type UBTTask_MoveToCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Comp = &OwnerComp;
	MyNodeMemory = NodeMemory;
	
	if (OwnerComp.GetOwner())
	{
		ABaseEnemyCharacter* EnemyChar = Cast<ABaseEnemyCharacter>(OwnerComp.GetOwner());
		if (EnemyChar)
			EnemyChar->OnChangeCanMove.AddDynamic(this, &UBTTask_MoveToCPP::ChangeMoveState);
	}

	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_MoveToCPP::ChangeMoveState(APawn* ControlledPawn, bool Value)
{
	Super::OnTaskFinished(*Comp, MyNodeMemory, EBTNodeResult::Aborted);	
}
