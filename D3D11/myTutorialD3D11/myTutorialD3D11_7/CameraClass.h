#pragma once

#include <D3DX10math.h>

class CameraClass
{
	//֧�����������ģ�� AIRCRAFT �����ڿռ������˶�,����6�����ɶ� 
	// LANDOBJECT ��ĳЩ�ض�������ƶ�
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };

public:
	CameraClass(void);
	CameraClass(const CameraClass&);
	~CameraClass(void);

	void strafe(float units);		//�����ƶ�
	void fly(float units);			//�����ƶ�
	void walk(float units);		//ǰ���ƶ�

	void pitch(float angle);		// ��תview����ϵright����
	void yaw(float angle);		// ��תup����
	void roll(float angle);			// ��תlook����

	void getViewMatrix(D3DXMATRIX* V);
	void setCameraType(CameraType cameraType);
	void getPosition(D3DXVECTOR3* pos);
	void setPosition(D3DXVECTOR3* pos);

	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void getLook(D3DXVECTOR3* look);

private:
	CameraType _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
};

