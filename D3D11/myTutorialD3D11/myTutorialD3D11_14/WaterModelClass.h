#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

class WaterModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	WaterModelClass(void);
	WaterModelClass(const WaterModelClass&);
	~WaterModelClass(void);

	bool Initialize(ID3D11Device*, int, int, float, float, float, float);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	bool update(ID3D11DeviceContext*, float);
	void disturb(int, int, float);

private:
	bool InitializeBuffers(ID3D11Device*, int, int, float, float, float, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//二维网格的行数、列数
	int m_NumRows, m_NumCols;

	// 欲计算的模拟常量
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	D3DXVECTOR3* mPrevSolution;
	D3DXVECTOR3* mCurrSolution;

	//顶点临时缓冲
	VertexType* vertices;
};

