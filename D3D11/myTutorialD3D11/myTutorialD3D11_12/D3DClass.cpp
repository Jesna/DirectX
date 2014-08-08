#include "D3DClass.h"
#include "../common/common.h"

D3DClass::D3DClass(void)
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
}

D3DClass::~D3DClass(void)
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
						  float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator = 0, denominator = 1, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	// ���洹ֱͬ������
	m_vsync_enabled = vsync;

	// ����һ��DirectX graphics �ӿڹ���
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// �ýӿڹ�������һ�����Կ�������
	result =factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// �õ��������������
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	//�õ��ʺ� DXGI_FORMAT_R8G8B8A8_UNORM ����ʾģʽ
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if(FAILED(result)) 
	{
		HR(result);
		return false;
	}
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
		return false;

	// ������ʾģʽ��displayModeList��
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// ����������ʾģʽ���õ�ˢ������������ֵnumerator �� denominator
	for (int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == screenWidth)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// �õ��Կ����� 
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// �����Դ��С
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//�����Կ�������
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
		return false;

	//�����Ļ�߶Ȼ��߿��Ϊ0����ᴴ����Ȼ���ʧ�ܣ�
	//��������С��ʱ�򣬻���ִ���Ϊ0�������
	if(screenWidth < 1)
		screenWidth = 1;
	if(screenHeight < 1)
		screenHeight = 1;

	//��ʼ������������
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ��1���󻺳�
	swapChainDesc.BufferCount = 1;

	// ֡����Ĵ�С��Ӧ�ó��򴰿ڴ�С���
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// �󻺳��surface�ĸ�ʽΪDXGI_FORMAT_R8G8B8A8_UNORM
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ���ʹ�ô�ֱͬ�������ú󻺳��ˢ����
	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ���ú󻺳����;
	// ���ǵ���ȾĿ�껺��Ϊ�󻺳�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �󻺳�����Ĵ��ھ��
	swapChainDesc.OutputWindow = hwnd;

	// ��ʹ�ö��ز���
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ����ȫ�����ߴ���ģʽ
	if(fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// �趨ɨ����ordering�Լ�����Ϊunspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// �󻺳����ݳ��ֵ���Ļ�󣬷���������
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�����ñ�־
	swapChainDesc.Flags = 0;

	// ����feature levelΪD3D11 
	// ����Կ���֧��D3D11,�����ܹ�ͨ���������������ʹ��D3D10,����9
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//�ֿ�����device�ͽ���������������dxgi�Զ���hook alt+enter 
	//ע������㣺����õ�һ��������NULL,�ڶ�������ΪD3D_DRIVER_TYPE_HARDWARE������洴��swapchain 
	//��fail�����ֱ����adapter�����ı�ڶ��������������ʾ�����豸ʧ�ܣ�adapter��Ч��
	result = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, 
		&featureLevel, 1, D3D11_SDK_VERSION, &m_device, nullptr, &m_deviceContext);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	//����������
	result = factory->CreateSwapChain(m_device, &swapChainDesc, &m_swapChain);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	//��ֹDXGI������Ϣ���У���׽ALT+ENTER����ȫ���ʹ���ģʽ֮���л�
	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	// �ͷ���ʾģʽ�б�
	delete[] displayModeList;
	displayModeList = nullptr;

	//�ͷ����������.
	adapterOutput->Release();
	adapterOutput = nullptr;

	//�ͷ�������
	adapter->Release();
	adapter = nullptr;

	// �ͷŽӿڹ���.
	factory->Release();
	factory = nullptr;

	// �õ��������еĺ󻺳�ָ��
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// �ú󻺳崴����ȾĿ����ͼ
	result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	//�ͷź󻺳�(���ü�����1)
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	// ��ʼ����Ȼ�������
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//������Ȼ�������
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;	//������Ȼ���Ϊ1
	depthBufferDesc.ArraySize = 1;	//������Ȼ���Ϊ1������������2�������и�����;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ������Ȼ���
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// ��ʼ�����ģ��״̬����
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// �������ģ��״̬����
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	// D3D11_DEPTH_WRITE_MASK_ZERO��ֹд��Ȼ���

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// ����front face ����ʹ�õ�ģ���������
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ����back face����ʹ�õ�ģ�����ģʽ
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�������ģ��״̬
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// �������ģ��״̬��ʹ����Ч
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ��ʼ�����ģ����ͼ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// �������ģ����ͼ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// �������ģ����ͼ
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	 // ����ȾĿ����ͼ����Ȼ��嵽��Ⱦ����
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// ���ù�դ��������ָ���������α���Ⱦ
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ������դ��״̬
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// ���ù�դ��״̬��ʹ����Ч
	m_deviceContext->RSSetState(m_rasterState);

	// �����ӿڣ���ʾȫ���󻺳�����
	viewport.Width = (FLOAT)screenWidth;
	viewport.Height = (FLOAT)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// �����ӿ�
	m_deviceContext->RSSetViewports(1, &viewport);

	// ����͸��ͶӰ����
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// ����͸��ͶӰ����
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//��ʼ��world����Ϊ��λ����
	//�þ���ʵ�־ֲ����굽���������ת��
	D3DXMatrixIdentity(&m_worldMatrix);

	// ��������ͶӰ������Ҫ����ʵʩ2D��Ⱦ
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::ShutDown()
{
	// �ͷŽ�������Դǰ��������Ϊ����ģʽ��������ܻ�����쳣
	if(m_swapChain)
		m_swapChain->SetFullscreenState(FALSE, nullptr);

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// ��������󻺳���ɫ
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//����󻺳�
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//�����Ȼ���
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	//��Ⱦ��ɺ󣬰Ѻ󻺳�����present��ǰ����
	if(m_vsync_enabled)
		// ������Ļˢ����
		m_swapChain->Present(1, 0);
	else
		// �����ܿ��present�󻺳�
		m_swapChain->Present(0, 0);
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_worldMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}