#include "DrawManagiment.h"
#include "BackScreenManagiment.h"
#include "EnemyManagiment.h"
#include "BllentManagiment.h"
#include "PlayerManagiment.h"
#include "ItemManagiment.h"
#include "StartScreen.h"
#include "DxLib.h"

// Shared tile size constant used for all coordinate conversions in this file
// (JP: このファイル全体の座標変換に使う共通タイルサイズ定数)
static constexpr int TILE_SIZE = 32;

// Draws the player sprite centered horizontally within its tile
// Steps:
//   1. Query the actual image size to calculate the correct aspect-ratio height
//   2. Offset x so the sprite is centered within TILE_SIZE pixels
//   3. Offset y so the bottom of the sprite sits on the tile floor
// (JP: プレイヤースプライトをタイル内で水平中央に揃えて描画する)
// (JP: 手順: 1.実画像サイズから正しい縦横比の高さを計算 2.x座標をTILE_SIZE内で中央揃え 3.y座標をタイル底辺に合わせる)
void DrawManager::Player_Draw(const BackScreen& stage, const Player_Managiment& player) const
{
    // Get the actual pixel dimensions of the loaded sprite
    // (JP: ロードしたスプライトの実ピクセル寸法を取得する)
    int w = 1, h = 1;
    GetGraphSize(player.Get_PlayerHandle(), &w, &h);
    const int CHARA_WIDTH = player.Get_PlayerDisplaySize();

    // Scale height proportionally so the sprite is not stretched
// (JP: スプライトが引き伸ばされないよう高さを比例スケールする)
    int drawH = static_cast<int>(CHARA_WIDTH * (static_cast<float>(h) / w));

    int x1 = static_cast<int>(player.GetXf()) + (TILE_SIZE - CHARA_WIDTH) / 2;
    int y1 = static_cast<int>(player.GetYf()) + (TILE_SIZE - drawH);
    int x2 = x1 + CHARA_WIDTH;
    int y2 = y1 + drawH;

  

    if (player.GetPInvincibleTimer() > 0.0f && (player.GetMPblickTimer() / 4)%2 == 1)
    {
        return;
    }
    DrawExtendGraph(x1, y1, x2, y2, player.Get_PlayerHandle(), TRUE);
}

// Draws the full stage:
//   1. Stretch the background image to fill the play area (below the HUD)
//   2. Loop over every tile and draw an obstacle sprite on top where value == 0
// The +TILE_SIZE on the Y axis accounts for the 32px HUD bar at the top
// (JP: ステージ全体を描画する)
// (JP: 1.背景画像をプレイエリア(HUD下)に引き伸ばす 2.全タイルをループし値が0の場所に障害物スプライトを重ねる)
// (JP: Y軸の+TILE_SIZEは上部32pxのHUDバー分のオフセット)
void DrawManager::Map_Draw(const BackScreen& object) const
{
    // Draw the background image stretched across the play area
    // Y starts at TILE_SIZE to leave room for the HUD bar
    // (JP: 背景画像をプレイエリアに引き伸ばして描画する。YはHUDバーの分だけTILE_SIZEから始まる)
    DrawExtendGraph(0, TILE_SIZE, 1280, TILE_SIZE + 736, object.Get_Maphandle(), false);

        // Overlay obstacle sprites on every wall tile (value == 0)
        // (JP: 壁タイル(値==0)すべてに障害物スプライトを重ねて描画する)
        for (int y = 0; y < object.MAP_Get_SizeY(); y++)
        {
            for (int x = 0; x < object.MAP_Get_SizeX(); x++)
            {
                if (object.GetMapvalue(x, y) == 0)
                {
                 // +TILE_SIZE on Y offsets below the HUD bar
                // (JP: YへのTILE_SIZE加算でHUDバー下にオフセットする)
                DrawExtendGraph(x * TILE_SIZE, y * TILE_SIZE+TILE_SIZE,
                    x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE+TILE_SIZE,                 
                    object.Get_ObjectHanadle(), true);

                }
            }
        }
}
        
