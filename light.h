//=============================================================================
//
// ���C�g���� [light.h]
// Author : ���@�E��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
BOOL GetFogEnable(void);

LIGHT *GetLightData(int index);

