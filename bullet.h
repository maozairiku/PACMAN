//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET		(256)	// 弾最大数

enum TYPE
{
	LINE,
	BEZIER,
	
	TYPE_NUM,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			shadowIdx;		// 影ID
	bool		use;			// 使用しているかどうか

	int			aniFrame;		// アニメーション用フレーム
	int			aniDir;			// アニメーション用方向

	int			moveType; // 移動タイプ
	XMFLOAT3	beizerPos[4]; // ベジェ曲線用の座標
	float		beizerTime; // ベジェ曲線用の時間


} BULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);
int SetBulletBezier(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 targetPos);

BULLET *GetBullet(void);

