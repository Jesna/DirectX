#pragma once

#include <D3DX10math.h>

class CameraClass
{
	//支持两种摄像机模型 AIRCRAFT 允许在空间自由运动,具有6个自由度 
	// LANDOBJECT 沿某些特定轴进行移动
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };

public:
	CameraClass(void);
	CameraClass(const CameraClass&);
	~CameraClass(void);

	void strafe(float units);		//左右移动
	void fly(float units);			//上下移动
	void walk(float units);		//前后移动

	void pitch(float angle);		// 旋转view坐标系right向量
	void yaw(float angle);		// 旋转up向量
	void roll(float angle);			// 旋转look向量

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

