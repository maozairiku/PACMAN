//=============================================================================
//
// エネミーモデル処理 [dot.h]
// Author : 王　ウ華
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_DOT			(1)				// エネミーの数

#define	DOT_SIZE		(10.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct DOT
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	bool				use;
	bool				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	int					colorIdx;			// dot color

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitDot(void);
void UninitDot(void);
void UpdateDot(void);
void DrawDot(void);

DOT *GetCookies(void);
DOT *GetHotdog(void);
DOT *GetCherry(void);
DOT *GetBread(void);
DOT *GetCroissant(void);



