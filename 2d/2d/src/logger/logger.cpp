#include "logger.hpp"
#include <iostream>
#include <chrono>

std::vector<LogEntry> Logger::messages;

namespace
{
	auto green_text     = std::string{ "\u001b[32m" };
	auto red_text       = std::string{ "\u001b[32m" };
	auto end_color = std::string{ "\u001b[32m" };

}

void Logger::Log(const std::string& message)
{
	const auto now = std::chrono::system_clock::now();
	auto time_string = std::format("{:%d-%m-%Y %H:%M:%OS}", now);

	LogEntry logEntry{
		.logType = LOG_INFO,
		.message = time_string
	};
	std::cout << green_text << "[LOG] " << time_string << " - " << message << end_color << '\n';
	messages.push_back(logEntry);
}

void Logger::Err(const std::string& message)
{
	const auto now = std::chrono::system_clock::now();
	auto time_string = std::format("{:%d-%m-%Y %H:%M:%OS}", now);

	LogEntry logEntry{
		.logType = LOG_ERROR,
		.message = time_string
	};

	std::cerr << red_text << "[ERROR] " << time_string << " -  " << message << end_color << '\n';
	messages.push_back(logEntry);
}
