#include<ConsoleWindow.h>

ConsoleWindow::ConsoleWindow(){
	HistoryPos = -1;
	AutoScroll = false;
	ScrollToBottom = false;

	InputBuf[0] = '\0';

	Commands.push_back("Clear");
	ScrollToBottom = true;
};

ConsoleWindow::~ConsoleWindow() {

}

void ConsoleWindow::AddLogs(const char*Format, ...) {
	char Buf[1024];

	va_list Args;
	va_start(Args, Format);

	vsnprintf(Buf, sizeof(Buf), Format, Args);

	va_end(Args);

	Logs.push_back(Buf);
}

void ConsoleWindow::ClearLogs() {
	Logs.clear();
}

void ConsoleWindow::DrawConsole() {
	
	ImGui::Begin("Console");

	Filter.Draw("Filter");                //필터 부분
	ImGui::Separator();

	ImGui::BeginChild("ScrollingRegion"); //로그 부분

	for (const FString& Log : Logs)
	{
		if (Filter.PassFilter(Log.c_str()))
		{
			ImGui::TextUnformatted(Log.c_str());
		}
	}

	// 새 로그가 생길 때, 자동스크롤
	if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::EndChild();                    // ~로그 부분
	ImGui::Separator();


	if (ImGui::InputText(                 // 입력 부분
		"Input",
		InputBuf,
		sizeof(InputBuf),
		ImGuiInputTextFlags_EnterReturnsTrue)) 
	{
		if (InputBuf[0] != '\0')
		{
			Executor(InputBuf);
		}
		InputBuf[0] = '\0';
	}

	ImGui::End();                                 
}