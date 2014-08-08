#include "AxisModelClass.h"
#include "../common/common.h"

AxisModelClass::AxisModelClass(void)
{
}

AxisModelClass::AxisModelClass(const AxisModelClass&)
{
}

AxisModelClass::~AxisModelClass(void)
{
}

bool AxisModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// 初始化顶点缓冲和顶点索引缓冲
	result = InitializeBuffers(device);
	if(!result)
		return false;

	return true;
}

void AxisModelClass::Shutdown()
{
	// 释放顶点和索引缓冲
	ShutdownBuffers();
}

void AxisModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 把顶点和索引缓冲放入图形管线，准备渲染
	RenderBuffers(deviceContext);
}

int AxisModelClass::GetIndexCount()
{
	//返回索引顶点计数
	return m_indexCount;
}

bool AxisModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 设置顶点缓冲大小为6
	m_vertexCount = 6;

	// 设置索引缓冲大小
	m_indexCount = 6;

	// 创建顶点临时缓冲
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
		return false;

	// 创建索引缓冲
	indices = new unsigned long[m_indexCount];
	if(!indices)
		return false;

	// 设置顶点数据
	// X轴，红色
	vertices[0].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].color = RED;

	vertices[1].position = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	vertices[1].color = RED;

	// Y轴，绿色
	vertices[2].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[2].color = GREEN;

	vertices[3].position = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	vertices[3].color = GREEN;

	// Z轴，蓝色
	vertices[4].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[4].color = GREEN;

	vertices[5].position = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	vertices[5].color = GREEN;

	// 设置索引缓冲数据
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	// 设置顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 指向保存顶点数据的临时缓冲
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
		return false;

	// 设置索引缓冲描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 指向存临时索引缓冲
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
		return false;

	// 释放临时缓冲
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void AxisModelClass::ShutdownBuffers()
{
	// 释放索引缓冲
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// 释放顶点缓冲
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

void AxisModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride, offset;

	// 设置顶点缓冲跨度和偏移
	stride = sizeof(VertexType);
	offset = 0;

	//在input assemberl阶段绑定顶点缓冲，以便能够被渲染
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//在input assemberl阶段绑定索引缓冲，以便能够被渲染
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置体元语义，渲染线段，画出坐标轴
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}