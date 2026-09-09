#pragma once
#include "pch.h"

class ConsoleWindow
{
public:
	static ConsoleWindow& GetInstance();

	void DrawConsole();
	void AddLog(const char* Format, ...);
	void ClearLogs();

	void RequestResize();

private:
	ConsoleWindow();
	~ConsoleWindow();

	ConsoleWindow(const ConsoleWindow&) = delete;
	ConsoleWindow& operator=(const ConsoleWindow&) = delete;

	// UI
	void UpdateLayout();
	void DrawToolBar();
	void DrawOptionsAndFilter();
	void DrawLogArea();
	void DrawInput();
	void DrawSuggestions();

	// 명령어 실행
	void ExecuteCommand(const char* Input);
	bool ExecuteBasicCommand(const FString& Command);
	void ExecuteHelp();
	void ExecuteHistory();

	// UE_LOG 처리
	void NormalizeQuotes(FString& Command);
	bool ParseUELog(const FString& Input);
	void ExecuteUELog();

	// ISA 처리
	bool ParseTypeCompare(const FString& Input);
	void ExecuteTypeCompare();

	// InputText Callback
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* Data);
	int TextEditCallback(ImGuiInputTextCallbackData* Data);

	// History / 자동완성
	void HandleHistory(ImGuiInputTextCallbackData* Data);
	void HandleCompletion(ImGuiInputTextCallbackData* Data);
	void HandleSuggestionNavigation(ImGuiInputTextCallbackData* Data);
	void UpdateSuggestions(const FString& Input);
	
	FString Trim(const FString& Str);
	FString ToUpper(const FString& Str);

private:
	TArray<FString> Logs;
	TArray<FString> ParsedArguments;

	TArray<FString> Commands;
	TArray<FString> History;
	TArray<FString> Suggestions;

	char InputBuf[256] = {};

	int HistoryPos = -1;
	int SelectedSuggestion = -1;

	ImGuiTextFilter Filter;

	bool AutoScroll = true;
	bool bWasCollapsed = false;
	bool bResizeRequested = true;
};




#define UE_LOG(...) ConsoleWindow::GetInstance().AddLog(__VA_ARGS__)