#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	ModelClass(void);
	ModelClass(const ModelClass&);
	~ModelClass(void);

	bool Initialize(ID3D11Device*, int, int, float);
	void ShutDown();
	void Render(ID3D11DeviceContext*);
	
	int GetIndexCount();
	/*根据顶点的x值和z值，计算出y值*/
	float getHeight(float x, float z) const;

private:
	bool InitializeBuffers(ID3D11Device*, int, int, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	//顶点缓冲和顶点索引缓冲
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};