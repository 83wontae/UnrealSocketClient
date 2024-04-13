// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatClientComponent.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"  // 소켓 헤더
#include "HAL/RunnableThread.h"  // RunnableThread 헤더
#include "NetworkMessage.h"

// Sets default values for this component's properties
UChatClientComponent::UChatClientComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
}

UChatClientComponent::~UChatClientComponent()
{
    if(m_Socket)
        m_Socket->Close();
}

// Called when the game starts
void UChatClientComponent::BeginPlay()
{
    Super::BeginPlay();

    // 소켓 초기화
    m_Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

    // 서버 주소 설정
    FString ServerAddress = TEXT("127.0.0.1"); // 예시 서버 주소
    int32 Port = 27015; // 예시 포트 번호

    FIPv4Address ip;
    FIPv4Address::Parse(ServerAddress, ip);

    TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    RemoteAddr->SetIp(ip.Value);
    RemoteAddr->SetPort(Port);

    // 연결 시도
    if (!m_Socket->Connect(*RemoteAddr))
    {
        //UE_LOG(LogTemp, Error, TEXT("Failed to connect to server!"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Failed to connect to server!"));
        return;
    }

    //UE_LOG(LogTemp, Log, TEXT("Connected to the server!"));
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Connected to the server!"));
}

// Called every frame
void UChatClientComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 메시지 수신 및 처리를 여기에서 수행합니다.
    FString strMsg = ReceiveMessage();

    if (strMsg.IsEmpty() == true)
        return;

    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, strMsg);
    EventReceiveMessage.Broadcast(strMsg);
}

void UChatClientComponent::SendMessage(const FString& Message)
{
    // FString을 TCHAR 배열로 변환
    TArray<uint8> SendData;
    FTCHARToUTF8 Converter(*Message);
    SendData.Append(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length());

    // 데이터 송신
    int32 BytesSent = 0;
    m_Socket->Send(SendData.GetData(), SendData.Num(), BytesSent);
    if (BytesSent <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to send data!"));
    }
}

FString UChatClientComponent::ReceiveMessage()
{
    // 데이터 수신
    TArray<uint8> ReceivedData;
    uint32 Size;
    while (m_Socket->HasPendingData(Size))
    {
        ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
        int32 BytesRead = 0;
        m_Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);
        if (BytesRead > 0)
        {
            // TCHAR 배열을 FString으로 변환하여 반환
            return FString(UTF8_TO_TCHAR(ReceivedData.GetData()));
        }
    }
    return FString(); // 데이터를 받지 못한 경우 빈 문자열 반환
}