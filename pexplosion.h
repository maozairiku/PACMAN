//=============================================================================
//
// �p�[�e�B�N������ [pxxplosion.h]
// Author : ���@�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPExplosion(void);
void UninitPExplosion(void);
void UpdatePExplosion(void);
void DrawPExplosion(void);

int SetPExplosion(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorPExplosion(int nIdxPExplosion, XMFLOAT4 col);
void SetExplosionParticle(void);

