//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "stage.h"
#include "tutorial.h"
#include "ui.h"
#include "dot.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static bool	g_bPause = true;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �ǂ̏�����
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// ��(�����p�̔�����)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// stage
	InitStage();

	// �؂𐶂₷
	InitTree();

	// �h�b�g�̏�����
	InitDot();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_GAMEMODE);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �p�[�e�B�N���̏I������
	UninitParticle();

	// �X�e�[�W�̏I������
	UninitStage();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	UninitBullet();

	// �h�b�g�̏I������
	UninitDot();

	// �؂̏I������
	UninitTree();

	// �ǂ̏I������
	UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? false : true;
	}


#endif

	if(g_bPause == false)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �X�e�[�W�̍X�V
	UpdateStage();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �؂̍X�V����
	UpdateTree();

	// �e�̍X�V����
	UpdateBullet();

	// �h�b�g�̍X�V����
	UpdateDot();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateScore();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �X�e�[�W�̕`�揈��
	DrawStage();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �e�̕`�揈��
	DrawBullet();

	//�h�b�g�̕`�揈��
	DrawDot();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	DrawTree();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();


	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(false);

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	// �X�R�A�̕`�揈��
	DrawScore();


	// ���C�e�B���O��L����
	SetLightEnable(true);

	// Z��r����
	SetDepthEnable(true);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		//pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		//pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *ghostred = GetGhostRed();		// �G�l�~�[�̃|�C���^�[��������
	ENEMY* ghostorange = GetGhostOrange();	
	ENEMY* ghostgreen = GetGhostGreen();	
	ENEMY* ghostblue = GetGhostBlue();		
	ENEMY* ghostpurple = GetGhostPurple();	
	PLAYER *player = GetPlayer();			// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();			// �e�̃|�C���^�[��������
	DOT *dot = GetDot();

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����(red)
		if (ghostred[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, ghostred[i].pos, player->size, ghostred[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			ghostred[i].use = false;
			ReleaseShadow(ghostred[i].shadowIdx);

			// Game Over
			SetFade(FADE_OUT, MODE_RESULT);
		}


		//�G�̗L���t���O���`�F�b�N����(orange)
		if (ghostorange[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, ghostorange[i].pos, player->size, ghostorange[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			ghostorange[i].use = false;
			ReleaseShadow(ghostorange[i].shadowIdx);

			// Game Over
			SetFade(FADE_OUT, MODE_RESULT);
		}

		//�G�̗L���t���O���`�F�b�N����(green)
		if (ghostgreen[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, ghostgreen[i].pos, player->size, ghostgreen[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			ghostgreen[i].use = false;
			ReleaseShadow(ghostgreen[i].shadowIdx);

			// Game Over
			SetFade(FADE_OUT, MODE_RESULT);
		}


		//�G�̗L���t���O���`�F�b�N����(blue)
		if (ghostblue[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, ghostblue[i].pos, player->size, ghostblue[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			ghostblue[i].use = false;
			ReleaseShadow(ghostblue[i].shadowIdx);

			// Game Over
			SetFade(FADE_OUT, MODE_RESULT);
		}


		//�G�̗L���t���O���`�F�b�N����(purple)
		if (ghostpurple[i].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, ghostpurple[i].pos, player->size, ghostpurple[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			ghostpurple[i].use = false;
			ReleaseShadow(ghostpurple[i].shadowIdx);

			// Game Over
			SetFade(FADE_OUT, MODE_RESULT);
		}

	}

	// �v���C���[�ƃh�b�g
	for (int d = 0; d < MAX_DOT; d++)
	{
		if (dot[d].use == false)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, dot[d].pos, player->size, dot[d].size))
		{
			// �h�b�g�͐H�ׂ���
			dot[d].use = false;
			ReleaseShadow(dot[d].shadowIdx);

			//�_�����Z
			AddScore(100);
		}
	}

	// �v���C���[�̒e�ƓG
	//for (int i = 0; i < MAX_BULLET; i++)
	//{
	//	//�e�̗L���t���O���`�F�b�N����
	//	if (bullet[i].use == false)
	//		continue;

	//	// �G�Ɠ������Ă邩���ׂ�
	//	for (int j = 0; j < MAX_ENEMY; j++)
	//	{
	//		//�G�̗L���t���O���`�F�b�N����
	//		if (ghostred[j].use == false)
	//			continue;

	//		//BC�̓����蔻��
	//		if (CollisionBC(bullet[i].pos, ghostred[j].pos, bullet[i].fWidth, ghostred[j].size))
	//		{
	//			// �����������疢�g�p�ɖ߂�
	//			bullet[i].use = false;
	//			ReleaseShadow(bullet[i].shadowIdx);

	//			// �G�L�����N�^�[�͓|�����
	//			ghostred[j].use = false;
	//			ReleaseShadow(ghostred[j].shadowIdx);

	//			// �X�R�A�𑫂�
	//			AddScore(10);
	//		}
	//	}

	//}


	// dot���S���Ȃ��Ȃ������ԑJ��
	int dot_count = 0;
	for (int i = 0; i < MAX_DOT; i++)
	{
		if (dot[i].use == false) continue;
		dot_count++;
	}

	// �G�l�~�[���O�C�H
	if (dot_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

}


