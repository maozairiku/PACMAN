//=============================================================================
//
// パーティクル処理 [rain.cpp]
// Author : 王ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "rain.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	RAIN_SIZE_X			(2.0f)		// 頂点サイズ
#define	RAIN_SIZE_Y			(5.0f)		// 頂点サイズ
#define	VALUE_MOVE_RAIN		(5.0f)		// 移動速度

#define	MAX_RAIN			(1024)		// パーティクル最大数

#define	DISP_SHADOW						// 影の表示

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

} RAIN;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexRain(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static RAIN						g_Rain[MAX_RAIN];				// パーティクルワーク
static XMFLOAT3					g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRain(void)
{
	// 頂点情報の作成
	MakeVertexRain();

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
	for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
	{
		g_Rain[nCntRain].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rain[nCntRain].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rain[nCntRain].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Rain[nCntRain].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_Rain[nCntRain].material, sizeof(g_Rain[nCntRain].material));
		g_Rain[nCntRain].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Rain[nCntRain].fSizeX = RAIN_SIZE_X;
		g_Rain[nCntRain].fSizeY = RAIN_SIZE_Y;
		g_Rain[nCntRain].nIdxShadow = -1;
		g_Rain[nCntRain].nLife = 0;
		g_Rain[nCntRain].bUse = false;
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
void UninitRain(void)
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
void UpdateRain(void)
{

	{
		for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
		{
			if (g_Rain[nCntRain].bUse)				// 使用中
			{
				g_Rain[nCntRain].pos.x += g_Rain[nCntRain].move.x;
				g_Rain[nCntRain].pos.y += g_Rain[nCntRain].move.y;

				if (g_Rain[nCntRain].pos.y < 1.0f)	// 着地した  
				{
					g_Rain[nCntRain].bUse = FALSE;
				}


#ifdef DISP_SHADOW		// shadow用

#endif

				g_Rain[nCntRain].nLife--;
				if (g_Rain[nCntRain].nLife <= 0)
				{
					g_Rain[nCntRain].bUse = false;
					ReleaseShadow(g_Rain[nCntRain].nIdxShadow);
					g_Rain[nCntRain].nIdxShadow = -1;
				}
				else
				{
					if (g_Rain[nCntRain].nLife <= 80)
					{
						g_Rain[nCntRain].material.Diffuse.x = 1.0f - (float)(80 - g_Rain[nCntRain].nLife) / 80 * 1.0f;
						g_Rain[nCntRain].material.Diffuse.y = 1.0f - (float)(80 - g_Rain[nCntRain].nLife) / 80 * 1.0f;
						g_Rain[nCntRain].material.Diffuse.z = 1.0f - (float)(80 - g_Rain[nCntRain].nLife) / 80 * 1.0f;
					}

					if (g_Rain[nCntRain].nLife <= 20)
					{
						// α値設定
						g_Rain[nCntRain].material.Diffuse.w -= 0.05f;
						if (g_Rain[nCntRain].material.Diffuse.w < 0.0f)
						{
							g_Rain[nCntRain].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

		// パーティクル発生
		{
			XMFLOAT3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;

			// rain set
			for (int r = 0; r < 100; r++)
			{
				fAngle = (float)(rand() % 628 - 314) / 100.0f;
				fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;
				move.x = 0.0f;
				move.y = -(rand() % 10);
				move.z = 0.0f;

				nLife = rand() % 100 + 150;

				// ビルボードの設定
				SetRain(XMFLOAT3((rand() % 1300 - 650), 100.0f, (rand() % 1300 - 650)), move, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.85f), RAIN_SIZE_X, RAIN_SIZE_Y, nLife);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawRain(void)
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

	for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
	{
		if (g_Rain[nCntRain].bUse)
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
			mtxScl = XMMatrixScaling(g_Rain[nCntRain].scale.x, g_Rain[nCntRain].scale.y, g_Rain[nCntRain].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Rain[nCntRain].pos.x, g_Rain[nCntRain].pos.y, g_Rain[nCntRain].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_Rain[nCntRain].material);

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
HRESULT MakeVertexRain(void)
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
		vertex[0].Position = XMFLOAT3(-RAIN_SIZE_X / 2, RAIN_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(RAIN_SIZE_X / 2, RAIN_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-RAIN_SIZE_X / 2, -RAIN_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(RAIN_SIZE_X / 2, -RAIN_SIZE_Y / 2, 0.0f);

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
void SetColorRain(int nIdxRain, XMFLOAT4 col)
{
	g_Rain[nIdxRain].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetRain(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxRain = -1;

	for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
	{
		if (!g_Rain[nCntRain].bUse)
		{
			g_Rain[nCntRain].pos = pos;
			g_Rain[nCntRain].rot = { 0.0f, 0.0f, 0.0f };
			g_Rain[nCntRain].scale = { 1.0f, 1.0f, 1.0f };
			g_Rain[nCntRain].move = move;
			g_Rain[nCntRain].material.Diffuse = col;
			g_Rain[nCntRain].fSizeX = fSizeX;
			g_Rain[nCntRain].fSizeY = fSizeY;
			g_Rain[nCntRain].nLife = nLife;
			g_Rain[nCntRain].bUse = true;

			nIdxRain = nCntRain;

#ifdef DISP_SHADOW		// shadow用

#endif

			break;
		}
	}

	return nIdxRain;
}
