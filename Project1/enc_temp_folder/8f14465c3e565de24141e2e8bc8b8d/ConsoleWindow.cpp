#include "pch.h"
#include "ConsoleWindow.h"

ConsoleWindow& ConsoleWindow::GetInstance()
{
	static ConsoleWindow Instance;
	return Instance;
}

ConsoleWindow::ConsoleWindow()
{
	// 콘솔에서 사용할 명령어 목록
	Commands.Add("HELP");
	Commands.Add("CLEAR");
	Commands.Add("HISTORY");
	Commands.Add("UE_LOG");
	Commands.Add("TYPE");
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::AddLog(const char* Format, ...)
{
	char Buffer[1024];

	// 가변 인자를 실제 문자열 하나로 만든다.
	va_list Args;
	va_start(Args, Format);
	vsnprintf(Buffer, sizeof(Buffer), Format, Args);
	va_end(Args);

	Buffer[sizeof(Buffer) - 1] = '\0';

	Logs.Add(Buffer);
}

void ConsoleWindow::ClearLogs()
{
	Logs.Empty();
}

void ConsoleWindow::DrawConsole()
{
	ImGuiViewport* Viewport = ImGui::GetMainViewport();
	ImVec2 WorkPos = Viewport->WorkPos;
	ImVec2 WorkSize = Viewport->WorkSize;

	float ConsoleHeight = WorkSize.y * 0.3f;

	// 메인 화면 하단 30%를 콘솔 영역으로 사용한다.
	ImGui::SetNextWindowPos(ImVec2(WorkPos.x, WorkPos.y + WorkSize.y - ConsoleHeight));
	ImGui::SetNextWindowSize(ImVec2(WorkSize.x, ConsoleHeight));

	ImGui::Begin("Console");

	DrawToolBar();
	DrawOptionsAndFilter();
	DrawLogArea();
	DrawSuggestions();
	DrawInput();

	ImGui::End();
}

void ConsoleWindow::DrawToolBar()
{
	if (ImGui::Button("Add Debug Text"))
		AddLog("Debug Text");

	ImGui::SameLine();

	if (ImGui::Button("Add Debug Error"))
		AddLog("[Error] Debug Error");

	ImGui::SameLine();

	if (ImGui::Button("Clear"))
		ClearLogs();

	ImGui::SameLine();

	if (ImGui::Button("Copy"))
	{
		FString AllLogs;

		// 현재 출력된 모든 로그를 하나의 문자열로 합친다.
		for (const FString& Log : Logs)
		{
			AllLogs += Log;
			AllLogs += '\n';
		}

		ImGui::SetClipboardText(AllLogs.c_str());
	}

	ImGui::Separator();
}

void ConsoleWindow::DrawOptionsAndFilter()
{
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");

	if (ImGui::BeginPopup("Options"))
	{
		// 켜져 있을 때만 새 로그를 따라 아래로 이동한다.
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	ImGui::SameLine();

	Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 300.0f);

	ImGui::Separator();
}

void ConsoleWindow::DrawLogArea()
{
	float FooterHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	// 자동완성 목록이 나오면 그만큼 로그 영역 높이를 줄인다.
	if (!Suggestions.IsEmpty())
		FooterHeight += ImGui::GetFrameHeightWithSpacing() * Suggestions.Num();

	ImGui::BeginChild(
		"ScrollingRegion",
		ImVec2(0, -FooterHeight),
		false,
		ImGuiWindowFlags_HorizontalScrollbar
	);

	// 로그를 그리기 전에 사용자가 원래 맨 아래에 있었는지 기억한다.
	bool WasAtBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();

	for (const FString& Log : Logs)
	{
		if (!Filter.PassFilter(Log.c_str()))
			continue;

		bool HasColor = false;

		// 문자열의 종류에 따라 기본적인 로그 색상을 지정한다.
		if (Log.find("[Error]") != FString::npos)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			HasColor = true;
		}
		else if (Log.find("[Warning]") != FString::npos)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
			HasColor = true;
		}

		ImGui::TextUnformatted(Log.c_str());

		if (HasColor)
			ImGui::PopStyleColor();
	}

	// AutoScroll이 켜져 있을 때만 새 로그를 따라간다.
	if (AutoScroll)
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::Separator();
}

