#include "WaterModelClass.h"
#include "../common/common.h"
#include <vector>
using namespace std;

WaterModelClass::WaterModelClass(void)
{
	mPrevSolution = nullptr;
	mCurrSolution = nullptr;
	vertices = nullptr;
}

WaterModelClass::WaterModelClass(const WaterModelClass&)
{
}

WaterModelClass::~WaterModelClass(void)
{
}

bool WaterModelClass::Initialize(ID3D11Device* device, int m, int n, float dx, float dt, float speed, float damping)
{
	bool result;

	// 初始化顶点缓冲和顶点索引缓冲
	result = InitializeBuffers(device, m, n, dx, dt, speed, damping);
	if(!result)
		return false;

	return true;
}

void WaterModelClass::Shutdown()
{
	// 释放顶点和索引缓冲
	ShutdownBuffers();
}

void WaterModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 把顶点和索引缓冲放入图形管线，准备渲染
	RenderBuffers(deviceContext);
}

int WaterModelClass::GetIndexCount()
{
	//返回索引顶点计数
	return m_indexCount;
}

//damping表示阻尼系数
bool WaterModelClass::InitializeBuffers(ID3D11Device* device, int m, int n, float dx, float dt, float speed, float damping)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_NumRows = m;
	m_NumCols = n;

	//首先得到三角形的数目，然后乘以3就是顶点索引数目
	m_vertexCount = m * n;
	m_indexCount = (m - 1) * (n - 1) * 2 * 3;

	mTimeStep = dt;
	mSpatialStep = dx;

	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	mK1 = (damping * dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f * e) / d;
	mK3 = (2.0f * e) / d;

	mPrevSolution = new D3DXVECTOR3[m * n];
	mCurrSolution = new D3DXVECTOR3[m * n];

	float halfWidth = (n - 1) * dx * 0.5f;
	float halfDepth = (m - 1) * dx * 0.5f;
	for (int i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dx;
		for (int j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			mPrevSolution[i * n + j] = D3DXVECTOR3(x, 0.0f, z);
			mCurrSolution[i * n + j] = D3DXVECTOR3(x, 0.0f, z);
		}
	}

	//如果顶点临时缓冲不为空，先释放
	if(vertices)
	{
		// 释放临时缓冲
		delete[] vertices;
		vertices = nullptr;
	}

	// 创建顶点临时缓冲.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// 创建索引缓冲.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// 迭代每个grid，计算得出索引.
	int k = 0;
	for(int i = 0; i < m-1; ++i)
	{
		for(int j = 0; j < n-1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; //下一个grid
		}
	}

	// 设置顶点缓冲描述，动态缓冲
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags =D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// 指向保存顶点数据的临时缓冲.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// 创建顶点缓冲
	result = device->CreateBuffer(&vertexBufferDesc,&vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// 设置索引缓冲描述.
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE; //创建后资源从不变化，系统可能会有优化操作
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向存临时索引缓冲.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	delete[] indices;
	indices = nullptr;

	return true;
}

void WaterModelClass::ShutdownBuffers()
{
	// 释放顶点缓冲.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// 释放索引缓冲
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	//释放water顶点位置
	if(mPrevSolution)
	{
		delete[] mPrevSolution;
		mPrevSolution = nullptr;
	}

	if(mCurrSolution)
	{
		delete[] mCurrSolution;
		mCurrSolution = nullptr;
	}

	if(vertices)
	{
		// 释放临时缓冲.
		delete[] vertices;
		vertices = 0;
	}
}

void WaterModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// 设置顶点缓冲跨度和偏移.
	stride = sizeof(VertexType); 
	offset = 0;

	//在input assemberl阶段绑定顶点缓冲，以便能够被渲染
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//在input assemberl阶段绑定索引缓冲，以便能够被渲染
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置体元语义，渲染三角形列表.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool WaterModelClass::update(ID3D11DeviceContext* deviceContext, float dt)
{
	static float t = 0;

	// 累加时间
	t += dt;

	// 在指定的时间点更新模拟
	if(t >= mTimeStep)
	{
		// 仅更新迭代点
		for (int i = 1; i < m_NumRows - 1; ++i)
		{
			for (int j = 1; j < m_NumCols - 1; ++j)
			{
				//更新后，放弃mPrevSolution内容，用新产生的缓冲覆盖它
				//j索引x，i索引z
				mPrevSolution[i * m_NumCols + j].y = 
					mK1 * mPrevSolution[i * m_NumCols + j].y + 
					mK2 * mCurrSolution[i * m_NumCols + j].y +
					mK3 * (mCurrSolution[(i+1) * m_NumCols + j].y + 
					mCurrSolution[(i - 1) * m_NumCols + j].y + 
					mCurrSolution[i * m_NumCols + j + 1].y +
					mCurrSolution[i * m_NumCols + j - 1].y);
			}
		}

		//交换缓冲
		swap(mPrevSolution, mCurrSolution);

		// 复位时间
		t = 0.0f;

		// 更新顶点缓冲
		for (int i = 0; i < m_vertexCount; ++i)
		{
			vertices[i].position = mCurrSolution[i];
			vertices[i].color = BLUE;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		VertexType* verticesPtr;
		HRESULT result;

		// 锁定顶点缓冲以便能写
		result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			HR(result);
			return false;
		}

		// 得到一个顶点缓冲的指针
		verticesPtr = (VertexType*)mappedResource.pData;

		// 把新生成的顶点数据拷贝到顶点缓冲
		memcpy(verticesPtr, (void*)vertices, sizeof(VertexType) * m_vertexCount);

		// 解锁顶点缓冲
		deviceContext->Unmap(m_vertexBuffer, 0);
	}

	return true;
}

void WaterModelClass::disturb(int i, int j, float magnitude)
{
	// 不在边界处扰动
	assert(i > 1 && i < m_NumRows - 2);
	assert(j > 1 && j < m_NumCols - 2);

	float halfMag = 0.5f * magnitude;

	// 扰动第ij个顶点的高度和它的邻接点
	mCurrSolution[i * m_NumCols + j].y += magnitude;
	mCurrSolution[i * m_NumCols + j + 1].y += halfMag;
	mCurrSolution[i * m_NumCols + j - 1].y += halfMag;
	mCurrSolution[(i+1)* m_NumCols + j].y += halfMag;
	mCurrSolution[(i-1) * m_NumCols + j].y += halfMag;
}