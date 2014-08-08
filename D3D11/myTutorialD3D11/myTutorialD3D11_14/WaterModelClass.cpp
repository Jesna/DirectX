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

	// ��ʼ�����㻺��Ͷ�����������
	result = InitializeBuffers(device, m, n, dx, dt, speed, damping);
	if(!result)
		return false;

	return true;
}

void WaterModelClass::Shutdown()
{
	// �ͷŶ������������
	ShutdownBuffers();
}

void WaterModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �Ѷ���������������ͼ�ι��ߣ�׼����Ⱦ
	RenderBuffers(deviceContext);
}

int WaterModelClass::GetIndexCount()
{
	//���������������
	return m_indexCount;
}

//damping��ʾ����ϵ��
bool WaterModelClass::InitializeBuffers(ID3D11Device* device, int m, int n, float dx, float dt, float speed, float damping)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_NumRows = m;
	m_NumCols = n;

	//���ȵõ������ε���Ŀ��Ȼ�����3���Ƕ���������Ŀ
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

	//���������ʱ���岻Ϊ�գ����ͷ�
	if(vertices)
	{
		// �ͷ���ʱ����
		delete[] vertices;
		vertices = nullptr;
	}

	// ����������ʱ����.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// ������������.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// ����ÿ��grid������ó�����.
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

			k += 6; //��һ��grid
		}
	}

	// ���ö��㻺����������̬����
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags =D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// ָ�򱣴涥�����ݵ���ʱ����.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// �������㻺��
	result = device->CreateBuffer(&vertexBufferDesc,&vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// ����������������.
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE; //��������Դ�Ӳ��仯��ϵͳ���ܻ����Ż�����
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ָ�����ʱ��������.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// ������������.
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
	// �ͷŶ��㻺��.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// �ͷ���������
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	//�ͷ�water����λ��
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
		// �ͷ���ʱ����.
		delete[] vertices;
		vertices = 0;
	}
}

void WaterModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// ���ö��㻺���Ⱥ�ƫ��.
	stride = sizeof(VertexType); 
	offset = 0;

	//��input assemberl�׶ΰ󶨶��㻺�壬�Ա��ܹ�����Ⱦ
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//��input assemberl�׶ΰ��������壬�Ա��ܹ�����Ⱦ
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ������Ԫ���壬��Ⱦ�������б�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool WaterModelClass::update(ID3D11DeviceContext* deviceContext, float dt)
{
	static float t = 0;

	// �ۼ�ʱ��
	t += dt;

	// ��ָ����ʱ������ģ��
	if(t >= mTimeStep)
	{
		// �����µ�����
		for (int i = 1; i < m_NumRows - 1; ++i)
		{
			for (int j = 1; j < m_NumCols - 1; ++j)
			{
				//���º󣬷���mPrevSolution���ݣ����²����Ļ��帲����
				//j����x��i����z
				mPrevSolution[i * m_NumCols + j].y = 
					mK1 * mPrevSolution[i * m_NumCols + j].y + 
					mK2 * mCurrSolution[i * m_NumCols + j].y +
					mK3 * (mCurrSolution[(i+1) * m_NumCols + j].y + 
					mCurrSolution[(i - 1) * m_NumCols + j].y + 
					mCurrSolution[i * m_NumCols + j + 1].y +
					mCurrSolution[i * m_NumCols + j - 1].y);
			}
		}

		//��������
		swap(mPrevSolution, mCurrSolution);

		// ��λʱ��
		t = 0.0f;

		// ���¶��㻺��
		for (int i = 0; i < m_vertexCount; ++i)
		{
			vertices[i].position = mCurrSolution[i];
			vertices[i].color = BLUE;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		VertexType* verticesPtr;
		HRESULT result;

		// �������㻺���Ա���д
		result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			HR(result);
			return false;
		}

		// �õ�һ�����㻺���ָ��
		verticesPtr = (VertexType*)mappedResource.pData;

		// �������ɵĶ������ݿ��������㻺��
		memcpy(verticesPtr, (void*)vertices, sizeof(VertexType) * m_vertexCount);

		// �������㻺��
		deviceContext->Unmap(m_vertexBuffer, 0);
	}

	return true;
}

void WaterModelClass::disturb(int i, int j, float magnitude)
{
	// ���ڱ߽紦�Ŷ�
	assert(i > 1 && i < m_NumRows - 2);
	assert(j > 1 && j < m_NumCols - 2);

	float halfMag = 0.5f * magnitude;

	// �Ŷ���ij������ĸ߶Ⱥ������ڽӵ�
	mCurrSolution[i * m_NumCols + j].y += magnitude;
	mCurrSolution[i * m_NumCols + j + 1].y += halfMag;
	mCurrSolution[i * m_NumCols + j - 1].y += halfMag;
	mCurrSolution[(i+1)* m_NumCols + j].y += halfMag;
	mCurrSolution[(i-1) * m_NumCols + j].y += halfMag;
}