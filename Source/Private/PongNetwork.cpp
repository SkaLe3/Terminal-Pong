#include "PongNetwork.h"
#include "Core/Logger.h"


PongNetwork& PongNetwork::Get()
{
	static PongNetwork instance;
	return instance;
}

void PongNetwork::StartServer(uint16 port)
{
	m_IsServer = true;
	m_Running = true;

	Net::Socket server; // Make member to implement late-join/re-join
	if (!server.Bind(port))
	{
		LOG_ERROR("PongNetwork: Failed to bind server");
		return;
	}

	server.Listen();
	LOG_INFO("PongNetwrok: Server started. Waiting for 2 players...");

	m_Clients.reserve(2);

	for (int32 i = 0; i < 2; i++)
	{
		Net::Socket client = server.Accept();
		if (!client.IsValid())
		{
			LOG_ERROR("PongNetwork: Accept failed");
			return;
		}

		ClientConnection conn;
		conn.Socket = std::move(client);
		conn.PlayerIndex = i;
		conn.Thread = std::thread(&PongNetwork::ClientThread, this, i);
		m_Clients.push_back(std::move(conn));

		ConnectMessage msg{};
		msg.Type = PongMessage::Connect;
		msg.PlayerIndex = i;

		uint16 size = sizeof(msg);
		m_Clients.back().Socket.Send(&size, sizeof(size));
		m_Clients.back().Socket.Send(&msg, size);

		LOG_INFO("PongNetwork: Player connected: " + std::to_string(i));
	}

	LOG_INFO("PongNetwork: Both players connected. Game start.");
}

bool PongNetwork::ConnectClient(const std::string& ip, uint16 port)
{
	m_IsServer = false;
	m_Running = true;

	OnConnect = [&](const ConnectMessage& playerIndex)
		{
			m_LocalPlayerIndex = playerIndex.PlayerIndex;
		};

	if (!m_ClientSocket.Connect(ip, port))
	{
		LOG_ERROR("PongNetwork: Failed to connect to server");
		return false;
	}

	m_ClientThread = std::thread(&PongNetwork::ClientThread, this, -1);

	LOG_INFO("PongNetwork: Connected to server");
	return true;
}

void PongNetwork::Shutdown()
{
	m_Running = false;

	if (m_ClientSocket.IsValid())
		m_ClientSocket.Close();
	if (m_ClientThread.joinable())
		m_ClientThread.join();

	for (auto& c : m_Clients)
	{
		if (c.Socket.IsValid())
			c.Socket.Close();
		if (c.Thread.joinable())
			c.Thread.join();
	}
	m_Clients.clear();
}


void PongNetwork::SendInput(float move)
{
	if (!m_Running || m_IsServer)
	{
		return;
	}

	InputMessage msg{};
	msg.Type = PongMessage::Input;
	msg.Move = move;

	uint16 size = sizeof(msg);
	m_ClientSocket.Send(&size, sizeof(size));
	m_ClientSocket.Send(&msg, size);
}

void PongNetwork::SendBallState(const BallStateMessage& msg)
{
	if (!m_Running || !m_IsServer)
	{
		return;
	}

	for (auto& client : m_Clients)
	{
		uint16 size = sizeof(msg);
		client.Socket.Send(&size, sizeof(size));
		client.Socket.Send(&msg, size);
	}
}

void PongNetwork::SendPaddleState(const PaddleStateMessage& msg)
{
	if (!m_Running || !m_IsServer)
	{
		return;
	}
	for (auto& client : m_Clients)
	{
		uint16 size = sizeof(msg);
		client.Socket.Send(&size, sizeof(size));
		client.Socket.Send(&msg, size);
	}
}

void PongNetwork::ClientThread(int32 playerIndex)
{
	Net::Socket* socket = nullptr;

	if (m_IsServer)
	{
		if (playerIndex < 0 || playerIndex >(int32) m_Clients.size())
		{
			LOG_ERROR("Invalid player index: " + std::to_string(playerIndex) + " passed to ClientThread");
			return;
		}
		socket = &m_Clients[playerIndex].Socket;
	}
	else
	{
		socket = &m_ClientSocket;
	}
	if (!socket)
	{
		LOG_ERROR("Invalid socket in ClientThread");
		return;
	}

	while (m_Running)
	{
		uint16 size = 0;
		int32 r = socket->Receive(&size, sizeof(size));
		if (r <= 0)
			break;

		NetEvent evt;
		evt.PlayerIndex = playerIndex;
		evt.Data.resize(size);

		if (socket->Receive(evt.Data.data(), size) <= 0)
			break;

		std::lock_guard lock(m_QueueMutex);
		m_Inbox.push(std::move(evt));
	}
	LOG_INFO("Client thread ended");
}

void PongNetwork::Poll()
{
	std::lock_guard lock(m_QueueMutex);

	while (!m_Inbox.empty())
	{
		NetEvent& evt = m_Inbox.front();
		PongMessage type = *(PongMessage*)evt.Data.data();

		switch (type)
		{
			case PongMessage::BallState:
				if (OnBall)
					OnBall(*(BallStateMessage*)evt.Data.data());
				break;
		case PongMessage::PaddleState:
		{
			PaddleStateMessage message = *(PaddleStateMessage*)evt.Data.data();
			if (OnPaddle0 && message.playerIndex == 0)
				OnPaddle0(message);
			if (OnPaddle1 && message.playerIndex == 1)
				OnPaddle1(message);
		}
			break;
		case PongMessage::Input:
		{

			if (OnInputPlayer0 && evt.PlayerIndex == 0)
				OnInputPlayer0(*(InputMessage*)evt.Data.data());
			if (OnInputPlayer1 && evt.PlayerIndex == 1)
				OnInputPlayer1(*(InputMessage*)evt.Data.data());
		}
		break;
		case PongMessage::Connect:
			if (OnConnect)
				OnConnect(*(ConnectMessage*)evt.Data.data());
		}
		m_Inbox.pop();
	}
}