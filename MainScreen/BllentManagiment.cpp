#include "BllentManagiment.h"
#include "DatabaseManagiment.h"
#include "DxLib.h"
#include <Windows.h>

static constexpr int BULLET_HIT_SIZE = 16;
static constexpr float BULLET_SPEED = 300.0f; // px/s（旧 5px/frame @60fps 相当）

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
		m_bullets[i].damage = 1;
		m_bullets[i].isActive = false;
		m_bullets[i].isEnemyBullet = false;
	}
	now_bllet_Handle = bllet_Handle[SEAGE];
	m_lastTime = GetNowCount();
}
void Bllent_Managiment::Update(BackScreen& stage, Player_Managiment& player, Enemy_Managiment& enemy)
{
	unsigned int now = GetNowCount();
	float dt = (now - m_lastTime) / 1000.0f;
	if (dt > 0.1f) dt = 0.1f;
	m_lastTime = now;

	for (int i = 0; i < Max_Bullets; i++)
	{
		if (!m_bullets[i].isActive)
			continue;

		// Apply velocity (JP: 速度を反映)
		m_bullets[i].x += m_bullets[i].vx * dt;
		m_bullets[i].y += m_bullets[i].vy * dt;

		// Remove if out of screen (JP: 画面外なら無効化)
		if (m_bullets[i].x < -32 || m_bullets[i].x > 1312 ||
			m_bullets[i].y < -32 || m_bullets[i].y > 768)
		{
			m_bullets[i].isActive = false;
			continue;
		}

		// Check collision with map walls — 四隅 (JP: マップの壁との衝突判定)
		const float bx = m_bullets[i].x;
		const float by = m_bullets[i].y;
		if (stage.CheckCollision(bx, by) ||
			stage.CheckCollision(bx + BULLET_HIT_SIZE - 1, by) ||
			stage.CheckCollision(bx, by + BULLET_HIT_SIZE - 1) ||
			stage.CheckCollision(bx + BULLET_HIT_SIZE - 1, by + BULLET_HIT_SIZE - 1))
		{
			m_bullets[i].isActive = false;
			continue;
		}

		// --- 敵との当たり判定（AABB 簡易判定） ---
		if (!m_bullets[i].isEnemyBullet&&enemy.Get_EnemyActive())
		{
			float ex = enemy.Get_enemyX();
			float ey = enemy.Get_enemyY();
			int esz = enemy.Get_EnemyDisplaySize(); // 28px

			// 描画オフセットを当たり判定にも反映する
			float hitOffsetX = (32 - esz) / 2.0f;
			float hitOffsetY = (32 - esz);  // DrawEnemy と同じ計算

			bool hit = !(m_bullets[i].x + BULLET_HIT_SIZE < ex + hitOffsetX ||
				m_bullets[i].x > ex + hitOffsetX + esz ||
				m_bullets[i].y + BULLET_HIT_SIZE < ey + hitOffsetY ||
				m_bullets[i].y > ey + hitOffsetY + esz);

			if (hit)
			{
				// 弾と敵を無効化（もしくは enemy.OnHit() でHP減少など）
				m_bullets[i].isActive = false;
				enemy.OnHit();
				continue;
			}
		}
		// --- プレイヤーとの当たり判定（敵弾のみ）---
		if (m_bullets[i].isEnemyBullet)
		{
			float px = player.GetXf();
			float py = player.GetYf();
			int psz = player.Get_PlayerDisplaySize(); // 28px

			bool hit = !(m_bullets[i].x + BULLET_HIT_SIZE < px ||
				m_bullets[i].x > px + psz ||
				m_bullets[i].y + BULLET_HIT_SIZE < py ||
				m_bullets[i].y > py + psz);

			if (hit)
			{
				m_bullets[i].isActive = false;
				player.PlayerOnHit();
				player.GetFullness();
				continue;
			}
		}
	}

}

static int HandleForPizzaType(PizzaType type)
{
	switch (type)
	{
	case PizzaType::Margherita:      return MARGHERITA;
	case PizzaType::QuattroFormaggi: return QUATTROFORMAGGI;
	case PizzaType::Marinara:        return MARINARA;
	case PizzaType::Genovese:        return GENOVESE;
	default:                         return SEAGE;
	}
}

void Bllent_Managiment::Shot(Player_Managiment& player)
{
	const auto& timer = player.GetPizzaTimer();
	PizzaType pizzaType = PizzaType::Sage;

	if (timer.Marigherita > 0.0f)
		pizzaType = PizzaType::Margherita;
	else if (timer.QuattroFormaggi > 0.0f)
		pizzaType = PizzaType::QuattroFormaggi;
	else if (timer.Marinara > 0.0f)
		pizzaType = PizzaType::Marinara;
	else if (timer.Genovese > 0.0f)
		pizzaType = PizzaType::Genovese;
	else
		pizzaType = player.TryMakePizza();

	const int handleIndex = HandleForPizzaType(pizzaType);
	now_bllet_Handle = bllet_Handle[handleIndex];
	const int damage = (m_db && m_db->IsInitialized())
		? m_db->GetPizzaDamage(pizzaType)
		: 1;


	// Find inactive bullet slot (JP: 未使用の弾スロットを探す)
	const int displaySize = player.Get_PlayerDisplaySize();
	const float spawnX = player.GetXf() + (displaySize - BULLET_HIT_SIZE) / 2.0f;
	const float spawnY = player.GetYf() + (displaySize - BULLET_HIT_SIZE) / 2.0f;

	for (int i = 0; i < Max_Bullets; i++)
	{
		if (!m_bullets[i].isActive)
		{
			m_bullets[i].isActive = true;
			m_bullets[i].x = spawnX;
			m_bullets[i].y = spawnY;
			m_bullets[i].using_handle = now_bllet_Handle;
			m_bullets[i].damage = damage;
			m_bullets[i].vx = 0.0f;
			m_bullets[i].vy = 0.0f;
			m_bullets[i].isEnemyBullet = false;

			switch (player.GetDir())
			{
			case PlayerEye_Up:    m_bullets[i].vy = -BULLET_SPEED; break;
			case PlayerEye_Down:  m_bullets[i].vy =  BULLET_SPEED; break;
			case PlayerEye_Left:  m_bullets[i].vx = -BULLET_SPEED; break;
			case PlayerEye_Right: m_bullets[i].vx =  BULLET_SPEED; break;
			}
			return;
		}
	}
	OutputDebugStringA("Bullet pool exhausted: no free slot\n");
}

void Bllent_Managiment::EnemyShot(float spawnX, float spawnY, float vx, float vy)
{
	for (int i = 0; i < Max_Bullets; i++)
	{
		if (!m_bullets[i].isActive)
		{
			m_bullets[i].isActive = true;
			m_bullets[i].x = spawnX;
			m_bullets[i].y = spawnY;
			m_bullets[i].vx = vx;
			m_bullets[i].vy = vy;
			m_bullets[i].using_handle = bllet_Handle[SEAGE]; // 敵弾はSEAGE画像
			m_bullets[i].damage = 1;
			m_bullets[i].isEnemyBullet = true;
			return;
		}
	}
}


