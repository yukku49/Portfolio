#include "DrawManagiment.h"
#include "BackScreenManagiment.h"
#include "EnemyManagiment.h"
#include "BllentManagiment.h"
#include "PlayerManagiment.h"
#include "ItemManagiment.h"
#include"StartScreen.h"
#include "DxLib.h"

// Tile drawing unit（タイル描画単位）
static constexpr int TILE_SIZE = 32; // Map タイルサイズ（描画で使う場合のみ）

// Player 描画をピクセル単位に対応させる（player.GetXf()/GetYf() を使用）
void DrawManager::Player_Draw(const BackScreen& stage, const Player_Managiment& player) const
{
	// 画像実サイズ取得
	int w = 1, h = 1;
	GetGraphSize(player.Get_PlayerHandle(), &w, &h);

	const int CHARA_WIDTH = player.Get_PlayerDisplaySize();
	// 比率維持で高さ計算
	int drawH = static_cast<int>(CHARA_WIDTH * (static_cast<float>(h) / w));

	// 描画座標：ピクセル座標を直接使用
	int x1 = static_cast<int>(player.GetXf()) + (TILE_SIZE - CHARA_WIDTH) / 2;
	int y1 = static_cast<int>(player.GetYf()) + (TILE_SIZE - drawH);

	int x2 = x1 + CHARA_WIDTH;
	int y2 = y1 + drawH;

	DrawExtendGraph(x1, y1, x2, y2, player.Get_PlayerHandle(), TRUE);
}

// Draw map and obstacles（マップと障害物を表示）
void DrawManager::Map_Draw(const BackScreen& object)const
{	
	// Full background（背景全体）
	DrawExtendGraph(0, TILE_SIZE, 1280, TILE_SIZE+736, object.Get_Maphandle(), false);

	for (int y = 0; y < object.MAP_Get_SizeY(); y++)
	{
		for (int x = 0; x < object.MAP_Get_SizeX(); x++)
		{
			if (object.GetMapvalue(x, y) == 0)
			{
				DrawExtendGraph(x * TILE_SIZE, y * TILE_SIZE,
                    x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE,
                    object.Get_ObjectHanadle(), true);
			}
		}
	}
}

void DrawManager::Bullets_Draw(const Bllent_Managiment& bullets) const
{
	for (int i = 0; i < bullets.GetMaxBullets(); ++i)
	{
		const auto& b = bullets.Get_Bullethandle(i);
		if (b.isActive && b.using_handle >= 0)
		{
			// 16px 当たり判定の中心に 32px スプライトを合わせる
			const int drawX = static_cast<int>(b.x) - 8;
			const int drawY = static_cast<int>(b.y) + TILE_SIZE - 8;
			DrawExtendGraph(drawX, drawY, drawX + 32, drawY + 32, b.using_handle, true);
		}
	}
}

void DrawManager::Enemy_Draw(const Enemy_Managiment& enemy, const BackScreen& stage) const
{
	// 敵情報を取得
	const auto& e = enemy.Get_enemyPoint();

	// 有効でなければ描画しない
	if (!e.isActive) return;

	// ハンドル取得（Enemy クラスの getter を使用）
	int handle = enemy.Get_EnemyHandle();
	if (handle < 0)
	{
		// 画像未ロードやエラー時はスキップ
		return;
	}

	// 画像実サイズ取得
	int w = 1, h = 1;
	GetGraphSize(enemy.Get_EnemyHandle(), &w, &h);

	const int CHARA_WIDTH = enemy.Get_EnemyDisplaySize();
	// 比率維持で高さ計算
	int drawH = static_cast<int>(CHARA_WIDTH * (static_cast<float>(h) / w));

	// 描画座標：ピクセル座標を直接使用
	int x1 = static_cast<int>(enemy.Get_enemyX()) + (TILE_SIZE - CHARA_WIDTH) / 2;
	int y1 = static_cast<int>(enemy.Get_enemyY()) + (TILE_SIZE - drawH);

	int x2 = x1 + CHARA_WIDTH;
	int y2 = y1 + drawH;

	DrawExtendGraph(x1, y1, x2, y2, enemy.Get_EnemyHandle(), TRUE);
}

//start画面の文字と背景を描画するだけ
void DrawManager::Start_Draw(const StartScreen& start) const
{
	
	
		//枠を描画
		DrawBox(0, 0, 736 - 1, 1280 - 1, GetColor(255, 255, 255), false);
	
}
void DrawManager::HUD_Draw(const Player_Managiment& player, const Enemy_Managiment& enemy) const
{
	//UIバー背景（黒）
	DrawBox(0, 0, 1280, TILE_SIZE, GetColor(30, 30, 30), TRUE);
	//区切り線
	DrawBox(0, TILE_SIZE - 1, 1280, TILE_SIZE, GetColor(80, 80, 80), TRUE);
	//プレイヤー満腹ゲージ（左）
	const int BAR_W = 200, BAR_H = 14;
	const int P_BAR_X = 10, BAR_Y = (TILE_SIZE - BAR_H) / 2;
	//ゲージ枠
	DrawBox(P_BAR_X, BAR_Y, P_BAR_X + BAR_W, BAR_Y + BAR_H, GetColor(80, 80, 80), TRUE);

	//TODO:実際の満腹値をplayerから取得
	//float ratio=player.GetFullness()/player.GetMaxFullness();
	float ratio = player.GetFullness()/Player_Managiment::MAX_FULLNES;
	int fillw = static_cast<int>(BAR_W * ratio);
	DrawBox(P_BAR_X, BAR_Y, P_BAR_X + fillw, BAR_Y + BAR_H, GetColor(255, 180, 0), TRUE);//オレンジ

	DrawFormatString(P_BAR_X + BAR_W + 6, BAR_Y, GetColor(255, 255, 255), "P1");

	//敵の満腹ゲージ
	const int E_BAR_X = 1280 - BAR_W - 10;
	DrawBox(E_BAR_X, BAR_Y, E_BAR_X + BAR_W, BAR_Y + BAR_H, GetColor(80, 80, 80), TRUE);

	//ToDo
	float enemyRatio = 0.3f;
	int eFillW = static_cast<int>(BAR_W * enemyRatio);
	DrawBox(E_BAR_X, BAR_Y, E_BAR_X + eFillW, BAR_Y + BAR_H, GetColor(220, 60, 60), TRUE);

	DrawFormatString(E_BAR_X - 30, BAR_Y, GetColor(255, 255, 255), "CPU");

	//所持アイテム数（中央）
	auto items = player.Get_Player_Itembring();
	DrawFormatString(480, BAR_Y, GetColor(200, 200, 200), "生地:%d トマト:%d ゴルゴン:%d バジル:%d", items.Pizzadough_Counter, items.Tmato_Counter,
		items.Cheese_Counter, items.Basil_Counter);
};

void DrawManager::Item_Draw(const Item_Managiment& item, const BackScreen& stage) const
{
	for (int i = 0; i < item.Get_Item_number(); i++)
	{
		const ItemData* it = item.Get_Items(i);
		if (!it->isActive) continue;
		int x1 = static_cast<int>(it->x * TILE_SIZE);
		int y1 = static_cast<int>(it->y * TILE_SIZE)+TILE_SIZE;
		int x2 = x1 + TILE_SIZE;
		int y2 = y1 + TILE_SIZE;
		DrawExtendGraph(x1, y1, x2, y2, item.Get_ItemHandle(it->type), TRUE);
	}
}