void ConsoleWindow::DrawInput()
{
	ImGuiInputTextFlags Flags =
		ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_CallbackHistory |
		ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_CallbackEdit;

	if (ImGui::InputText("Input", InputBuf, sizeof(InputBuf), Flags, TextEditCallbackStub, this))
	{
		if (InputBuf[0] != '\0')
		{
			ExecuteCommand(InputBuf);
			InputBuf[0] = '\0';
		}

		// 명령 실행 후 기존 자동완성 목록을 비운다.
		Suggestions.Empty();
		SelectedSuggestion = -1;

		ImGui::SetKeyboardFocusHere(-1);
	}
}

void ConsoleWindow::ExecuteCommand(const char* Input)
{
	FString Command = Input;

	// 같은 명령은 History에서 제거한 뒤 가장 최근 위치에 다시 넣는다.
	History.Remove(Command);
	History.Add(Command);
	HistoryPos = -1;

	// “ ” 형태의 따옴표도 일반 " 로 처리한다.
	NormalizeQuotes(Command);

	if (Command == "HELP")
	{
		ExecuteHelp();
		return;
	}

	if (Command == "CLEAR")
	{
		ClearLogs();
		return;
	}

	if (Command == "HISTORY")
	{
		ExecuteHistory();
		return;
	}

	if (Command.rfind("UE_LOG", 0) == 0)
	{
		if (!ParseUELog(Command))
		{
			AddLog("Syntax Error");
			return;
		}

		ExecuteUELog();
		return;
	}

	if (Command.rfind("TYPE", 0) == 0)
	{
		if (!ParseTypeCompare(Command))
		{
			AddLog("Syntax Error");
			return;
		}
		ExecuteTypeCompare();
		return;
	}

	AddLog("Unknown Command");
	return;

}

void ConsoleWindow::ExecuteHelp()
{
	AddLog("Commands:");
	AddLog("HELP");
	AddLog("CLEAR");
	AddLog("HISTORY");
	AddLog("UE_LOG(\"Message %%d\", Value);");
}

void ConsoleWindow::ExecuteHistory()
{
	for (int i = 0; i < History.Num(); i++)
		AddLog("%d: %s", i, History[i].c_str());
}

void ConsoleWindow::NormalizeQuotes(FString& Command)
{
	const FString LeftQuote = "\xE2\x80\x9C";
	const FString RightQuote = "\xE2\x80\x9D";

	size_t Pos;

	while ((Pos = Command.find(LeftQuote)) != FString::npos)
		Command.replace(Pos, LeftQuote.length(), "\"");

	while ((Pos = Command.find(RightQuote)) != FString::npos)
		Command.replace(Pos, RightQuote.length(), "\"");
}

bool ConsoleWindow::ParseUELog(const FString& Input)
{
	ParsedArguments.Empty();

	const FString Command = "UE_LOG";

	// UE_LOG로 시작하는지 확인.
	if (Input.rfind(Command, 0) != 0)
		return false;

	// 명령 마지막에는 ;가 존재.
	if (Input.empty() || Input.back() != ';')
		return false;

	// UE_LOG와 마지막 ;를 제거.
	FString SubCommand = Input.substr(Command.length(), Input.length() - Command.length() - 1);

	// 남은 부분은 반드시 (...) 형태.
	if (SubCommand.length() < 2 || SubCommand.front() != '(' || SubCommand.back() != ')')
		return false;

	FString Arguments = SubCommand.substr(1, SubCommand.length() - 2);

	if (Arguments.empty())
		return false;

	FString Current;
	bool InQuotes = false;

	for (char Ch : Arguments)
	{
		// 따옴표 안에서는 쉼표도 문자열의 일부로 취급.
		if (Ch == '"')
		{
			InQuotes = !InQuotes;
			Current += Ch;
			continue;
		}

		// 따옴표 밖의 쉼표만 실제 인자 구분자로 사용.
		if (Ch == ',' && !InQuotes)
		{
			if (Current.empty())
				return false;

			ParsedArguments.Add(Current);
			Current.clear();
			continue;
		}

		Current += Ch;
	}

	// 따옴표가 닫히지 않았거나 마지막 인자가 비어 있으면 문법 오류.
	if (InQuotes || Current.empty())
		return false;

	ParsedArguments.Add(Current);

	return true;
}

