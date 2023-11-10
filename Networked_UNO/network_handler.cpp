#pragma comment(lib, "ws2_32.lib")

#include "network_handler.hpp"

#include <WS2tcpip.h>

#include <array>
#include <vector>
#include <thread>
#include <future>
#include <cstring>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

namespace networked_UNO {
	network_handler::connection_results network_handler::client_connection(SOCKET udp_socket, std::atomic_bool& running) {
		const std::string match_string("Networked_UNO_CNT_4007_2023");

		sockaddr incoming_addr = {};
		int sockaddr_size = sizeof(sockaddr);
		
		bool found_server = false;
		std::array<char, 1024> data_buffer;
		while (!found_server && running) {
			int inc_data_size = recvfrom(udp_socket, data_buffer.data(), static_cast<int>(data_buffer.size()), 0, &incoming_addr, &sockaddr_size);
			if (inc_data_size == -1)
				return { INVALID_SOCKET, "Error! Could not receive data from server. Terminating.\n" };
			if (inc_data_size == 0)
				return { INVALID_SOCKET, "Error! Server connection has shutdown. Terminating.\n" };
			
			if (inc_data_size == match_string.size())
				if (match_string.compare(0, inc_data_size, data_buffer.data()) == 0)
					found_server = true;
		}

		SOCKET tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tcp_socket == INVALID_SOCKET)
			return { INVALID_SOCKET, "Error! Could not create TCP socket. Terminating.\n" };

		if (connect(tcp_socket, &incoming_addr, sizeof(sockaddr)) == -1) {
			closesocket(tcp_socket);
			return { INVALID_SOCKET, "Error! Could not establish TCP connection to server. Terminating.\n" };
		}

		if (!running)
			closesocket(tcp_socket);

		return { tcp_socket, "" };
	}

	network_handler::connection_results network_handler::server_connection(SOCKET udp_socket, std::atomic_bool& running) {
		return { INVALID_SOCKET, "" };
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
			
		addrinfo* addr_info;
		if (getaddrinfo("", nullptr, &addr_hint, &addr_info) != 0) {
			std::cout << "Error! Could not get localhost info. Terminating.\n";
			return nullptr;
		}

		SOCKET new_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (new_socket == INVALID_SOCKET) {
			std::cout << "Error! Could not create UDP broadcast socket. Terminating.\n";
			return nullptr;
		}

		sockaddr_in* ip_addr = reinterpret_cast<sockaddr_in*>(addr_info->ai_addr);
		ip_addr->sin_addr.S_un.S_un_b.s_b4 = 255; // Set to the UDP broadcast address.

		if (connect(new_socket, addr_info->ai_addr, addr_info->ai_addrlen) != 0) {
			std::cout << "Error! Socket could not connect. Terminating\n";
			return nullptr;
		}
		
		bool client = true;
		bool connected = false;
		while (!connected) {
			std::atomic_bool running = true;
			std::future<connection_results> connection_future;
			running = true;
			if (client)
				connection_future = std::async(network_handler::client_connection, new_socket, std::ref(running));
			else
				connection_future = std::async(network_handler::server_connection, new_socket, std::ref(running));

			while (connection_future.wait_for(16ms) != std::future_status::ready) {
				if (client)
			}
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