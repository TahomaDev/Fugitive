// MageHunter @2021 Bardak Studio. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

/**
 * 
 */
UCLASS()
class FUGITIVE_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void Init();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage*	IMG_NPC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage*	IMG_GG;
};

