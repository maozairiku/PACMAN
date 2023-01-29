//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "particle.h"
#include "player.h"
#include <time.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY_GR		"data/MODEL/ghostred.obj"		// Ghost_Red
#define	MODEL_ENEMY_GO		"data/MODEL/orange.obj"			// Ghost_Orange
#define	MODEL_ENEMY_GG		"data/MODEL/ghostgreen.obj"		// Ghost_Green
#define	MODEL_ENEMY_GB		"data/MODEL/ghostblue.obj"		// Ghost_Blue
#define	MODEL_ENEMY_GP		"data/MODEL/ghostpurple.obj"	// Ghost_Purple

#define	VALUE_MOVE			(5.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define ENEMY_OFFSET_Y		(20.0f)							// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_GhostRed[MAX_ENEMY];				// エネミー
static ENEMY			g_GhostOrange[ORANGE_MAX];

static float g_angle = 0;

static BOOL				g_Load = FALSE;

static float orangeRadius[ORANGE_MAX];

static float g_timer = -1;


// red_ghost達の線形補間
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f), XMFLOAT3(0.0f, XMConvertToRadians(180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1},
	{ XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f), XMFLOAT3(0.0f, XMConvertToRadians(0), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1},
	{ XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f), XMFLOAT3(0.0f, XMConvertToRadians(-180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1},
};

static INTERPOLATION_DATA move_tbl2[] = {
	{ XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f), XMFLOAT3(0.0f, XMConvertToRadians(180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2},
	{ XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f), XMFLOAT3(0.0f, XMConvertToRadians(0), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2},
	{ XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f), XMFLOAT3(0.0f, XMConvertToRadians(-180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2},
};

static INTERPOLATION_DATA move_tbl3[] = {
	{ XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f), XMFLOAT3(0.0f, XMConvertToRadians(180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.5},
	{ XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f), XMFLOAT3(0.0f, XMConvertToRadians(0), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.5},
	{ XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f), XMFLOAT3(0.0f, XMConvertToRadians(-180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.5},
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	srand((unsigned int)time(NULL));
	g_timer = -1;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// Ghost_Red
		LoadModel(MODEL_ENEMY_GR, &g_GhostRed[i].model);
		g_GhostRed[i].load = true;

		g_GhostRed[0].pos = XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f);
		g_GhostRed[1].pos = XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f);
		g_GhostRed[2].pos = XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f);

		g_GhostRed[i].rot = XMFLOAT3(0.0f, 360.0f, 0.0f);
		g_GhostRed[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostRed[i].spd = 0.0f;			// 移動スピードクリア
		g_GhostRed[i].size = ENEMY_SIZE;	// 当たり判定の大きさ
		g_GhostRed[i].colorIdx = 1;
		g_GhostRed[i].attackflame = 0;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_GhostRed[0].model, &g_GhostRed[0].diffuse[0]);

		XMFLOAT3 rpos = g_GhostRed[i].pos;
		rpos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostRed[i].shadowIdx = CreateShadow(rpos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostRed[i].move_time = 0.0f;		// 線形補間用のタイマーをクリア
		g_GhostRed[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_GhostRed[i].tbl_size = 0;			// 再生するアニメデータのレコード数をセット

		g_GhostRed[i].use = true;			// true:生きてる

	}

	// orange ghost
	LoadModel(MODEL_ENEMY_GO, &g_GhostOrange[0].model);
	g_GhostOrange[0].load = true;

	for (int i = 0; i < ORANGE_MAX; i++)
	{
		g_GhostOrange[i].pos = XMFLOAT3(rand()%1300 - 650, ENEMY_OFFSET_Y, rand()%1300 - 650);
		g_GhostOrange[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostOrange[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostOrange[i].spd = 0.0f;			// 移動スピードクリア
		g_GhostOrange[i].size = ENEMY_SIZE;		// 当たり判定の大きさ
		g_GhostOrange[i].colorIdx = 2;
		g_GhostOrange[i].attackflame = 0;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_GhostOrange[0].model, &g_GhostOrange[0].diffuse[0]);

		XMFLOAT3 opos = g_GhostOrange[i].pos;
		opos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostOrange[i].shadowIdx = CreateShadow(opos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostOrange[i].move_time = 0.0f;		// 線形補間用のタイマーをクリア
		g_GhostOrange[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_GhostOrange[i].tbl_size = 0;			// 再生するアニメデータのレコード数をセット

		g_GhostOrange[i].use = true;			// true:生きてる

		orangeRadius[i] = rand() % 10;
	}

	// ghost red達は線形補間で動かしてみる
	g_GhostRed[0].move_time = 0.0f;				// 線形補間用のタイマーをクリア
	g_GhostRed[0].tbl_adr = move_tbl;			// 再生するアニメデータの先頭アドレスをセット
	g_GhostRed[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

	g_GhostRed[1].move_time = 0.0f;				// 線形補間用のタイマーをクリア
	g_GhostRed[1].tbl_adr = move_tbl2;			// 再生するアニメデータの先頭アドレスをセット
	g_GhostRed[1].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

	g_GhostRed[2].move_time = 0.0f;				// 線形補間用のタイマーをクリア
	g_GhostRed[2].tbl_adr = move_tbl3;			// 再生するアニメデータの先頭アドレスをセット
	g_GhostRed[2].tbl_size = sizeof(move_tbl3) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_GhostRed[i].load)
		{
			UnloadModel(&g_GhostRed[i].model);
			g_GhostRed[i].load = false;
		}
	}

	if (g_GhostOrange[0].load)
	{
		UnloadModel(&g_GhostOrange[0].model);
		g_GhostOrange[0].load = false;
	}

	g_Load = FALSE;

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// move time
	g_timer++;

	// for particle
	int nLife = 150;
	float fSize = 50.0f;

	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		BULLET* bullet = GetBullet();
		PLAYER* player = GetPlayer();

		// Ghost_Red
		if (g_GhostRed[i].use == true)			// このエネミーが使われている？
		{										// Yes
			if (g_GhostRed[i].tbl_adr != NULL)	// 線形補間を実行する？
			{									// 線形補間の処理
				// 移動処理
				int		index = (int)g_GhostRed[i].move_time;
				float	time = g_GhostRed[i].move_time - index;
				int		size = g_GhostRed[i].tbl_size;

				float dt = 1.0f / g_GhostRed[i].tbl_adr[index].frame;	// 1フレームで進める時間
				g_GhostRed[i].move_time += dt;							// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					g_GhostRed[i].move_time = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				XMVECTOR p1 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 1].pos);	// 次の場所
				XMVECTOR p0 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 0].pos);	// 現在の場所
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				XMVECTOR r1 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 1].rot);	// 次の角度
				XMVECTOR r0 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 0].rot);	// 現在の角度
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				XMVECTOR s1 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 1].scl);	// 次のScale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 0].scl);	// 現在のScale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostRed[i].scl, s0 + scl * time);

			}

			//// レイキャストして足元の高さを求める
			//XMFLOAT3 HitPosition;		// 交点
			//XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
			//bool ans = RayHitField(g_GhostRed[i].pos, &HitPosition, &Normal);
			//if (ans)
			//{
			//	g_GhostRed[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			//}
			//else
			//{
			//	g_GhostRed[i].pos.y = ENEMY_OFFSET_Y;
			//	Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			//}

			// 影もエネミーの位置に合わせる
			XMFLOAT3 pos = g_GhostRed[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostRed[i].shadowIdx, pos);

			// enemy attack frame count
			g_GhostRed[i].attackflame++;

			if ((int)g_timer % 50 == 0)
			{
				SetBulletBezier(g_GhostRed[i].pos, g_GhostRed[i].rot, player->pos);
			}

			// particle
			//SetParticle(g_GhostRed[i].pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
		}
	}

	// enemy cercle move
	float radian = XMConvertToRadians(g_angle);
	if (radian >= XM_2PI)
	{
		radian -= XM_2PI;
		g_angle = 0;
	}

	for (int i = 0; i < ORANGE_MAX; i++)
	{
		if (g_GhostOrange[i].use == true)
		{
			g_GhostOrange[i].pos.x += orangeRadius[i] * cosf(radian);
			g_GhostOrange[i].pos.z += orangeRadius[i] * sinf(radian);
			g_GhostOrange[i].rot.y += 0.1f;

			if ((int)g_timer % 60 == 0)
			{
				SetBullet(g_GhostOrange[i].pos, g_GhostOrange[i].rot);
			}

			// orange ghost move particle
			SetParticle(g_GhostOrange[i].pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), fSize, fSize, nLife);
		}
	}

	g_angle++;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// Ghost Red
		if (g_GhostRed[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_GhostRed[i].scl.x, g_GhostRed[i].scl.y, g_GhostRed[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostRed[i].rot.x, g_GhostRed[i].rot.y + XM_PI, g_GhostRed[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_GhostRed[i].pos.x, g_GhostRed[i].pos.y, g_GhostRed[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostRed[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_GhostRed[0].model);
	}

	for (int i = 0; i < ORANGE_MAX; i++)
	{
		if (g_GhostOrange[i].use == false) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_GhostOrange[i].scl.x, g_GhostOrange[i].scl.y, g_GhostOrange[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostOrange[i].rot.x, g_GhostOrange[i].rot.y + XM_PI, g_GhostOrange[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_GhostOrange[i].pos.x, g_GhostOrange[i].pos.y, g_GhostOrange[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostOrange[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_GhostOrange[0].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetGhostRed()
{
	return &g_GhostRed[0];
}
ENEMY* GetGhostOrange()
{
	return &g_GhostOrange[0];
}

