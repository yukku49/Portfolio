#include "StartScreen.h"
#include"DxLib.h"

const char* string_select[] =
{ "play the game",
	"option",
	"exit",
	NULL
};

void StartScreen::SelectGames()
{
	while (string_select[selectNumber] != nullptr)
	{
		++selectNumber;
	}
	

	//モードに応じた処理をする
    // メンバ関数ポインタの配列に修正
    void (StartScreen::*P_func[])() =
    {
        &StartScreen::Play_the_game,
        &StartScreen::Option,
		& StartScreen::Exit
    };
}

void StartScreen::MoveCursor()
{
	//キーを取得
	int cursorUp = CheckHitKey(KEY_INPUT_UP);
	int cursorDown = CheckHitKey(KEY_INPUT_DOWN);
	int cursorEnter = CheckHitKey(KEY_INPUT_RETURN);
	//カーソルを動かす
	if(cursorUp==1)
	{
		cursorY -= 32;

	}
	if (cursorDown == 1)
	{
		cursorY += 32;
	}
	//EnterKeyが押されたらモードに応じた処理
	if (cursorEnter == 1)
	{
		if (cursorY == 0)Play_the_game();
		else if (cursorY == 1)Option();
		else if (cursorY == 2)Exit();
	}
	//カーソルの位置を制限する
	if (cursorY < 0)
	{
		cursorY = 0;
	}
	if (cursorY > selectNumber)
	{
		cursorY = selectNumber;
	}
	DrawBox(cursorX+32,cursorY+32,(cursorX+1)+32,(cursorY+1) + 32, GetColor(255, 255, 255), true);
}

void StartScreen::Play_the_game()
{
	
	isStartScreenActiveflag = false;
}

void StartScreen::Option()
{
	
}

void StartScreen::Exit()
{
	modeNumber = 3;
	PostQuitMessage(0);
	DxLib_End();
}