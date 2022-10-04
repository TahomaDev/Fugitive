// MageHunter @2021 Bardak Studio. All Rights Reserved

#pragma once

#include "UObject/Interface.h"
#include "NetworkInterface.generated.h"

class FOnlineSessionSearchResult;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNetworkInterface : public UInterface
{
	GENERATED_BODY()
};

/** @Tahoma. Интерфейс для работы с сетевыми подключениями
 */
class FUGITIVE_API INetworkInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Network")	
    void CreateNetworkSession_Client(APlayerController* Owner, int MaxPlayers, bool bIsSteam);

//	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Network")	
//    virtual void JoinNetworkSession(FOnlineSessionSearchResult& OnlineSessionSearchResult);

};
