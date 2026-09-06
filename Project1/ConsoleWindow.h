#include "pch.h"

class ConsoleWindow
{
	char InputBuf[256];
	std::vector<FString> Logs;
	std::vector<const char*> Commands;
	std::vector<FString> History;
	int  HistoryPos;
	ImGuiTextFilter Filter;
	bool AutoScroll;
	bool ScrollToBottom;

	ConsoleWindow();
	~ConsoleWindow();

	void AddLogs(const char* Format, ...);
	void ClearLogs();

	void DrawConsole();
	void Executor(const char* Command);
};

