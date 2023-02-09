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
#include "tutorial.h"
#include "ui.h"
#include "dot.h"
#include "explosion.h"
#include "pexplosion.h"
#include "field.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "sky.h"
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

static float g_Time;	// Time set (init->�ŏ��̎��ԁ@/�@Update->���ԉ��Z�@/�@g_Time->�����Ɖ��Z���Ă��܂��̂ŁAinit�ł͂Ȃ��A�O���[�o���ϐ��ɐݒ肵�܂��B)


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_Time = 0.0f;

	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	g_bPause = true;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ���wfield�̏�����
	InitField();

	// camera work init
	InitCamera();

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

	// sky
	InitSky();

	// �h�b�g�̏�����
	InitDot();

	// �؂𐶂₷
	InitTree();

	// �e�̏�����
	InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// explosion
	InitExplosion();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// �����p�[�e�B�N���̏�����
	InitPExplosion();

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

	// sky
	UninitSky();

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

	// ���wfield�̏I������
	UninitField();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// explosion
	UninitExplosion();

	// particle explosion
	UninitPExplosion();

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
	// Set time
	SetTime(g_Time);		// g_time ������ 0.0f�ƂȂ��Ă��܂�
	g_Time++;				// ���Z�J�n

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

	if (g_bPause == false)
	{
		// explosion
		//UpdateExplosion();

		// particle explosion
		UpdatePExplosion();

		return;
	}

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// ���wfield�̍X�V
	UpdateField();

	// sky
	UpdateSky();

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

	// set explosion
	UpdateExplosion();

	// particle explosion
	UpdatePExplosion();

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// ���wfield�̕`�揈��
	DrawField();

	// sky
	DrawSky();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// explosion
	DrawExplosion();

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

	// particle explosion
	DrawPExplosion();

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

	PLAYER *player = GetPlayer();			// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = GetBullet();			// �e�̃|�C���^�[��������

	DOT *cookies = GetCookies();
	DOT *hotdog = GetHotdog();
	DOT *cherry = GetCherry();
	DOT *bread = GetBread();
	DOT *croissant = GetCroissant();

	EXPLOSION *explosion = GetExplosion();
	CAMERA *camera = GetCamera();

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
	}

	// �v���C���[�ƃh�b�g
	for (int d = 0; d < MAX_DOT; d++)
	{
		// cookies
		//BC�̓����蔻��
		if (CollisionBC(player->pos, cookies[d].pos, player->size, cookies[d].size))
		{
			// �H�ׂ�ꂽ��A���W���Z�b�g
			//cookies[d].use = false;
			//ReleaseShadow(cookies[d].shadowIdx);
			cookies[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//�_�����Z
			AddScore(150);
		}


		// hotdog
		//BC�̓����蔻��
		if (CollisionBC(player->pos, hotdog[d].pos, player->size, hotdog[d].size))
		{
			// �H�ׂ�ꂽ��A���W���Z�b�g
			//hotdog[d].use = false;
			//ReleaseShadow(hotdog[d].shadowIdx);
			hotdog[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//�_�����Z
			AddScore(150);
		}


		// cherry
		//BC�̓����蔻��
		if (CollisionBC(player->pos, cherry[d].pos, player->size, cherry[d].size))
		{
			// �H�ׂ�ꂽ��A���W���Z�b�g
			//cherry[d].use = false;
			//ReleaseShadow(cherry[d].shadowIdx);
			cherry[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//�_�����Z
			AddScore(150);
		}

		// bread
		//BC�̓����蔻��
		if (CollisionBC(player->pos, bread[d].pos, player->size, bread[d].size))
		{
			// �H�ׂ�ꂽ��A���W���Z�b�g
			//bread[d].use = false;
			//ReleaseShadow(bread[d].shadowIdx);
			bread[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//�_�����Z
			AddScore(150);
		}

		// croissant
		//BC�̓����蔻��
		if (CollisionBC(player->pos, croissant[d].pos, player->size, croissant[d].size))
		{
			// �H�ׂ�ꂽ��A���W���Z�b�g
			//croissant[d].use = false;
			//ReleaseShadow(croissant[d].shadowIdx);
			croissant[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//�_�����Z
			AddScore(150);
		}
	}

	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].use == false)
			continue;

			//BC�̓����蔻��
			if (CollisionBC(bullet[i].pos, player->pos, bullet[i].fWidth, player->size))
			{
				// �����������疢�g�p�ɖ߂�
				bullet[i].use = false;
				ReleaseShadow(bullet[i].shadowIdx);

				// player die
				player->use = false;
				ReleaseShadow(player->shadowIdx);

				// Set Pause
				SetPause(false);

				// camera shake
				SetShake();
				
				// particle explosion
				SetExplosionParticle();
				
				// to result
				SetFade(FADE_OUT, MODE_RESULT);
			}
	}
}

//=============================================================================
// Set Pause
//=============================================================================
void SetPause(BOOL pause)
{
	g_bPause = pause;
}

