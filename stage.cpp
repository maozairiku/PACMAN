//=============================================================================
//
// ステージモデル処理 [Stage.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "stage.h"
#include "shadow.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_Stage			"data/MODEL/map.obj"		// 読み込むモデル名


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static stage			g_Stage[MAX_stage];				// ステージ

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStage(void)
{
	for (int i = 0; i < MAX_stage; i++)
	{
		LoadModel(MODEL_Stage, &g_Stage[i].model);
		g_Stage[i].load = true;

		g_Stage[i].pos = XMFLOAT3(0.0f, 6.0f, 0.0f);
		g_Stage[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Stage[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Stage[i].use = true;			// true:生きてる

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStage(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_stage; i++)
	{
		if (g_Stage[i].load)
		{
			UnloadModel(&g_Stage[i].model);
			g_Stage[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStage(void)
{
	for (int i = 0; i < MAX_stage; i++)
	{

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStage(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_stage; i++)
	{
		if (g_Stage[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Stage[i].scl.x, g_Stage[i].scl.y, g_Stage[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Stage[i].rot.x, g_Stage[i].rot.y + XM_PI, g_Stage[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Stage[i].pos.x, g_Stage[i].pos.y, g_Stage[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Stage[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Stage[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// ステージの取得
//=============================================================================
stage* GetStage()
{
	return &g_Stage[0];
}
