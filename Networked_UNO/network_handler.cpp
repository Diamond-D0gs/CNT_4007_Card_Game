#pragma comment(lib, "ws2_32.lib")

#include "network_handler.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinUser.h>

#include <array>
#include <vector>
#include <thread>
#include <future>
#include <cstring>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

namespace networked_UNO {
	const std::string match_string("Networked_UNO_CNT_4007_2023");

	bool create_UDP_socket(const addrinfo* host_addr_info, const uint32_t time_out, SOCKET* socket_out) {
		SOCKET udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (udp_socket == INVALID_SOCKET) {
			std::cout << "Error! Could not create UDP broadcast socket. Terminating.\n";
			return false;
		}

		// Set UDP socket send and receive timeout.
		uint32_t udp_time_out = time_out;
		if (setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&udp_time_out), sizeof(uint32_t)) != 0) {
			std::cout << "Error! Could not configure UDP socket. Terminating.\n";
			// Close socket if configuration failed.
			closesocket(udp_socket);
			return false;
		}
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
		ip_addr->sin_addr.S_un.S_un_b.s_b4 = 255; // Set to the UDP broadcast address.

		// Connect socket to the UDP broadcast address.
		if (connect(udp_socket, &udp_broadcast_addr, udp_addr_len) != 0) {
			std::cout << "Error! Socket could not connect. Terminating\n";
			return false;
		}
		
		*socket_out = udp_socket;

		return true;
	}

	void client_connection(SOCKET udp_socket, std::atomic_bool& running, std::atomic_bool& done, sockaddr* sockaddr_out) {
		std::array<char, 256> data_buffer;
		
		while (!done && running) {
			sockaddr temp_sockaddr;
			int from_len = sizeof(sockaddr);
			int data_size = recvfrom(udp_socket, data_buffer.data(), static_cast<int>(data_buffer.size()), 0, &temp_sockaddr, &from_len);
			if (data_size >= 0 && WSAGetLastError() != WSAETIMEDOUT)
				running = false;
			else if (data_size == data_buffer.size()) {
				if (match_string.compare(0, data_size, data_buffer.data(), data_buffer.size()) == 0) {
					*sockaddr_out = temp_sockaddr;
					running = false;
					done = true;
				}
			}
		}
	}

	void server_connection(SOCKET udp_socket, SOCKET tcp_socket, std::atomic_bool& running, std::atomic_bool& done, SOCKET* client_tcp_out) {
		while (!done && running) {
			int data_sent = send(udp_socket, match_string.c_str(), static_cast<int>(match_string.size()), 0);
			if (data_sent == SOCKET_ERROR)
				running = false;
			else if (data_sent == match_string.size()) {
				if (listen(tcp_socket, 0) == 0) {
					*client_tcp_out = accept(tcp_socket, nullptr, nullptr);
					if (*client_tcp_out != INVALID_SOCKET)
						done = true;
					running = false;
				}
			}
			
			if (!running)
				std::this_thread::sleep_for(100ms);
		}
	}

	network_handler* network_handler::start_networking() {
		WSAData wsa_data;
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
			std::cout << "Error! Failed to start Winsock. Terminating.\n";
			return nullptr;
		}

		// Get an IPV4 address for a UDP socket.
		addrinfo addr_hint = {
			.ai_family = AF_INET,
			.ai_socktype = SOCK_DGRAM,
			.ai_protocol = IPPROTO_UDP
		};
			
		addrinfo* host_addr_info;
		if (getaddrinfo("", nullptr, &addr_hint, &host_addr_info) != 0) {
			std::cout << "Error! Could not get localhost info. Terminating.\n";
			return nullptr;
		}

		SOCKET udp_socket;
		if (!create_UDP_socket(host_addr_info, 100, &udp_socket))
			return nullptr;

		SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tcp_socket == INVALID_SOCKET) {
			std::cout << "Error! Failed to create socket for server. Terminating.\n";
			closesocket(udp_socket);
			return nullptr;
		}
		
		// Wait loop for the incoming connection.
		bool client = true;
		std::atomic_bool done = false;
		std::atomic_bool running = true;
		SOCKET client_socket = INVALID_SOCKET;
		sockaddr server_address = {};
		while (!done) {
			running = true;

			// Create a seperate thread to wait for a connection on.
			std::jthread connection;
			if (client)
				connection = std::move(std::jthread(client_connection, udp_socket, std::ref(running), std::ref(done), &server_address));
			else
				connection = std::move(std::jthread(server_connection, udp_socket, tcp_socket, std::ref(running), std::ref(done), &client_socket));

			uint64_t ticks = 0;
			while (running) {
				std::this_thread::sleep_for(100ms);

				// Clear cout and return to the beginning of the current line.
				std::cout.flush();
				std::cout << "\r";
				
				if (client)
					std::cout << "Waiting for server connection...";
				else
					std::cout << "Waiting for client connection...";

				switch (ticks++ % 4) {
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

				std::cout << " (Press \"M\" to switch to " << ((client) ? "server" : "client") << " mode).";

				// Check if the M key was pressed.
				if (GetAsyncKeyState('M') != 0) {
					client = (client + 1) % 2;
					running = false;
				}
			}

			if (client && !done)
				if (client_socket != INVALID_SOCKET)
					closesocket(client_socket);
		}
		
		if (!client) {
			
		}

		return nullptr;
	}

	bool network_handler::terminate_networking(network_handler* network_handler) {
		if (network_handler == nullptr)
			return false;
		if (WSACleanup() != 0)
			return false;

		delete network_handler;

		return true;
	}
}