void ConsoleWindow::ExecuteUELog()
{
	if (ParsedArguments.IsEmpty())
		return;

	FString Format = ParsedArguments[0];

	// 첫 번째 인자는 반드시 "문자열" 형태.
	if (Format.length() < 2 || Format.front() != '"' || Format.back() != '"')
	{
		AddLog("Syntax Error");
		return;
	}

	// 앞뒤 따옴표를 제거.
	Format = Format.substr(1, Format.length() - 2);

	FString Result = Format;

	if (ParsedArguments.Num() >= 2)
	{
		FString Value = ParsedArguments[1];

		// 두 번째 인자 앞뒤 공백을 제거.
		while (!Value.empty() && Value.front() == ' ')
			Value.erase(Value.begin());

		while (!Value.empty() && Value.back() == ' ')
			Value.pop_back();

		// 현재 과제 범위에서는 %d 하나를 두 번째 인자로 치환.
		size_t FormatPos = Result.find("%d");

		if (FormatPos != FString::npos)
			Result.replace(FormatPos, 2, Value);
	}

	AddLog("%s", Result.c_str());
}

bool ConsoleWindow::ParseTypeCompare(const FString& Input)
{
	ParsedArguments.Empty();

	const FString Command = "TYPE";

	// TYPE으로 시작하는지 확인.
	if (Input.rfind(Command, 0) != 0)
		return false;

	// 명령 마지막에는 ;가 존재.
	if (Input.empty() || Input.back() != ';')
		return false;

	// TYPE와 마지막 ;를 제거.
	FString SubCommand = Input.substr(Command.length(), Input.length() - Command.length() - 1);

	// 남은 부분은 반드시 (...) 형태.
	if (SubCommand.length() < 2 || SubCommand.front() != '(' || SubCommand.back() != ')')
		return false;

	FString Arguments = SubCommand.substr(1, SubCommand.length() - 2);

	if (Arguments.empty())
		return false;

	size_t CommaPos = Arguments.find(',');

	if (CommaPos == FString::npos)
		return false;
	
	FString First = Arguments.substr(0, CommaPos);
	FString Second = Arguments.substr(CommaPos + 1);

	First = Trim(First);
	Second = Trim(Second);

	if (First.empty() || Second.empty())
		return false;

	ParsedArguments.Add(First);
	ParsedArguments.Add(Second);

	return true;
}

void ConsoleWindow::ExecuteTypeCompare() {

	if (ParsedArguments.Num() != 2) {
		AddLog("ISA requires exactly 2 arguments.");
		return;
	}

	auto& ClassMap = ObjectManager::GetInstance().AllClassInfoMap;

	FString first = ParsedArguments[0];
	FString second = ParsedArguments[1];

	ClassInfo** FirstClass = ClassMap.Find(first);
	ClassInfo** SecondClass = ClassMap.Find(second);

	if (!FirstClass || !SecondClass)
	{
		AddLog("Class Not Found");
		return;
	}

	bool Result = (*FirstClass)->IsA(*SecondClass);

	FString Message = Result 
		? first + "is a " + second
		: first + "is not a " + second;

	AddLog("%s", Message.c_str());

}

int ConsoleWindow::TextEditCallbackStub(ImGuiInputTextCallbackData* Data)
{
	// ImGui의 static callback에서 현재 ConsoleWindow 객체로 다시 연결.
	return static_cast<ConsoleWindow*>(Data->UserData)->TextEditCallback(Data);
}

