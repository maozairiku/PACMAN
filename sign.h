//=============================================================================
//
// サイン処理 [sign.h]
// Author : 王　ウ華
//
//=============================================================================
#pragma once
struct SIGN
{
	XMFLOAT3			pos;				// 位置
	XMFLOAT3			rot;				// 回転
	XMFLOAT3			scl;				// スケール
	MATERIAL			material;			// マテリアル
	float				fWidth;				// 幅
	float				fHeight;			// 高さ
	int					nIdxShadow;			// 影ID
	bool				bUse;				// 使用しているかどうか

	INTERPOLATION_MOVE* tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSign(void);
void UninitSign(void);
void UpdateSign(void);
void DrawSign(void);

int SetSign(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


