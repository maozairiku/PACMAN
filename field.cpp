//=============================================================================
//
// フィールド表示処理 [field.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "sprite.h"
#include "renderer.h"
#include "camera.h"
#include "debugproc.h"
#include "field.h"
#include "meshfield.h"
#include <math.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)						// テクスチャの数
#define TEXTURE_FLOOR_SIZE	(1500)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexField(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static UPFIELD						g_Field;			

static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点情報
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static XMFLOAT3		g_upField;
static XMFLOAT3		g_rotField;

int wavemove;


static char* g_TextureName[] = {
	"data/TEXTURE/ocean.jpg"
};

// reflection
static ID3D11PixelShader* g_ReflectionPixelShader = NULL;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitField(void)
{
	// 頂点バッファの作成
	MakeVertexField();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);


	// 反射ピクセルシェーダ
	{
		ID3DBlob* pPSBlob = NULL;
		ID3DBlob* pErrorBlob;
		DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;
		HRESULT hr;

		hr = D3DX11CompileFromFile("ground.hlsl", NULL, NULL, "ReflectionPS", "ps_4_0", shFlag, 0, NULL, &pPSBlob, &pErrorBlob, NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
		}

		GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_ReflectionPixelShader);

		pPSBlob->Release();
	}

	wavemove = 0.0f;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer) 
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateField(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
{
	FIELD* meshfield = GetMeshField();

	// ライティングを無効
	SetLightEnable(false);

	// reflection shader set
	GetDeviceContext()->PSSetShader(g_ReflectionPixelShader, NULL, 0);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Field.rot.x, g_Field.rot.y, g_Field.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Field.pos.x, g_Field.pos.y, g_Field.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Field.mtxWorld, mtxWorld);

	// wave move
	float px = -700.0f;
	float pz = 700.0f;
	float pw = TEXTURE_FLOOR_SIZE;
	float ph = 1.0f;

	float tx = 0.0f;
	float ty = 0.0f;
	float tw = 1.0f;
	float th = 1.0f / TEXTURE_FLOOR_SIZE;

	for (int i = pz; i > pz - TEXTURE_FLOOR_SIZE; i--)
	{
		float wave = 5 * sinf(0.05 * (i + wavemove));	


		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

		// 左上を原点として設定するプログラム
		vertex[0].Position = XMFLOAT3(px + wave , 0.0f, i);
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = XMFLOAT2(tx, abs(i - 700) * th);
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		vertex[1].Position = XMFLOAT3(px + wave + TEXTURE_FLOOR_SIZE, 0.0f, i);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, abs(i - 700) * th);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		vertex[2].Position = XMFLOAT3(px + wave, 0.0f, i - 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = XMFLOAT2(tx, (abs(i - 700) * th) + th);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		vertex[3].Position = XMFLOAT3(px + wave + TEXTURE_FLOOR_SIZE, 0.0f, i - 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, (abs(i - 700) * th) + th);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	
	wavemove++;

	// Set Default Shader
	SetDefaultShader();

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false);
}



HRESULT MakeVertexField(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = TEXTURE_FLOOR_SIZE;
	float fWeight = TEXTURE_FLOOR_SIZE;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-TEXTURE_FLOOR_SIZE, 0.0f, TEXTURE_FLOOR_SIZE);
	vertex[1].Position = XMFLOAT3(TEXTURE_FLOOR_SIZE, 0.0f, TEXTURE_FLOOR_SIZE);
	vertex[2].Position = XMFLOAT3(-TEXTURE_FLOOR_SIZE, 0.0f, -TEXTURE_FLOOR_SIZE);
	vertex[3].Position = XMFLOAT3(TEXTURE_FLOOR_SIZE, 0.0f, -TEXTURE_FLOOR_SIZE);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}