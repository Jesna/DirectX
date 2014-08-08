#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"

GraphicsClass::GraphicsClass(void)
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_ColorShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{

}

GraphicsClass::~GraphicsClass(void)
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//��������Ѿ����ڣ����ͷŵ�����
	ShutDown();

	// ����һ��D3DClass����
	m_D3D = new D3DClass;
	if(!m_D3D)
		return false;

	// ����D3DClass��ʼ������ 
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, 
		SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, TEXT("Could not initialize Direct3D"), TEXT("Error"), MB_OK); 
		return false; 
	}

	//�������������
	m_Camera = new CameraClass;
	if(!m_Camera)
		return false;

	// ���������λ��
	D3DXVECTOR3 campos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_Camera->setPosition(&campos);

	// ����ģ�Ͷ���
	m_Model = new ModelClass;
	if(!m_Model)
		return false;

	// ��ʼ��ģ�Ͷ���
	result = m_Model->Initialize(m_D3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, TEXT("Could not initialize the model object."), TEXT("Error"), MB_OK);
		return false;
	}

	// ����shader����
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
		return false;

	// ��ʼ��shader����
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, TEXT("Could not initialize the color shader object."), TEXT("Error"), MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::ShutDown()
{
	// �ͷ�shader����
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	// �ͷ�ģ�Ͷ���
	if(m_Model)
	{
		m_Model->ShutDown();
		delete m_Model;
		m_Model = nullptr;
	}

	// �ͷ����������
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	//����m_D3D���� 
	if(m_D3D)
	{
		m_D3D->ShutDown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	//����Render��������Ⱦ3D����
	bool result = Render();
	if(!result)
		return false;

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	// ����framebuffer.Ϊǳ��ɫ
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);

	// �õ�3������
	m_Camera->getViewMatrix(&viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// ��ģ�Ͷ�����������������ߣ�׼����Ⱦ
	m_Model->Render(m_D3D->GetDeviceContext());

	// ��shader��Ⱦ
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
		return false;

	//��framebuffer�е�ͼ��present����Ļ��
	m_D3D->EndScene();

	return true;
}