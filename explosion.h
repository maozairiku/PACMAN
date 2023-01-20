//=============================================================================
//
// 爆発処理 [explosion.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct EXPLOSION
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	bool		bUse;			// 使用しているかどうか
	float		countAnim;		// animation count
	float		animFlame;		// animation flame 

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);

EXPLOSION* GetExplosion(void);

int SetExplosion(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


