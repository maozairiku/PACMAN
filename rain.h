//=============================================================================
//
// �p�[�e�B�N������ [rain.h]
// Author : ���E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitRain(void);
void UninitRain(void);
void UpdateRain(void);
void DrawRain(void);

int SetRain(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorRain(int nIdxRain, XMFLOAT4 col);

