#pragma once
#include "Core/Core.h"
#include "PongSync.h"
#include "Networking/Message.h"
#include "PongSync.h"

#include <functional>
#include <thread>
#include <mutex>
#include <queue>

struct ClientConnection
{
	Net::Socket Socket;
	std::thread Thread;
	int32 PlayerIndex = -1;
};

struct NetEvent
{
    int32 PlayerIndex = -1;
    std::vector<Net::Byte> Data;
};

class PongNetwork
{
public:
    static PongNetwork& Get();

    int32 GetLocalPlayerIndex() { return m_LocalPlayerIndex; }
    bool HasLocalPlayerIndex() { return m_LocalPlayerIndex != -1; }

    void StartServer(uint16 port);
    bool ConnectClient(const std::string& ip, uint16 port);
    void Shutdown();

    void SendInput(float move);

    void SendBallState(const BallStateMessage& msg);
    void SendPaddleState(const PaddleStateMessage& msg);

    void Poll();

    std::function<void(const BallStateMessage&)> OnBall;
    std::function<void(const PaddleStateMessage&)> OnPaddle0;
    std::function<void(const PaddleStateMessage&)> OnPaddle1;
    std::function<void(const InputMessage&)> OnInputPlayer0;
    std::function<void(const InputMessage&)> OnInputPlayer1;
    std::function<void(const ConnectMessage&)> OnConnect;

    bool IsServer() const { return m_IsServer; }
    bool IsConnected() const { return m_Running; }
private:
    PongNetwork() = default;
    ~PongNetwork() = default;

    PongNetwork(const PongNetwork&) = delete;
    PongNetwork& operator=(const PongNetwork&) = delete;

    void ClientThread(int32 playerIndex);

private:

    std::atomic<int32> m_LocalPlayerIndex = -1;

    bool m_IsServer = false;
    std::atomic<bool> m_Running = false;


    Net::Socket m_ClientSocket;
    std::thread m_ClientThread;

    std::vector<ClientConnection> m_Clients;


    std::mutex m_QueueMutex;
    std::queue<NetEvent> m_Inbox;
};