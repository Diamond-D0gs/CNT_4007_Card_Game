#include "network_handler.hpp"
#include "client_handler.hpp"
#include "server_handler.hpp"

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
	network_handler::~network_handler() {
		closesocket(tcp_socket);
		WSACleanup();
	}

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
			.sin_port = htons(PORT),
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
		if (getaddrinfo("..localmachine", nullptr, &addr_hint, &host_addr_info) != 0) {
			std::cout << "Error! Could not get host info. Terminating.\n";
			return false;
		}

		// Setup TCP listening address.
		sockaddr_in tcp_sock_addr = *reinterpret_cast<sockaddr_in*>(host_addr_info->ai_addr);
		tcp_sock_addr.sin_family = AF_INET;
		tcp_sock_addr.sin_port = htons(PORT);

		// Setup UDP broadcast address.
		sockaddr_in udp_sock_addr = tcp_sock_addr;
		udp_sock_addr.sin_addr.S_un.S_un_b.s_b4 = 255;

		freeaddrinfo(host_addr_info);

		// Create the UDP socket.
		SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (udp_socket == INVALID_SOCKET) {
			std::cout << "Error! Unable to create UDP socket. Terminating.\n";
			return false;
		}

		// Enable the UDP socket to broadcast.
		const bool broadcast = true;
		if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcast), sizeof(bool)) != 0) {
			std::cout << "Error! Could not configure UDP socket. Terminating.\n";
			return false;
		}

		SOCKET temp_tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (temp_tcp_socket == INVALID_SOCKET) {
			std::cout << "Error! Could not create TCP socket. Terminating.\n";
			closesocket(udp_socket);
			return false;
		}

		const bool reuseaddr = true;
		if (setsockopt(temp_tcp_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuseaddr), sizeof(bool)) != 0) {
			std::cout << "Error! Could not configure TCP socket. Terminating.\n";
			closesocket(temp_tcp_socket);
			closesocket(udp_socket);
			return false;
		}

		if (bind(temp_tcp_socket, reinterpret_cast<sockaddr*>(&tcp_sock_addr), sizeof(sockaddr_in)) != 0) {
			auto error = WSAGetLastError();
			std::cout << "Error! Failed to bind TCP socket.\n Terminating.\n";
			closesocket(temp_tcp_socket);
			closesocket(udp_socket);
			return false;
		}

		if (listen(temp_tcp_socket, SOMAXCONN) != 0) {
			std::cout << "Error! Could not set TCP socket to listen for incoming connections. Terminating.\n";
			closesocket(temp_tcp_socket);
			closesocket(udp_socket);
			return false;
		}

		// Listen on a seperate thread for an incoming connection to accept.
		SOCKET temp_client_socket = INVALID_SOCKET;
		std::jthread wait_for_client_connection([&]() {
			// Gets terminated by the listening socket being closed or finding a connection.
			temp_client_socket = accept(temp_tcp_socket, nullptr, nullptr);
		});
		
		uint64_t ticks = 0;
		while (temp_client_socket == INVALID_SOCKET) {
			display_waiting_indicator("client", ticks++);
			
			if (GetAsyncKeyState('M') != 0) {
				if (closesocket(temp_tcp_socket) != 0) {
					std::cout << "Error! Could not close TCP listening socket. Terminating.\n";
					closesocket(udp_socket);
					return false;
				}
				if (closesocket(udp_socket) != 0) {
					std::cout << "Error! Could not close UDP broascast socket. Terminating.\n";
					return false;
				}
				return true;
			}

			size_t test = IDENTITY_STRING.size();
			if (sendto(udp_socket, IDENTITY_STRING.c_str(), static_cast<int>(IDENTITY_STRING.size()), 0, reinterpret_cast<sockaddr*>(&udp_sock_addr), sizeof(sockaddr_in)) < 0) {
				if (WSAGetLastError() != WSAETIMEDOUT) {
					std::cout << "Error! Could not broadcast identity string. Terminating.\n";
					closesocket(temp_tcp_socket);
					closesocket(udp_socket);
					return false;
				}
			}

			std::this_thread::sleep_for(100ms);
		}

		tcp_socket_out = temp_tcp_socket;
		client_socket_out = temp_client_socket;
		
		return true;
	}

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

		if (client)
			return new client_handler(tcp_socket);
		else
			return new server_handler(client_socket, tcp_socket);
	}
}