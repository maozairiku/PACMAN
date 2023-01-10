//=============================================================================
//
// UI���� [ui.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "ui.h"
#include "player.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define UI_ITEM_MAX					(3)				// UI�̐�



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/UI/pacmanhp.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
static Ui		g_UI[UI_ITEM_MAX];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUi(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		g_UI[i].use = FALSE;

		g_UI[i].w = TEXTURE_WIDTH;
		g_UI[i].h = TEXTURE_HEIGHT;
		g_UI[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_UI[i].texNo = 0;

		g_UI[i].scrl = 0.0f;		// TEX�X�N���[��
		g_UI[i].scrl2 = 0.0f;		// TEX�X�N���[��
	}

	switch (GetMode())
	{
	case MODE_TITLE:

		g_UI[0].use = true;
		g_UI[0].texNo = 2;
		g_UI[0].pos = XMFLOAT3(340.0f, 380.0f, 0.0f);
		g_UI[0].w = 50;
		g_UI[0].h = 50;

		break;
	}

	g_Load = TRUE;
	return S_OK;
}




//=============================================================================
// �I������
//=============================================================================
void UninitUi(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}




//=============================================================================
// �X�V����
//=============================================================================
void UpdateUi(void)
{
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		if (g_UI[i].use)
		{
			g_UI[i].old_pos = g_UI[i].pos;	// �P�t���O�̏���ۑ�

			switch (GetMode())
			{
			case MODE_TITLE:

				if (GetKeyboardTrigger(DIK_DOWN))
				{
					g_UI[i].pos.y += 95;
				}

				if (GetKeyboardTrigger(DIK_UP))
				{
					g_UI[i].pos.y -= 95;
				}

				if (g_UI[i].pos.y > 475)
				{
					g_UI[i].pos.y = 380;
				}

				if (g_UI[i].pos.y < 380)
				{
					g_UI[i].pos.y = 475;
				}

				break;
			}
		}
	}


	//g_BG.scrl -= 0.0f;		// 0.005f;		// �X�N���[��


#ifdef _DEBUG	// �f�o�b�O����\������

#endif

}




//=============================================================================
// �`�揈��
//=============================================================================
void DrawUi(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (GetMode())
		{
			if (player[i].use)
			{
				// ���~���̃Q�[�W�i�g�I�ȕ��j
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 100.0f;		// �Q�[�W�̕\���ʒuX
				float py = 500.0f;		// �Q�[�W�̕\���ʒuY
				float pw = 300.0f;		// �Q�[�W�̕\����
				float ph = 30.0f;		// �Q�[�W�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));


				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);


				// �G�l�~�[�̐��ɏ]���ăQ�[�W�̒�����\�����Ă݂�
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_MAX - 1]);

				//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
				pw = pw * ((float)player[i].HP / PLAYER_HP_MAX);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// �p�b�N�}��(HP)�`��
				{
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
					SetSpriteColor(g_VertexBuffer, px - 80, py - 25, 80, 80, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
					GetDeviceContext()->Draw(4, 0);
				}
			}
		}
	}



#ifdef _DEBUG	// �f�o�b�O����\������

#endif
}

Ui* GetUi(void)
{
	return &g_UI[0];
}