int ConsoleWindow::TextEditCallback(ImGuiInputTextCallbackData* Data)
{
	if (Data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
	{
		// 추천 목록이 있으면 ↑↓는 추천 선택에 사용하고, 없으면 History 탐색에 사용.
		if (Suggestions.IsEmpty())
			HandleHistory(Data);
		else
			HandleSuggestionNavigation(Data);
	}
	else if (Data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
	{
		HandleCompletion(Data);
	}
	else if (Data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
	{
		UpdateSuggestions(Data->Buf);
	}

	return 0;
}

void ConsoleWindow::HandleHistory(ImGuiInputTextCallbackData* Data)
{
	if (History.IsEmpty())
		return;

	if (Data->EventKey == ImGuiKey_UpArrow)
	{
		if (HistoryPos == -1)
			HistoryPos = History.Num() - 1;
		else if (HistoryPos > 0)
			HistoryPos--;
	}
	else if (Data->EventKey == ImGuiKey_DownArrow)
	{
		if (HistoryPos != -1)
		{
			HistoryPos++;

			if (HistoryPos >= History.Num())
				HistoryPos = -1;
		}
	}

	FString Command;

	if (HistoryPos >= 0)
		Command = History[HistoryPos];

	// 현재 입력창 내용을 선택한 History 명령으로 교체한다.
	Data->DeleteChars(0, Data->BufTextLen);
	Data->InsertChars(0, Command.c_str());
}

void ConsoleWindow::HandleSuggestionNavigation(ImGuiInputTextCallbackData* Data)
{
	if (Suggestions.IsEmpty())
		return;

	if (Data->EventKey == ImGuiKey_UpArrow)
	{
		if (SelectedSuggestion <= 0)
			SelectedSuggestion = Suggestions.Num() - 1;
		else
			SelectedSuggestion--;
	}
	else if (Data->EventKey == ImGuiKey_DownArrow)
	{
		if (SelectedSuggestion == -1 || SelectedSuggestion >= Suggestions.Num() - 1)
			SelectedSuggestion = 0;
		else
			SelectedSuggestion++;
	}
}

void ConsoleWindow::HandleCompletion(ImGuiInputTextCallbackData* Data)
{
	if (!Suggestions.IsValidIndex(SelectedSuggestion))
		return;

	const FString& Command = Suggestions[SelectedSuggestion];

	// Tab을 누르면 선택된 추천 명령으로 입력창 전체를 교체한다.
	Data->DeleteChars(0, Data->BufTextLen);
	Data->InsertChars(0, Command.c_str());

	Suggestions.Empty();
	SelectedSuggestion = -1;
}

void ConsoleWindow::UpdateSuggestions(const FString& Input)
{
	Suggestions.Empty();
	SelectedSuggestion = -1;

	if (Input.empty())
		return;

	// 현재 입력 문자열로 시작하는 명령어만 추천 목록에 넣는다.
	for (const FString& Command : Commands)
	{
		if (Command.rfind(Input, 0) == 0)
			Suggestions.Add(Command);
	}

	if (!Suggestions.IsEmpty())
		SelectedSuggestion = 0;
}

void ConsoleWindow::DrawSuggestions()
{
	if (Suggestions.IsEmpty())
		return;

	for (int i = 0; i < Suggestions.Num(); i++)
	{
		bool IsSelected = i == SelectedSuggestion;

		// 마우스로 추천 명령을 클릭해도 입력창에 들어간다.
		if (ImGui::Selectable(Suggestions[i].c_str(), IsSelected))
		{
			snprintf(InputBuf, sizeof(InputBuf), "%s", Suggestions[i].c_str());

			Suggestions.Empty();
			SelectedSuggestion = -1;
			break;
		}
	}
}

FString ConsoleWindow::Trim(const FString& Str)
{
	size_t Start = 0;
	size_t End = Str.length();

	while (Start < End && Str[Start] == ' ')
		++Start;

	while (End > Start && Str[End - 1] == ' ')
		--End;

	return Str.substr(Start, End - Start);
}