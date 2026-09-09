
#include "pch.h"
#include "GameManager.h"
#include "FConstants.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// COM 라이브러리 초기화
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	srand(static_cast<unsigned>(time(nullptr)));

	GameManager gm;
	gm.Init(hInstance);
	bool bIsExit = false;
	MSG msg;

	while (!bIsExit)
	{


		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bIsExit = true;
				break;
			}


		}

		gm.mainLoop();
	}

	// COM 라이브러리 해제
	CoUninitialize();

	return 0;
}