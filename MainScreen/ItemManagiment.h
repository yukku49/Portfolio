#pragma once
#include "DxLib.h"
#include"PlayerManagiment.h"
// Enum for item types (JP: アイテム種別の列挙)
enum Item_number
{
	TOMATO,
	BASIL,
	CHEESE,
	GORGONZOLA,
	PIZZADOUGH,
	ITEM_MAX
};
struct ItemData
{
	int x, y;// Grid position on stage (JP: ステージ上のマス座標)
	Item_number type;
	int aliveTimer;// Despawn timer (JP: 消滅までのタイマー)
	bool isActive;
};



class Item_Managiment
{

private:
	// Item image handle array (JP: アイテム画像ハンドル配列)
	int Item_Handle[ITEM_MAX];
	// Maximum simultaneous spawned items (JP: 同時に存在できる最大数)
	static const int MAX_SPAWN = 5;
	// Active item instances (JP: 有効なアイテム実体)
	ItemData m_items[MAX_SPAWN];
public:
	// Initialize item states (JP: アイテム状態の初期化)
	void ItemManagiment();

	// Load item images (JP: アイテム画像の読み込み)
	void Load();

	// Spawn item at random valid tile (JP: 有効マスにランダム配置)
	void Spawn(const BackScreen& stage, Item_number type);

	// Update item timers (JP: アイテムタイマーの更新)
	void Updata();

	// Check player pickup and return item type (JP: プレイヤー取得判定と種別返却)
	Item_number CheckPickUp(Player_Managiment& player);

	// Getter for item enum max (JP: 列挙の最大値取得)
	Item_number Get_Item_number()const{ return ITEM_MAX; };

	//Getter for item data array (JP:アイテムデータ配列)
      const ItemData* Get_Items(int i) const { return &m_items[i]; };

	//Getter for Item image handle(JP:アイテム画像ハンドルの取得)
	int Get_ItemHandle(Item_number type)const { return Item_Handle[type]; };

};

