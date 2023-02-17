//=============================================================================
//
// パーティクル処理 [pexplosion.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "pexplosion.h"
#include "player.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(1)			// テクスチャの数

#define	PEXPLOSION_SIZE_X		(40.0f)		// 頂点サイズ
#define	PEXPLOSION_SIZE_Y		(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_PEXPLOSION	(5.0f)		// 移動速度

#define	MAX_PEXPLOSION			(512)		// パーティクル最大数

#define	DISP_SHADOW							// 影の表示
//#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	bool			bUse;			// 使用しているかどうか

} PEXPLOSION;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexPExplosion(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int						g_TexNo;							// テクスチャ番号

static PEXPLOSION				g_PExplosion[MAX_PEXPLOSION];		// パーティクルワーク
static XMFLOAT3					g_posBase;							// ビルボード発生位置
static float					g_fWidthBase = 5.0f;				// 基準の幅
static float					g_fHeightBase = 10.0f;				// 基準の高さ
static float					g_roty = 0.0f;						// 移動方向
static float					g_spd = 0.0f;						// 移動スピード

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPExplosion(void)
{
	// 頂点情報の作成
	MakeVertexPExplosion();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
	{
		g_PExplosion[nCntPExplosion].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_PExplosion[nCntPExplosion].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_PExplosion[nCntPExplosion].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_PExplosion[nCntPExplosion].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_PExplosion[nCntPExplosion].material, sizeof(g_PExplosion[nCntPExplosion].material));
		g_PExplosion[nCntPExplosion].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_PExplosion[nCntPExplosion].fSizeX = PEXPLOSION_SIZE_X;
		g_PExplosion[nCntPExplosion].fSizeY = PEXPLOSION_SIZE_Y;
		g_PExplosion[nCntPExplosion].nIdxShadow = -1;
		g_PExplosion[nCntPExplosion].nLife = 0;
		g_PExplosion[nCntPExplosion].bUse = false;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPExplosion(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePExplosion(void)
{
	PLAYER *pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	{
		for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
		{
			if (g_PExplosion[nCntPExplosion].bUse)		// 使用中
			{
				g_PExplosion[nCntPExplosion].pos.x += g_PExplosion[nCntPExplosion].move.x;
				g_PExplosion[nCntPExplosion].pos.z += g_PExplosion[nCntPExplosion].move.z;

				g_PExplosion[nCntPExplosion].pos.y += g_PExplosion[nCntPExplosion].move.y;
				if (g_PExplosion[nCntPExplosion].pos.y <= g_PExplosion[nCntPExplosion].fSizeY / 2)	// 着地した
				{
					g_PExplosion[nCntPExplosion].pos.y = g_PExplosion[nCntPExplosion].fSizeY / 2;
					g_PExplosion[nCntPExplosion].move.y = -g_PExplosion[nCntPExplosion].move.y * 0.75f;
				}

				g_PExplosion[nCntPExplosion].move.x += (0.0f - g_PExplosion[nCntPExplosion].move.x) * 0.015f;
				g_PExplosion[nCntPExplosion].move.y -= 0.25f;
				g_PExplosion[nCntPExplosion].move.z += (0.0f - g_PExplosion[nCntPExplosion].move.z) * 0.015f;

#ifdef DISP_SHADOW
				if (g_PExplosion[nCntPExplosion].nIdxShadow != -1)		// 影使用中
				{
					float colA;

					// 影の位置設定
					SetPositionShadow(g_PExplosion[nCntPExplosion].nIdxShadow, XMFLOAT3(g_PExplosion[nCntPExplosion].pos.x, 0.1f, g_PExplosion[nCntPExplosion].pos.z));

					// 影の色の設定
					colA = g_PExplosion[nCntPExplosion].material.Diffuse.w;
					SetColorShadow(g_PExplosion[nCntPExplosion].nIdxShadow, XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
				}
#endif

				g_PExplosion[nCntPExplosion].nLife--;
				if (g_PExplosion[nCntPExplosion].nLife <= 0)
				{
					g_PExplosion[nCntPExplosion].bUse = false;
					ReleaseShadow(g_PExplosion[nCntPExplosion].nIdxShadow);
					g_PExplosion[nCntPExplosion].nIdxShadow = -1;
				}
				else
				{
					if (g_PExplosion[nCntPExplosion].nLife <= 20)
					{
						g_PExplosion[nCntPExplosion].material.Diffuse.x = 0.7f - (float)(20 - g_PExplosion[nCntPExplosion].nLife) / 20 * 0.7f;
						g_PExplosion[nCntPExplosion].material.Diffuse.y = 0.5f - (float)(20 - g_PExplosion[nCntPExplosion].nLife) / 20 * 0.5f;
						g_PExplosion[nCntPExplosion].material.Diffuse.z = 0.0f - (float)(20 - g_PExplosion[nCntPExplosion].nLife) / 20 * 0.0f;
					}

					if (g_PExplosion[nCntPExplosion].nLife <= 10)
					{
						// α値設定
						g_PExplosion[nCntPExplosion].material.Diffuse.w -= 0.05f;
						if (g_PExplosion[nCntPExplosion].material.Diffuse.w < 0.0f)
						{
							g_PExplosion[nCntPExplosion].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPExplosion(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(false);

	// フォグ無効
	SetFogEnable(false);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
	{
		if (g_PExplosion[nCntPExplosion].bUse)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_PExplosion[nCntPExplosion].scale.x, g_PExplosion[nCntPExplosion].scale.y, g_PExplosion[nCntPExplosion].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_PExplosion[nCntPExplosion].pos.x, g_PExplosion[nCntPExplosion].pos.y, g_PExplosion[nCntPExplosion].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_PExplosion[nCntPExplosion].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(true);

	// フォグ有効
	SetFogEnable(true);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexPExplosion(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PEXPLOSION_SIZE_X / 2, PEXPLOSION_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PEXPLOSION_SIZE_X / 2, PEXPLOSION_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PEXPLOSION_SIZE_X / 2, -PEXPLOSION_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PEXPLOSION_SIZE_X / 2, -PEXPLOSION_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorPExplosion(int nIdxPExplosion, XMFLOAT4 col)
{
	g_PExplosion[nIdxPExplosion].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetPExplosion(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxPExplosion = -1;

	for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
	{
		if (!g_PExplosion[nCntPExplosion].bUse)
		{
			g_PExplosion[nCntPExplosion].pos = pos;
			g_PExplosion[nCntPExplosion].rot = { 0.0f, 0.0f, 0.0f };
			g_PExplosion[nCntPExplosion].scale = { 1.0f, 1.0f, 1.0f };
			g_PExplosion[nCntPExplosion].move = move;
			g_PExplosion[nCntPExplosion].material.Diffuse = col;
			g_PExplosion[nCntPExplosion].fSizeX = fSizeX;
			g_PExplosion[nCntPExplosion].fSizeY = fSizeY;
			g_PExplosion[nCntPExplosion].nLife = nLife;
			g_PExplosion[nCntPExplosion].bUse = true;

			nIdxPExplosion = nCntPExplosion;

#ifdef DISP_SHADOW
			// 影の設定
			g_PExplosion[nCntPExplosion].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
			if (g_PExplosion[nCntPExplosion].nIdxShadow != -1)
			{
				SetColorShadow(g_PExplosion[nCntPExplosion].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxPExplosion;
}

//=============================================================================
// set particle explosion
//=============================================================================
void SetExplosionParticle(void)
{
	XMFLOAT3 pos;
	XMFLOAT3 move;
	float fAngle, fLength;
	int nLife;
	float fSize;

	pos = g_posBase;

	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;

	nLife = 30;

	fSize = (float)(rand() % 30 + 20);

	pos.y = fSize / 2;

	for (int i = 0; i < 50; i++)
	{
		move.x = (rand() % 6) - 3;	
		move.y = rand() % 5;		
		move.z = (rand() % 6) - 3;	

		SetPExplosion(pos, move, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), fSize, fSize, nLife);
	}

}