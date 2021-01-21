//=============================================================================
// Main.cpp
// エントリーポイント
//=============================================================================
#include "ModelViewer.h"
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif

// アプリケーションのエントリーポイント
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリーリークの検出
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Game game;
	// アプリケーションの起動設定
	game.Initialize(L"Gameタイトル", 640, 480);
	// メッセージ ループを実行
	return game.Run();
}
