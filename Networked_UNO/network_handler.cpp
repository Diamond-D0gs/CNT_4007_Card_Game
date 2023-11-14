#pragma comment(lib, "ws2_32.lib")

#include "network_handler.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinUser.h>

#include <array>
#include <chrono>
#include <string>
#include <atomic>
#include <thread>
#include <cstdlib>
#include <iostream>

using namespace std::chrono_literals;

namespace networked_UNO {
	void display_waiting_indicator(const char* name, const uint64_t tick) {
		std::cout.flush();

		std::cout << "\rWaiting for " << name << " connection...";

		switch (tick % 4) {
		case 0:
			std::cout << "|";
			break;
		case 1:
			std::cout << "/";
			break;
		case 2:
			std::cout << "-";
			break;
		case 3:
			std::cout << "\\";
			break;
		}

		std::cout << " (Press \"M\" to switch to " << name << " mode).";
	}

	bool create_UDP_socket(const addrinfo* host_addr_info, const uint16_t port, const uint32_t time_out, SOCKET* socket_out) {
		SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (udp_socket == INVALID_SOCKET) {
			std::cout << "Error! Could not create UDP broadcast socket. Terminating.\n";
			return false;
		}

		// Set UDP socket send and receive timeout.
		uint32_t udp_time_out = time_out;
		//if (setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&udp_time_out), sizeof(uint32_t)) != 0) {
		//	std::cout << "Error! Could not configure UDP socket. Terminating.\n";
		//	// Close socket if configuration failed.
		//	closesocket(udp_socket);
		//	return false;
		//}
		if (setsockopt(udp_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&udp_time_out), sizeof(uint32_t)) != 0) {
			std::cout << "Error! Could not configure UDP socket. Terminating.\n";
			closesocket(udp_socket);
			return false;
		}

