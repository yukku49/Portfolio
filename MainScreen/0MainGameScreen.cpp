#include <iostream>
#include"DxLib.h"
#include"sqlite_DB/sqlite-amalgamation-3530100/sqlite3.h"
#include"BackScreenManagiment.h"
#include"BllentManagiment.h"
#include"DrawManagiment.h"
#include"PlayerManagiment.h"
#include"EnemyManagiment.h"
#include"ItemManagiment.h"
#include"StartScreen.h"

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
    static constexpr int TILE_SIZE = 32;

    // Create manager instances (JP: マネジャーインスタンスの作成)
    Player_Managiment player;
    Item_Managiment item;
    Enemy_Managiment enemy;
    Bllent_Managiment bllet;
    DrawManager draw;
    BackScreen stage;
    StartScreen start;

    // Initialize game objects (JP: ゲームオブジェクトの初期化)
    player.Initialisation();
    item.ItemManagiment();
    // Enemy_Initialisation expects pixel coordinates now -> convert from tile coords
    enemy.Enemy_Initialisation(38 * TILE_SIZE, 21 * TILE_SIZE);
    bllet.Load();
    stage.Initialize();
    item.Load();
    for (int i = 0; i < (int)ITEM_MAX; i++)
    {
        item.Spawn(stage, static_cast<Item_number>(i));
    }
    // Start main loop (JP: メインループ)
    while (1)
    {
    
        if (ProcessMessage() != 0) break;
        ClearDrawScreen();

        if (start.Get_Start_Flog())
        {
            //draw.Start_Draw(start);
            //start.SelectGames();
            start.MoveCursor();

        }

        // 更新
        player.Update(stage, bllet);
        bllet.Update(stage, player, enemy);
        // 当たり判定を行う更新を呼ぶ（BackScreen を渡す）
        enemy.Enemy_Update(stage);

        item.Updata();
        Item_number picked = item.CheckPickUp(player);
        if (picked != ITEM_MAX)
        {
            player.Player_BringItem(item);
        }
        static int spawnTimer = 0;
        spawnTimer++;
        if (spawnTimer >= 120)
        {
            spawnTimer = 0;
            item.Spawn(stage, static_cast<Item_number>(GetRand(ITEM_MAX - 1)));
        }
        // 描画
        draw.Map_Draw(stage);
        draw.Player_Draw(stage, player);
        draw.Enemy_Draw(enemy, stage);
        draw.Bullets_Draw(bllet);
        draw.Item_Draw(item, stage);
    
       
        // screen flip
        ScreenFlip();
    }

    // Finalize DxLib (JP: )

    DxLib_End();

    // End application (JP: ??????????)
    return 0;
}
