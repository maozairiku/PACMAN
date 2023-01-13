//=============================================================================
//
// �G�l�~�[���f������ [dot.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "dot.h"
#include "shadow.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_DOT			"data/MODEL/dot.obj"		// Ghost_Red

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define DOT_SHADOW_SIZE		(0.4f)						// �e�̑傫��
#define DOT_OFFSET_Y		(10.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DOT			g_Dot[MAX_DOT];				// �G�l�~�[

static BOOL			g_Load = FALSE;


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, DOT_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
	{ XMFLOAT3(-200.0f, DOT_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
	{ XMFLOAT3(-200.0f, DOT_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
	{ XMFLOAT3(   0.0f, DOT_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitDot(void)
{
	for (int i = 0; i < MAX_DOT; i++)
	{
		// Ghost_Red
		LoadModel(MODEL_DOT, &g_Dot[i].model);
		g_Dot[i].load = true;

		g_Dot[0].pos = XMFLOAT3(98.0f, DOT_OFFSET_Y, 30.0f);
		g_Dot[1].pos = XMFLOAT3(163.0f, DOT_OFFSET_Y, -5.0f);
		g_Dot[2].pos = XMFLOAT3(4.0f, DOT_OFFSET_Y, -331.0f);
		g_Dot[3].pos = XMFLOAT3(343.0f, DOT_OFFSET_Y, -494.0f);
		g_Dot[4].pos = XMFLOAT3(410.0f, DOT_OFFSET_Y, 44.0f);
		g_Dot[5].pos = XMFLOAT3(390.0f, DOT_OFFSET_Y, -224.0f);

		g_Dot[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Dot[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Dot[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Dot[i].size = DOT_SIZE;	// �����蔻��̑傫��
		g_Dot[i].colorIdx = 1;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Dot[0].model, &g_Dot[0].diffuse[0]);

		XMFLOAT3 rpos = g_Dot[i].pos;
		rpos.y -= (DOT_OFFSET_Y - 0.1f);
		g_Dot[i].shadowIdx = CreateShadow(rpos, DOT_SHADOW_SIZE, DOT_SHADOW_SIZE);

		g_Dot[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Dot[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Dot[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Dot[i].use = true;			// true:�����Ă�


		//// 0�Ԃ������`��Ԃœ������Ă݂�
		//g_Dot[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		//g_Dot[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Dot[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitDot(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_DOT; i++)
	{
		if (g_Dot[i].load)
		{
			UnloadModel(&g_Dot[i].model);
			g_Dot[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateDot(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_DOT; i++)
	{
		// Ghost_Red
		if (g_Dot[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_Dot[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Dot[i].move_time;
				float	time = g_Dot[i].move_time - index;
				int		size = g_Dot[i].tbl_size;

				float dt = 1.0f / g_Dot[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Dot[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Dot[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Dot[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Dot[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Dot[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Dot[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Dot[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Dot[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Dot[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Dot[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Dot[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Dot[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Dot[i].pos.y = HitPosition.y + DOT_OFFSET_Y;
			}
			else
			{
				g_Dot[i].pos.y = DOT_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Dot[i].pos;
			pos.y -= (DOT_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Dot[i].shadowIdx, pos);
		}

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDot(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_DOT; i++)
	{
		// Ghost Red
		if (g_Dot[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Dot[i].scl.x, g_Dot[i].scl.y, g_Dot[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Dot[i].rot.x, g_Dot[i].rot.y + XM_PI, g_Dot[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Dot[i].pos.x, g_Dot[i].pos.y, g_Dot[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Dot[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Dot[i].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
DOT* GetDot(void)
{
	return &g_Dot[0];
}

