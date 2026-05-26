#include <iostream>
#include"DxLib.h"
#include"BackScreenManagiment.h"
#include"BllentManagiment.h"
#include"DrawManagiment.h"
#include"PlayerManagiment.h"
#include"EnemyManagiment.h"
#include"ItemManagiment.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    // Enable windowed mode (JP: )
    ChangeWindowMode(true);
    // Set window size (JP: ???????????)
    SetGraphMode(1280, 736, 32);
    // Initialize DxLib (JP: DxLib????)
    if (DxLib_Init() < 0)
    {
        return -1;
    }

    // Tile size used for conversions
    const int TILE_SIZE = 32;

    // Create manager instances (JP: マネジャーインスタンスの作成)
    Player_Managiment player;
    Item_Managiment item;
    Enemy_Managiment enemy;
    Bllent_Managiment bllet;
    DrawManager draw;
    BackScreen stage;

    // Initialize game objects (JP: ゲームオブジェクトの初期化)
    player.Initialisation();
   // item.ItemManagiment();
    // Enemy_Initialisation expects pixel coordinates now -> convert from tile coords
    enemy.Enemy_Initialisation(38 * TILE_SIZE, 21 * TILE_SIZE);
    bllet.Load();
    stage.Initialize();
    //item.Spawn(stage, static_cast<Item_number>(GetRand(item.Get_Item_number() - 1)));
    // Start main loop (JP: メインループ)
    while (1)
    {
        if (ProcessMessage() != 0) break;
        ClearDrawScreen();

        // 更新
        player.Update(stage, bllet);
        bllet.Update(stage, player);
        // 当たり判定を行う更新を呼ぶ（BackScreen を渡す）
        enemy.Enemy_Update(stage);
		
        item.Updata();

        // 描画
        draw.Map_Draw(stage);
        draw.Player_Draw(stage, player);
        draw.Enemy_Draw(enemy, stage);
        draw.Bullets_Draw(bllet);

        // screen flip
        ScreenFlip();
    }

    // Finalize DxLib (JP: )
    DxLib_End();

    // End application (JP: ??????????)
    return 0;
}
