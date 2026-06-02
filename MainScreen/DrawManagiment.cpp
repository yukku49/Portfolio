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
	DrawExtendGraph(0, 0, 1280, 736, object.Get_Maphandle(), false);

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
		if (b.isActive&&b.using_handle>=0)
		{
			DrawExtendGraph(static_cast<int>(b.x), static_cast<int>(b.y),static_cast<int>(b.x)+32,static_cast<int>(b.y)+32, b.using_handle, true);
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
	DrawBox(0, 0, TILE_SIZE-1, TILE_SIZE-1, GetColor(255, 255, 255), false);
}

void DrawManager::Item_Draw(const Item_Managiment& item, const BackScreen& stage) const
{
	for (int i = 0; i < item.Get_Item_number(); i++)
	{
		const ItemData* it = item.Get_Items(i);
		if (!it->isActive) continue;
		int x1 = static_cast<int>(it->x * TILE_SIZE);
		int y1 = static_cast<int>(it->y * TILE_SIZE);
		int x2 = x1 + TILE_SIZE;
		int y2 = y1 + TILE_SIZE;
		DrawExtendGraph(x1, y1, x2, y2, item.Get_ItemHandle(it->type), TRUE);
	}
}


