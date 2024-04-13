// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChatClientComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_ReceiveMessage, const FString&, Message);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCHATCLIENT_API UChatClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChatClientComponent();
    ~UChatClientComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // 메시지 전송
    UFUNCTION(BlueprintCallable)
    void SendMessage(const FString& Message);

    // 메시지 수신
    FString ReceiveMessage();

private:
    FSocket* m_Socket;

    UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
    FDele_ReceiveMessage EventReceiveMessage;
};
