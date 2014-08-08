#include "ModelClass.h"

ModelClass::ModelClass(void)
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

ModelClass::ModelClass(const ModelClass&)
{

}

ModelClass::~ModelClass(void)
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// ��ʼ�����㻺��Ͷ�����������
	result = InitializeBuffers(device);
	if(!result)
		return false;

	return true;
}

void ModelClass::ShutDown()
{
	// �ͷŶ������������
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �Ѷ���������������ͼ�ι��ߣ�׼����Ⱦ
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	//���������������
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ���ö��㻺���СΪ3��һ��������
	m_vertexCount = 3;
	// �������������С
	m_indexCount = 3;

	// ����������ʱ����
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
		return false;

	// ������������
	indices = new unsigned long[m_indexCount];
	if(!indices)
		return false;

	// ���ö�������
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);	//����
	vertices[0].color = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		//����
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);		//����
	vertices[2].color = D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f);

	// ����������������
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// ���ö��㻺������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ָ�򱣴涥�����ݵ���ʱ����
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������㻺��
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
		return false;

	// ����������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	 // ָ�����ʱ��������
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// ������������
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
		return false;

	// �ͷ���ʱ����
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// �ͷŶ��㻺��
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
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride, offset;

	// ���ö��㻺���Ⱥ�ƫ��
	stride = sizeof(VertexType);
	offset = 0;

	//��input assemberl�׶ΰ󶨶��㻺�壬�Ա��ܹ�����Ⱦ
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//��input assemberl�׶ΰ��������壬�Ա��ܹ�����Ⱦ
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ������Ԫ���壬��Ⱦ�������б�
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}