//=============================================================================
//
// 空モデル処理 [Sky.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "sky.h"
#include "shadow.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_Sky			"data/MODEL/sky.obj"		// 読み込むモデル名


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static SKY				g_Sky[MAX_SKY];				// ステージ

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{
		LoadModel(MODEL_Sky, &g_Sky[i].model);
		g_Sky[i].load = true;

		g_Sky[i].pos = XMFLOAT3(0.0f, 6.0f, 0.0f);
		g_Sky[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Sky[i].scl = XMFLOAT3(100.0f, 100.0f, 100.0f);

		g_Sky[i].use = true;		

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].load)
		{
			UnloadModel(&g_Sky[i].model);
			g_Sky[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{
	for (int i = 0; i < MAX_SKY; i++)
	{

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_SKY; i++)
	{
		if (g_Sky[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Sky[i].scl.x, g_Sky[i].scl.y, g_Sky[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sky[i].rot.x, g_Sky[i].rot.y + XM_PI, g_Sky[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Sky[i].pos.x, g_Sky[i].pos.y, g_Sky[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sky[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Sky[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// ステージの取得
//=============================================================================
SKY* GetSky()
{
	return &g_Sky[0];
}