// Iterates the entire bullet pool and draws each active bullet
// The sprite is offset by -8px on both axes so the bullet image is visually centered
// on the bullet's logical position (the logical size is 16px, the drawn size is 32px)
// (JP: 弾プール全体を走査し、アクティブな弾を描画する)
// (JP: スプライトは両軸で-8pxオフセットし、論理位置(16px)に対して描画(32px)を視覚的に中央揃えにする)
void DrawManager::Bullets_Draw(const Bllent_Managiment& bullets) const
{
    for (int i = 0; i < bullets.GetMaxBullets(); ++i)
    {
        const auto& b = bullets.Get_Bullethandle(i);
        if (b.isActive && b.using_handle >= 0)
        {
            // Center the 32px sprite around the 16px logical hit-box
            // (JP: 16px論理ヒットボックスの周囲に32pxスプライトを中央揃えする)
            const int drawX = static_cast<int>(b.x) - 8;
            const int drawY = static_cast<int>(b.y) + - 8;
            DrawExtendGraph(drawX, drawY, drawX + 32, drawY + 32, b.using_handle, true);
        }
    }
}

// Draws the enemy sprite with the same centering logic as Player_Draw
// Returns immediately if the enemy is not active (already defeated)
// (JP: Player_Drawと同じ中央揃えロジックで敵スプライトを描画する)
// (JP: 敵が非アクティブ(撃破済み)の場合はすぐに返る)
void DrawManager::Enemy_Draw(const Enemy_Managiment& enemy, const BackScreen& stage) const
{
    const auto& e = enemy.Get_enemyPoint();
    if (!e.isActive) return;

    int handle = enemy.Get_EnemyHandle();
    if (handle < 0) return;

    const int CHARA_WIDTH = 38;
  

    // ★ 現在の向きの画像サイズから drawH を計算（Player_Draw と同じロジック）
    int w = 1, h = 1;
    GetGraphSize(handle, &w, &h);
    int drawH = 71;

    int x1 = static_cast<int>(enemy.Get_enemyX()) + (TILE_SIZE - CHARA_WIDTH) / 2;
    int y1 = static_cast<int>(enemy.Get_enemyY()) + (TILE_SIZE - drawH);
    int x2 = x1 + CHARA_WIDTH;
    int y2 = y1 + drawH;

    if (enemy.GetInvincibleTimer() > 0.0f && (enemy.GetBlinkTimer() / 4) % 2 == 1)
        return;

    DrawExtendGraph(x1, y1, x2, y2, handle, TRUE);
}

// Placeholder: draws a white outline box as a stand-in for the real start screen art
// Replace with actual menu graphics when the start screen design is finalized
// (JP: 仮実装。実際のスタート画面アートの代わりに白い枠を描画する)
// (JP: スタート画面のデザインが確定したら実際のメニューグラフィックに差し替える)
void DrawManager::Start_Draw(const StartScreen& start) const
{
    DrawBox(0, 0, 736 - 1, 1280 - 1, GetColor(255, 255, 255), false);
}

