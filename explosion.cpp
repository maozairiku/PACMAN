//=============================================================================
//
// �������� [explosion.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "explosion.h"
#include "explosion.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(2)				// �e�N�X�`���̐�

#define	EXPLOSION_WIDTH			(50.0f)			// ���_�T�C�Y
#define	EXPLOSION_HEIGHT		(50.0f)			// ���_�T�C�Y

#define ANIM_WAIT				(24)			// �A�j���[�V�����̐؂�ւ��Wait�l
#define ANIM_WAIT2				(30)			//

#define	MAX_EXPLOSION			(256)			// �����ő吔

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexExplosion(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static EXPLOSION					g_Explosion[MAX_EXPLOSION];	// �������[�N
static int					g_TexNo;			// �e�N�X�`���ԍ�
static bool					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static char* g_TextureName[] =
{
	"data/TEXTURE/explosion.png",
	"data/TEXTURE/explosion2.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitExplosion(void)
{
	MakeVertexExplosion();

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

	// �������[�N�̏�����
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		ZeroMemory(&g_Explosion[nCntExplosion].material, sizeof(g_Explosion[nCntExplosion].material));
		g_Explosion[nCntExplosion].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Explosion[nCntExplosion].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[nCntExplosion].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[nCntExplosion].fWidth = EXPLOSION_WIDTH;
		g_Explosion[nCntExplosion].fHeight = EXPLOSION_HEIGHT;
		g_Explosion[nCntExplosion].bUse = false;
		g_Explosion[nCntExplosion].countAnim = 0.0f;
		g_Explosion[nCntExplosion].animFlame = 0.0f;
		g_Explosion[nCntExplosion].type = DIE_EXPLO;
	}

	g_bAlpaTest = true;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitExplosion(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateExplosion(void)
{	
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (g_Explosion[nCntExplosion].bUse == true)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			VERTEX_3D* vertex;
			
			float fWidth = g_Explosion[nCntExplosion].fWidth;
			float fHeight = g_Explosion[nCntExplosion].fHeight;
			
			switch (g_Explosion[nCntExplosion].type)
			{
			case DIE_EXPLO:
				g_Explosion[nCntExplosion].countAnim += 1.0f;
				if ((int)g_Explosion[nCntExplosion].countAnim % 3 == 0)
				{
					g_Explosion[nCntExplosion].animFlame++;
				}

				// ���_�o�b�t�@�ɒl���Z�b�g����
				GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

				vertex = (VERTEX_3D*)msr.pData;

				// ���_���W�̐ݒ�
				vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
				vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
				vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
				vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

				// �g�U���̐ݒ�
				vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// �e�N�X�`�����W�̐ݒ�
				vertex[0].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 8, 0.0f);
				vertex[1].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 8, 0.0f);
				vertex[2].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 8, 1.0f);
				vertex[3].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 8, 1.0f);

				GetDeviceContext()->Unmap(g_VertexBuffer, 0);

				if (g_Explosion[nCntExplosion].countAnim >= ANIM_WAIT)
				{
					g_Explosion[nCntExplosion].bUse = false;
					ReleaseShadow(g_Explosion[nCntExplosion].nIdxShadow);
					g_Explosion[nCntExplosion].countAnim = 0.0f;
				}
				break;

			case OFFGROUND_EXPLO:
				g_Explosion[nCntExplosion].countAnim += 1.0f;
				if ((int)g_Explosion[nCntExplosion].countAnim % 3 == 0)
				{
					g_Explosion[nCntExplosion].animFlame++;
				}

				// ���_�o�b�t�@�ɒl���Z�b�g����
				GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

				vertex = (VERTEX_3D*)msr.pData;

				// ���_���W�̐ݒ�
				vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
				vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
				vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
				vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

				// �g�U���̐ݒ�
				vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// �e�N�X�`�����W�̐ݒ�
				vertex[0].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 10, 0.0f);
				vertex[1].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 10, 0.0f);
				vertex[2].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 10, 1.0f);
				vertex[3].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 10, 1.0f);

				GetDeviceContext()->Unmap(g_VertexBuffer, 0);

				if (g_Explosion[nCntExplosion].countAnim >= ANIM_WAIT2)
				{
					g_Explosion[nCntExplosion].bUse = false;
					ReleaseShadow(g_Explosion[nCntExplosion].nIdxShadow);
					g_Explosion[nCntExplosion].countAnim = 0.0f;
				}
				break;
			}
		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? false : true;
	}

	//// �A���t�@�e�X�g��臒l�ύX
	//if(GetKeyboardPress(DIK_I))
	//{
	//	g_nAlpha--;
	//	if(g_nAlpha < 0)
	//	{
	//		g_nAlpha = 0;
	//	}
	//}
	//if(GetKeyboardPress(DIK_K))
	//{
	//	g_nAlpha++;
	//	if(g_nAlpha > 255)
	//	{
	//		g_nAlpha = 255;
	//	}
	//}
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawExplosion(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == true)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(true);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		if (g_Explosion[i].bUse)
		{

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Explosion[i].scl.x, g_Explosion[i].scl.y, g_Explosion[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Explosion[i].pos.x, g_Explosion[i].pos.y, g_Explosion[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Explosion[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Explosion[i].type]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(false);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexExplosion(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = EXPLOSION_WIDTH;
	float fHeight = EXPLOSION_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(0.125f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(0.125f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �����̃p�����[�^���Z�b�g
//=============================================================================
int SetExplosion(XMFLOAT3 pos, float fWidth, float fHeight, int Type)
{
	int nIdxExplosion = -1;

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (!g_Explosion[nCntExplosion].bUse)
		{
			if (Type == DIE_EXPLO)
			{
				g_Explosion[nCntExplosion].pos = pos;
			}
			else if(Type == OFFGROUND_EXPLO)
			{
				g_Explosion[nCntExplosion].pos = XMFLOAT3(pos.x, 0.0, pos.z);
			}
			g_Explosion[nCntExplosion].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Explosion[nCntExplosion].fWidth = fWidth;
			g_Explosion[nCntExplosion].fHeight = fHeight;
			g_Explosion[nCntExplosion].type = Type;
			g_Explosion[nCntExplosion].bUse = true;

			// �e�̐ݒ�
			g_Explosion[nCntExplosion].nIdxShadow = CreateShadow(g_Explosion[nCntExplosion].pos, 0.5f, 0.5f);

			nIdxExplosion = nCntExplosion;

			break;
		}
	}

	return nIdxExplosion;
}

//=============================================================================
// explosion�̎擾
//=============================================================================
EXPLOSION* GetExplosion()
{
	return &g_Explosion[0];
}

