//=============================================================================
//
// �t�B�[���h�\������ [field.cpp]
// Author : ���@�E��
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)						// �e�N�X�`���̐�
#define TEXTURE_FLOOR_SIZE	(1500)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static UPFIELD						g_Field;			

static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_���
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;				// �e�N�X�`���ԍ�

static XMFLOAT3		g_upField;
static XMFLOAT3		g_rotField;

int wavemove;


static char* g_TextureName[] = {
	"data/TEXTURE/ocean.jpg"
};

// reflection
static ID3D11PixelShader* g_ReflectionPixelShader = NULL;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
{
	// ���_�o�b�t�@�̍쐬
	MakeVertexField();

	// �e�N�X�`������
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


	// ���˃s�N�Z���V�F�[�_
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
// �I������
//=============================================================================
void UninitField(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) 
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
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
// �X�V����
//=============================================================================
void UpdateField(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	FIELD* meshfield = GetMeshField();

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	// reflection shader set
	GetDeviceContext()->PSSetShader(g_ReflectionPixelShader, NULL, 0);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Field.rot.x, g_Field.rot.y, g_Field.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Field.pos.x, g_Field.pos.y, g_Field.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
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

		// ��������_�Ƃ��Đݒ肷��v���O����
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

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	
	wavemove++;

	// Set Default Shader
	SetDefaultShader();

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(false);
}



HRESULT MakeVertexField(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = TEXTURE_FLOOR_SIZE;
	float fWeight = TEXTURE_FLOOR_SIZE;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-TEXTURE_FLOOR_SIZE, 0.0f, TEXTURE_FLOOR_SIZE);
	vertex[1].Position = XMFLOAT3(TEXTURE_FLOOR_SIZE, 0.0f, TEXTURE_FLOOR_SIZE);
	vertex[2].Position = XMFLOAT3(-TEXTURE_FLOOR_SIZE, 0.0f, -TEXTURE_FLOOR_SIZE);
	vertex[3].Position = XMFLOAT3(TEXTURE_FLOOR_SIZE, 0.0f, -TEXTURE_FLOOR_SIZE);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}