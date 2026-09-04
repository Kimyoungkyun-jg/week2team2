#pragma once

#include <string>
#include <vector>

class EditorConsole 
{
public:
	EditorConsole();

	void Render();

	void AddLog(const std::string& message);
	void Clear();

private:
	std::vector<std::string> logs;
	char InputBuffer[256];
	bool autoScroll = true;
};