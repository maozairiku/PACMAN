//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(1)				// �v���C���[�̐�

#define PLAYER_HP_MAX	(3)				// HP�ő吔

#define	PLAYER_SIZE		(5.0f)			// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	float				spd;				// �ړ��X�s�[�h
	int					HP;					// HP
	
	BOOL				load;
	DX11_MODEL			model;				// ���f�����

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	BOOL				use;

	float				size;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA	*tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	float				move_time;			// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	int					select;				// title select

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
