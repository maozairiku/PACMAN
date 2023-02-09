//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "explosion.h"
#include "collision.h"
#include "enemy.h"
#include "particle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER			"data/MODEL/duck.obj"			// 読み込むモデル名

#define	VALUE_MOVE				(2.0f)							// 移動量
#define	VALUE_ROTATE			(D3DX_PI * 0.02f)				// 回転量

#define PLAYER_SHADOW_SIZE		(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y			(15.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX		(2)								// プレイヤーのパーツの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static float		roty = 0.0f;

static LIGHT		g_Light;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.load = TRUE;
	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = XMFLOAT3(100.0f,PLAYER_OFFSET_Y, -50.0f);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;			// 移動スピードクリア

	g_Player.use = TRUE;			// true:生きてる
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	// キーを押した時のプレイヤーの向き
	roty = 0.0f;

	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる

	// クォータニオンの初期化
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	XMFLOAT3 oldPos = g_Player.pos;

	CAMERA *cam = GetCamera();

	// for particle
	int nLife = 50;
	float fSize = 30.0f;

	g_Player.spd *= 0.9f;

	if (g_Player.use == true)
	{

		// 移動処理
		if (GetKeyboardPress(DIK_LEFT))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.x -= g_Player.spd;
			roty = XM_PI / 2;
			SetParticle(g_Player.pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), fSize, fSize, nLife);

		}
		if (GetKeyboardPress(DIK_RIGHT))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.x += g_Player.spd;
			roty = -XM_PI / 2;
			SetParticle(g_Player.pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), fSize, fSize, nLife);

		}
		if (GetKeyboardPress(DIK_UP))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.z += g_Player.spd;
			roty = XM_PI;
			SetParticle(g_Player.pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), fSize, fSize, nLife);
		}
		if (GetKeyboardPress(DIK_DOWN))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.z -= g_Player.spd;
			roty = 0.0f;
			SetParticle(g_Player.pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), fSize, fSize, nLife);
		}

		// ゲームパッド対応
		else if (IsButtonTriggered(0, BUTTON_LEFT))
		{
			g_Player.spd = VALUE_MOVE;
			roty = XM_PI / 2;
		}
		else if (IsButtonTriggered(0, BUTTON_RIGHT))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.x += g_Player.spd;
			roty = -XM_PI / 2;
		}
		else if (IsButtonTriggered(0, BUTTON_UP))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.z += g_Player.spd;
			roty = XM_PI;
		}
		else if (IsButtonTriggered(0, BUTTON_DOWN))
		{
			g_Player.spd = VALUE_MOVE;
			//g_Player.pos.z -= g_Player.spd;
			roty = 0.0f;
		}


		// player move limit
		if (g_Player.pos.x >= 630.0f)
		{
			g_Player.pos.x = 630.0f;
		}
		if (g_Player.pos.x <= -630.0f)
		{
			g_Player.pos.x = -630.0f;
		}
		if (g_Player.pos.z >= 650.0f)
		{
			g_Player.pos.z = 650.0f;
		}
		if (g_Player.pos.z <= -650.0f)
		{
			g_Player.pos.z = -650.0f;
		}


#ifdef _DEBUG
		if (GetKeyboardPress(DIK_R))
		{
			g_Player.pos.z = g_Player.pos.x = 0.0f;
			g_Player.spd = 0.0f;
			roty = 0.0f;
		}
#endif


		{	// 押した方向にプレイヤーを移動させる
			// 押した方向にプレイヤーを向かせている所
			g_Player.rot.y = roty + cam->rot.y;

			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}


		// レイキャストして足元の高さを求める
		XMFLOAT3 HitPosition;		// 交点
		XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
		bool ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
		if (ans)
		{
			g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
		}
		else
		{
			g_Player.pos.y = PLAYER_OFFSET_Y;
			Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}

		// 影もプレイヤーの位置に合わせる
		XMFLOAT3 pos = g_Player.pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		SetPositionShadow(g_Player.shadowIdx, pos);


		// ポイントライトのテスト
		{
			BULLET* bullet = GetBullet();

			for (int i = 0; i < MAX_BULLET; i++)
			{
				LIGHT* light = GetLightData(1);
				XMFLOAT3 pos = g_Player.pos;
				pos.y += 20.0f;

				light->Position = pos;
				light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Type = LIGHT_TYPE_POINT;
				light->Enable = TRUE;
				SetLightData(1, light);

				if (CollisionBC(g_Player.pos, bullet[i].pos, 10.0f, 10.0f))
				{
					light->Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
					light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}
		}
	}


#ifdef _DEBUG
	// デバッグ表示
		PrintDebugProc("Player X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int u = 0; u < MAX_PLAYER; u++)
	{

		if (g_Player.use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// クォータニオンを反映
		quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.Quaternion));
		mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


		// 縁取りの設定
		SetFuchi(1);

		// モデル描画
		DrawModel(&g_Player.model);

	}

	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する
		if (g_Parts[i].use == false) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Parts[i].model);

	}

	SetFuchi(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