		// Set UDP socket to be able to broadcast.
		bool udp_broadcast = true;
		if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&udp_broadcast), sizeof(bool)) != 0) {
			std::cout << "Error! Could not configure UDP socket. Terminating.\n";
			closesocket(udp_socket);
			return false;
		}

		// Create UDP broadcast addrinfo
		sockaddr udp_broadcast_addr = *host_addr_info->ai_addr;
		int udp_addr_len = host_addr_info->ai_addrlen;

		// Cast addrinfo to a sockaddr_in to edit the IP address.
		sockaddr_in* ip_addr = reinterpret_cast<sockaddr_in*>(&udp_broadcast_addr);
		ip_addr->sin_addr.S_un.S_un_b.s_b3 = 255; // Set to the UDP broadcast address.
		ip_addr->sin_addr.S_un.S_un_b.s_b4 = 255; // Set to the UDP broadcast address.
		ip_addr->sin_port = htons(port);
		ip_addr->sin_family = AF_INET;

		// Connect socket to the UDP broadcast address.
		if (connect(udp_socket, &udp_broadcast_addr, udp_addr_len) != 0) {
			auto error = WSAGetLastError();
			std::cout << "Error! Socket could not connect. Terminating\n";
			return false;
		}
		
		*socket_out = udp_socket;

		return true;
	}

	bool create_TCP_socket(const uint32_t time_out, SOCKET* socket_out) {
		SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tcp_socket == INVALID_SOCKET) {
			std::cout << "Error! Failed to create TCP socket. Terminating.\n";
			return false;
		}

		// Set TCP socket send and receive timeout.
		uint32_t tcp_time_out = time_out;
		if (setsockopt(tcp_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&tcp_time_out), sizeof(uint32_t)) != 0) {
			std::cout << "Error! Could not configure TCP socket. Terminating.\n";
			// Close socket if configuration failed.
			closesocket(tcp_socket);
			return false;
		}
		if (setsockopt(tcp_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&tcp_time_out), sizeof(uint32_t)) != 0) {
			std::cout << "Error! Could not configure TCP socket. Terminating.\n";
			closesocket(tcp_socket);
			return false;
		}

		*socket_out = tcp_socket;

		return true;
	}

	bool waiting_for_server_connection(SOCKET& tcp_socket_out) {
		// Create the UDP socket to receive the server's broadcast packet.
		SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (udp_socket == INVALID_SOCKET) {
			std::cout << "Error! Failed to create UDP socket. Terminating.\n";
			return false;
		}

		// Configure the receive timeout value of the socket.
		if (setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&UDP_TIME_OUT_MS), sizeof(uint32_t)) != 0) {
			std::cout << "Error! Could not configure UDP socket. Terminating.\n";
			return false;
		}

		sockaddr_in udp_sock_addr = {
			.sin_family = AF_INET,
			.sin_port = htons(8757),
			.sin_addr = INADDR_ANY
		};

		if (bind(udp_socket, reinterpret_cast<sockaddr*>(&udp_sock_addr), sizeof(sockaddr_in)) != 0) {
			std::cout << "Error! Could not bind UDP socket. Terminating.\n";
			return false;
		}

		// Store the server's address info.
		sockaddr_in server_sock_addr;
		int sock_addr_len = sizeof(sockaddr_in);

		uint64_t ticks = 0;
		bool found_server = false;
		std::array<char, DATA_RECV_SIZE> data_recv_buffer;
		while (!found_server) {
			display_waiting_indicator("server", ticks++);

			// Clear out the data buffer.
			memset(data_recv_buffer.data(), 0, data_recv_buffer.size());
			
			// Check if the M key was pressed.
			if (GetAsyncKeyState('M') != 0) {
				if (closesocket(udp_socket) != 0) {
					std::cout << "\nError! Could not terminate UDP socket. Terminating.\n";
					return false;
				}
				return true; // Break out without terminating.
			}

			int data_recv_size = recvfrom(udp_socket, data_recv_buffer.data(), static_cast<int>(data_recv_buffer.size()), 0, reinterpret_cast<sockaddr*>(&server_sock_addr), &sock_addr_len);
			if (data_recv_size <= 0 && WSAGetLastError() != WSAETIMEDOUT) {
				std::cout << "\nError! Could not receive data. Terminating.\n";
				closesocket(udp_socket);
				return false;
			}
			else if (data_recv_size == IDENTITY_STRING.size()) {
				if (IDENTITY_STRING.compare(0, IDENTITY_STRING.size(), data_recv_buffer.data()) == 0)
					found_server = true;
			}
		}

		std::cout << "\rSuccess! Connection established to server. One Moment...                        \n";

		// Close the UDP socket, no longer needed.
		if (closesocket(udp_socket) != 0) {
			std::cout << "Error! Could not terminate UDP socket. Terminating.\n";
			return false;
		}

		SOCKET temp_tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (temp_tcp_socket == INVALID_SOCKET) {
			std::cout << "Error! Could not create TCP socket. Terminating.\n";
			return false;
		}

		if (connect(temp_tcp_socket, reinterpret_cast<sockaddr*>(&server_sock_addr), sizeof(sockaddr_in)) != 0) {
			std::cout << "Error! Could not connect to server. Terminating.\n";
			closesocket(temp_tcp_socket);
			return false;
		}

		tcp_socket_out = temp_tcp_socket;
		
		return true;
	}

	bool waiting_for_client_connection(SOCKET& tcp_socket_out, SOCKET& client_socket_out) {
		// Get an IPV4 address for a UDP socket.
		addrinfo addr_hint = {
			.ai_family = AF_INET
		};
		
		addrinfo* host_addr_info;
		if (getaddrinfo("", nullptr, &addr_hint, &host_addr_info) != 0) {
			std::cout << "Error! Could not get host info. Terminating.\n";
			return false;
		}

		sockaddr_in tcp_sock_addr = *reinterpret_cast<sockaddr_in*>(&host_addr_info->ai_addr);
		tcp_sock_addr.sin_family = AF_INET;

		sockaddr_in udp_sock_addr = tcp_sock_addr;
		udp_sock_addr.sin_addr.S_un.S_un_b.s_b4 = 255; // Set to UDP broadcast address.

		freeaddrinfo(host_addr_info);

		// Create the UDP socket.
		SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (udp_socket == INVALID_SOCKET) {
			std::cout << "Error! Unable to create UDP socket.\n Terminating.";
			return false;
		}

		// Enable the UDP socket to broadcast.
		const bool broadcast = true;
		if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcast), sizeof(bool)) != 0) {
			std::cout << "Error! Could not configure UDP socket.\n Terminating.\n";
			return false;
		}

		SOCKET temp_tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (temp_tcp_socket == INVALID_SOCKET) {
			std::cout << "Error! Could not create TCP socket.\n Terminating.\n";
			closesocket(udp_socket);
			return false;
		}
		
		uint64_t ticks = 0;
		bool found_server = false;
		while (!found_server) {
			display_waiting_indicator("client", ticks++);
			
			if (GetAsyncKeyState('M') != 0)
				return true;

			std::this_thread::sleep_for(100ms);
		}
		
		return true;
	}

	void server_connection(SOCKET udp_socket, SOCKET tcp_socket, std::atomic_bool& running, std::atomic_bool& done, SOCKET* client_tcp_out) {
		while (!done && running) {
			int data_sent = send(udp_socket, IDENTITY_STRING.c_str(), static_cast<int>(IDENTITY_STRING.size()), 0);
			if (data_sent == SOCKET_ERROR)
				running = false;
			else if (data_sent == IDENTITY_STRING.size()) {
				if (listen(tcp_socket, 0) == 0) {
					*client_tcp_out = accept(tcp_socket, nullptr, nullptr);
					if (*client_tcp_out != INVALID_SOCKET)
						done = true;
					running = false;
				}
			}
		}
	}
	
	//network_handler* network_handler::start_networking() {
	//	WSAData wsa_data;
	//	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
	//		std::cout << "Error! Failed to start Winsock. Terminating.\n";
	//		return nullptr;
	//	}
	//
	//	// Get an IPV4 address for a UDP socket.
	//	addrinfo addr_hint = {
	//		.ai_family = AF_INET,
	//		.ai_socktype = SOCK_DGRAM
	//	};
	//		
	//	addrinfo* host_addr_info;
	//	if (getaddrinfo("", nullptr, &addr_hint, &host_addr_info) != 0) {
	//		std::cout << "Error! Could not get localhost info. Terminating.\n";
	//		return nullptr;
	//	}
	//
	//	SOCKET udp_socket;
	//	if (!create_UDP_socket(host_addr_info, PORT, UDP_TIME_OUT_MS, &udp_socket))
	//		return nullptr;
	//	
	//	// Wait loop for the incoming connection.
	//	bool client = true;
	//	std::atomic_bool done = false;
	//	std::atomic_bool running = true;
	//	SOCKET client_socket = INVALID_SOCKET;
	//	SOCKET tcp_socket = INVALID_SOCKET;
	//	sockaddr server_address = {};
	//	while (!done) {
	//		running = true;
	//
	//		// Create a seperate thread to wait for a connection on.
	//		std::jthread connection;
	//		if (client)
	//			connection = std::move(std::jthread(client_connection, udp_socket, std::ref(running), std::ref(done), &server_address));
	//		else {
	//			if (!create_TCP_socket(1000, &tcp_socket)) {
	//				closesocket(udp_socket);
	//				return nullptr;
	//			}
	//			
	//			sockaddr server_tcp_sock_addr = *host_addr_info->ai_addr;
	//			reinterpret_cast<sockaddr_in*>(&server_tcp_sock_addr)->sin_port = PORT;
	//			if (bind(tcp_socket, &server_tcp_sock_addr, sizeof(sockaddr)) != 0) {
	//				std::cout << "Error! Could not bind TCP socket to listening address. Terminating.\n";
	//				return nullptr;
	//			}
	//
	//			connection = std::move(std::jthread(server_connection, udp_socket, tcp_socket, std::ref(running), std::ref(done), &client_socket));
	//		}
	//
	//		uint64_t ticks = 0;
	//		while (running) {
	//			std::this_thread::sleep_for(100ms);
	//
	//			// Clear cout and return to the beginning of the current line.
	//			std::cout.flush();
	//			std::cout << "\r";
	//			
	//			if (client)
	//				std::cout << "Waiting for server connection...";
	//			else
	//				std::cout << "Waiting for client connection...";
	//
	//			switch (ticks++ % 4) {
	//			case 0:
	//				std::cout << "|";
	//				break;
	//			case 1:
	//				std::cout << "/";
	//				break;
	//			case 2:
	//				std::cout << "-";
	//				break;
	//			case 3:
	//				std::cout << "\\";
	//				break;
	//			}
	//
	//			std::cout << " (Press \"M\" to switch to " << ((client) ? "server" : "client") << " mode).";
	//
	//			// Check if the M key was pressed.
	//			if (GetAsyncKeyState('M') != 0) {
	//				client = (client + 1) % 2;
	//				running = false;
	//			}
	//		}
	//
	//		// Close if socket does exist but mode was switched.
	//		if (client && !done) {
	//			closesocket(tcp_socket);
	//			if (client_socket != INVALID_SOCKET)
	//				closesocket(client_socket);
	//		}
	//	}
	//
	//	return nullptr;
	//}

network_handler* network_handler::start_networking() {
	WSAData wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		std::cout << "Error! Failed to start Winsock. Terminating.\n";
		return nullptr;
	}

	// Wait loop for the incoming connection.
	bool client = false;
	SOCKET tcp_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET;
	while (tcp_socket == INVALID_SOCKET) {
		bool result = false;
		if (client)
			result = waiting_for_server_connection(tcp_socket);
		else
			result = waiting_for_client_connection(tcp_socket, client_socket);
		
		if (!result)
			return nullptr;

		if (tcp_socket == INVALID_SOCKET)
			client = (client + 1) % 2;
	}

	return nullptr;
}

	bool network_handler::terminate_networking(network_handler* network_handler) {
		if (network_handler == nullptr)
			return false;
		if (WSACleanup() != 0)
			return false;

		closesocket(network_handler->tcp_socket);

		delete network_handler;

		return true;
	}
}