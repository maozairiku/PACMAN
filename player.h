//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)				// プレイヤーの数

#define PLAYER_HP_MAX	(3)				// HP最大数

#define	PLAYER_SIZE		(5.0f)			// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	float				spd;				// 移動スピード
	int					HP;					// HP
	
	BOOL				load;
	DX11_MODEL			model;				// モデル情報

	int					shadowIdx;			// 影のインデックス番号

	BOOL				use;

	float				size;

	// 階層アニメーション用のメンバー変数
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	float				move_time;			// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	int					select;				// title select

};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

