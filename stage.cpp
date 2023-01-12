//=============================================================================
//
// �X�e�[�W���f������ [Stage.cpp]
// Author : ���@�E��
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
// �}�N����`
//*****************************************************************************
#define	MODEL_Stage			"data/MODEL/map.obj"		// �ǂݍ��ރ��f����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static stage			g_Stage[MAX_stage];				// �X�e�[�W

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
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

		g_Stage[i].use = true;			// true:�����Ă�

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateStage(void)
{
	for (int i = 0; i < MAX_stage; i++)
	{

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStage(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_stage; i++)
	{
		if (g_Stage[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Stage[i].scl.x, g_Stage[i].scl.y, g_Stage[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Stage[i].rot.x, g_Stage[i].rot.y + XM_PI, g_Stage[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Stage[i].pos.x, g_Stage[i].pos.y, g_Stage[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Stage[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Stage[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �X�e�[�W�̎擾
//=============================================================================
stage* GetStage()
{
	return &g_Stage[0];
}