// Draws the HUD bar across the top 32px of the screen
// Layout (left to right):
//   [P1 fullness gauge] ... [item counts] ... [CPU fullness gauge]
// The enemy gauge ratio is currently hardcoded; replace with enemy.GetFullness() later
// (JP: 画面上部32pxにHUDバーを描画する)
// (JP: レイアウト左から右: [P1満腹ゲージ]...[アイテム所持数]...[CPU満腹ゲージ])
// (JP: 敵ゲージの比率は現在ハードコード。後でenemy.GetFullness()に差し替える)
void DrawManager::HUD_Draw(const Player_Managiment& player, const Enemy_Managiment& enemy) const
{
    // Dark background bar and a thin separator line at the bottom edge
    // (JP: 暗い背景バーと下端の細い区切り線)
    DrawBox(0, 0, 1280, TILE_SIZE, GetColor(30, 30, 30), TRUE);
    DrawBox(0, TILE_SIZE - 1, 1280, TILE_SIZE, GetColor(80, 80, 80), TRUE);

    // --- Player fullness gauge (left side, orange fill) ---
    // (JP: プレイヤー満腹ゲージ（左側、オレンジ塗り）)
    const int BAR_W = 200, BAR_H = 14;
    const int P_BAR_X = 10, BAR_Y = (TILE_SIZE - BAR_H) / 2;

    DrawBox(P_BAR_X, BAR_Y, P_BAR_X + BAR_W, BAR_Y + BAR_H, GetColor(80, 80, 80), TRUE);

    // Calculate fill width from current fullness / max fullness ratio
    // (JP: 現在の満腹値/最大満腹値の比率から塗り幅を計算する)
    float ratio = player.GetFullness() / Player_Managiment::MAX_FULLNES;
    int fillW = static_cast<int>(BAR_W * ratio);
    DrawBox(P_BAR_X, BAR_Y, P_BAR_X + fillW, BAR_Y + BAR_H, GetColor(255, 180, 0), TRUE);

    DrawFormatString(P_BAR_X + BAR_W + 6, BAR_Y, GetColor(255, 255, 255), "P1");

   

    /// --- Item inventory counts (center) ---
    // Displays how many of each ingredient the player is currently holding
    // (JP: アイテム所持数（中央）。プレイヤーが現在持っている各食材の数を表示する)
    auto items = player.Get_Player_Itembring();
    DrawFormatString(380, BAR_Y, GetColor(200, 200, 200),
        "Dough:%d Tomato:%d Cheese:%d Gorg:%d Basil:%d",
        items.Pizzadough_Counter,
        items.Tmato_Counter,
        items.Cheese_Counter,
        items.Gorgonzola_Counter,
        items.Basil_Counter);

    
}

// Draws every active item at its tile grid position
// Tile coordinates are multiplied by TILE_SIZE and shifted down by TILE_SIZE (HUD offset)
// (JP: アクティブなアイテムをタイルグリッド座標に描画する)
// (JP: タイル座標にTILE_SIZEを掛け、HUDオフセット分TILE_SIZEだけ下にずらす)
void DrawManager::Item_Draw(const Item_Managiment& item, const BackScreen& stage) const
{
    for (int i = 0; i < item.Get_Item_number(); i++)
    {
        const ItemData* it = item.Get_Items(i);
        if (!it->isActive) continue;

        int x1 = it->x * TILE_SIZE;
        int y1 = it->y * TILE_SIZE + TILE_SIZE;// +TILE_SIZE for HUD offset (JP: HUDオフセット分加算)
        int x2 = x1 + TILE_SIZE;
        int y2 = y1 + TILE_SIZE;

        DrawExtendGraph(x1, y1, x2, y2, item.Get_ItemHandle(it->type), TRUE);
    }
}

// Draws a semi-transparent overlay with WIN or LOSE message and final score
// Called from the main loop once a win condition is detected
// (JP: 半透明オーバーレイにWIN/LOSEメッセージと最終スコアを描画する)
// (JP: 勝利条件が検出されたらメインループから呼ぶ)
void DrawManager::Result_Draw(bool playerWin) const
{
    // Semi-transparent dark overlay over the whole screen
    // (JP: 画面全体への半透明の暗いオーバーレイ)
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
    DrawBox(0, 0, 1280, 768, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (playerWin)
    {
        // Player wins: show golden WIN text
        // (JP: プレイヤー勝利：金色のWINテキストを表示)
        DrawFormatString(520, 300, GetColor(255, 220, 0), "YOU WIN!");
    }
    else
    {
        // Enemy wins: show red LOSE text
        // (JP: 敵勝利：赤いLOSEテキストを表示)
        DrawFormatString(500, 300, GetColor(255, 60, 60), "YOU LOSE...");
    }

    // Prompt to return to the start screen (not yet implemented)
    // (JP: スタート画面に戻る案内（未実装）)
    DrawFormatString(460, 380, GetColor(200, 200, 200),
        "Press ENTER to return to title");
}