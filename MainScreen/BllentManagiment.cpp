#include "BllentManagiment.h"
#include"PlayerManagiment.h"
#include"BackScreenManagiment.h"
#include<iostream>
#include"DxLib.h"
#include<Windows.h>
#include"EnemyManagiment.h"

void Bllent_Managiment::Load()
{
	bllet_Handle[GENOVESE] = LoadGraph("../Pizza_Image/Genovese.png");
	bllet_Handle[MARGHERITA] = LoadGraph("../Pizza_Image/margherita.png");
	bllet_Handle[MARINARA] = LoadGraph("../Pizza_Image/marinara.png");
	bllet_Handle[QUATTROFORMAGGI] = LoadGraph("../Pizza_Image/quattro_formaggi.png");
	bllet_Handle[SEAGE] = LoadGraph("../Pizza_Image/sage.png");
	//画像ロードチェック
	for (int i = 0; i < BLLET_MAX; i++)
	{
		if (bllet_Handle[i] == -1)
		{
			char buf[256];
			sprintf_s(buf, "Bullet image load failed: index=%d", i);
			OutputDebugStringA(buf);
		}
	}
	//弾配列を必ず初期化
	for (int i = 0; i < Max_Bullets; i++)
	{
		m_bullets[i].x = 0.0f;
		m_bullets[i].y = 0.0f;
		m_bullets[i].vx = 0.0f;
		m_bullets[i].vy = 0.0f;
		m_bullets[i].using_handle = -1;
		m_bullets[i].timer = 0;
		m_bullets[i].isActive = false;
	}
	now_bllet_Handle = bllet_Handle[SEAGE];
	
}
void Bllent_Managiment::Update (BackScreen& stage, Player_Managiment& player, Enemy_Managiment& enemy)
{
	for (int i = 0; i < Max_Bullets; i++)
	{
		if (!m_bullets[i].isActive)					
			continue;

		// Apply velocity (JP: 速度を反映)
		m_bullets[i].x += m_bullets[i].vx;
		m_bullets[i].y += m_bullets[i].vy;

		// Remove if out of screen (JP: 画面外なら無効化)
		if (m_bullets[i].x < -32 || m_bullets[i].x > 1312 ||
			m_bullets[i].y < -32 || m_bullets[i].y > 768)
		{
			m_bullets[i].isActive = false;
			continue;
		}

		// Map size note X:40 Y:23 (JP: マップサイズのメモ X:40 Y:23)
		// Check collision with map walls (JP: マップの壁との衝突判定)
		if (stage.CheckCollision(m_bullets[i].x, m_bullets[i].y) ||
			stage.CheckCollision(m_bullets[i].x + 16, m_bullets[i].y + 16))
		{
			m_bullets[i].isActive = false;
			continue;
		}

		// --- 敵との当たり判定（AABB 簡易判定） ---
		if (enemy.Get_EnemyActive())
		{
			float ex = enemy.Get_enemyX();
			float ey = enemy.Get_enemyY();
			int esz = enemy.Get_EnemyDisplaySize(); // 敵の描画サイズ（当たり判定に利用）
			const int BULLET_SIZE = 16; // 弾の当たり大きさ（必要に応じ調整）

			bool hit = !(m_bullets[i].x + BULLET_SIZE < ex ||
						 m_bullets[i].x > ex + esz ||
						 m_bullets[i].y + BULLET_SIZE < ey ||
						 m_bullets[i].y > ey + esz);

			if (hit)
			{
				// 弾と敵を無効化（もしくは enemy.OnHit() でHP減少など）
				m_bullets[i].isActive = false;
				enemy.OnHit();
				continue;
			}
		}
	}

}

// Select bullet type from held ingredients, fallback to sage (JP: 所持材料で弾の種類を選び、なければセージ)
void Bllent_Managiment::Shot(float x, float y, Player_Managiment& player)
{

	auto item = player.Get_Player_Itembring();
	int tomato = item.Tmato_Counter;
	int Cheese = item.Cheese_Counter;
	int basil = item.Basil_Counter;
	int dough = item.Pizzadough_Counter;

	PizzaType pizza = player.TryMakePizza();
	switch (pizza)
	{
	case PizzaType::Margherita:      now_bllet_Handle = bllet_Handle[MARGHERITA];      break;
	case PizzaType::QuattroFormaggi: now_bllet_Handle = bllet_Handle[QUATTROFORMAGGI]; break;
	case PizzaType::Marinara:        now_bllet_Handle = bllet_Handle[MARINARA];        break;
	default:                         now_bllet_Handle = bllet_Handle[SEAGE];           break;
	}

	if (tomato > 0 && Cheese > 0 && basil > 0 && dough > 0) {
		now_bllet_Handle = bllet_Handle[MARGHERITA];
	}
	// Quattro formaggi (JP: クアトロフォルマッジ)
	else if ( tomato> 0 && Cheese> 0 && dough > 0)
	{
		now_bllet_Handle = bllet_Handle[QUATTROFORMAGGI];
	}
	// Genovese (JP: ジェノベーゼ)
	else if (basil > 0 && dough > 0 && Cheese > 0 && tomato > 0)
	{
		now_bllet_Handle = bllet_Handle[GENOVESE];
	}
	// Marinara (JP: マリナーラ)
	else if (tomato > 0 && dough > 0 && basil > 0)
	{
		now_bllet_Handle = bllet_Handle[MARINARA];
	}
	// Default to sage shot (JP: デフォルトはセージ弾)
	else
	{
		now_bllet_Handle = bllet_Handle[SEAGE];
	}
	// Find inactive bullet slot (JP: 未使用の弾スロットを探す)
	for (int i=0;i<Max_Bullets; i++)
	{
		// Use first available slot (JP: 最初の空きスロットを使う)
		if (!m_bullets[i].isActive)
		{
			m_bullets[i].isActive = true;
			m_bullets[i].x = player.GetX()*32.0;
			m_bullets[i].y = player.GetY()*32.0;
			m_bullets[i].using_handle = now_bllet_Handle;
			// Set velocity by player direction (JP: プレイヤーの向きで速度を設定)
			m_bullets[i].vx = 0;
			m_bullets[i].vy = 0;
			// Up (JP: 上)
			if (player.GetDir() == PlayerEye_Up)
			{
				m_bullets[i].vy = -5.0f;
				m_bullets[i].vx = 0;
				break;
			}
			// Down (JP: 下)
			else if (player.GetDir() ==PlayerEye_Down)
			{
				m_bullets[i].vx = 0;
				m_bullets[i].vy = 5.0f;
				break;
			}
			// Left (JP: 左)
			else if (player.GetDir() == PlayerEye_Left)
			{
				m_bullets[i].vx = -5.0f;
				m_bullets[i].vy = 0;
				break;
			}
			// Right (JP: 右)
			else if (player.GetDir() == PlayerEye_Right)
			{
				m_bullets[i].vx = 5.0f;
				m_bullets[i].vy = 0;
				break;
			}
		}
		printf("Shot: slot=%d x=%f y=%f handle=%d\n", i, m_bullets[i].x, m_bullets[i].y, m_bullets[i].using_handle);
	}
}

