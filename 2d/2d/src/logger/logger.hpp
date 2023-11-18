#pragma once
#include <string>
#include <vector>

enum LogType{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

struct LogEntry
{
	LogType logType;
	std::string message;
};

class Logger
{
public:
	Logger() = default;
	~Logger() = default;

	static std::vector<LogEntry> messages;
	// better to not be a class but just empty.
	static void Log(const std::string& message);
	static void Err(const std::string& message);

};