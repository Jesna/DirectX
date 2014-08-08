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

	// 保存垂直同步设置
	m_vsync_enabled = vsync;

	// 创建一个DirectX graphics 接口工厂
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
		return false;

	// 用接口工厂创建一个主显卡的适配
	result =factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
		return false;

	// 得到主适配器的输出
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
		return false;

	//得到适合 DXGI_FORMAT_R8G8B8A8_UNORM 的显示模式
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if(FAILED(result)) 
		return false; 
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
		return false;

	// 保存显示模式到displayModeList中
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
		return false;

	// 遍历所有显示模式，得到刷新率两个参数值numerator 和 denominator
	for (int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == screenWidth)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// 得到显卡描述 
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
		return false;

	// 保存显存大小
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//保存显卡描述串
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
		return false;

	// 释放显示模式列表
	delete[] displayModeList;
	displayModeList = nullptr;

	//释放适配器输出
	adapterOutput->Release();
	adapterOutput = nullptr;

	//释放适配器
	adapter->Release();
	adapter = nullptr;

	//释放接口工厂
	factory->Release();
	factory = nullptr;

	//初始化交换链描述
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 用1个后缓冲
	swapChainDesc.BufferCount = 1;

	// 帧缓冲的大小和应用程序窗口大小相等
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 后缓冲的surface的格式为DXGI_FORMAT_R8G8B8A8_UNORM
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 如果使用垂直同步，设置后缓冲的刷新率
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

	// 设置后缓冲的用途
	// 我们的渲染目标缓冲为后缓冲
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 后缓冲输出的窗口句柄
	swapChainDesc.OutputWindow = hwnd;

	// 不使用多重采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 设置全屏或者窗口模式
	if(fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// 设定扫描线ordering以及缩放为unspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 后缓冲内容呈现到屏幕后，放弃其内容
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//不设置标志
	swapChainDesc.Flags = 0;

	// 设置feature level为D3D11 
	// 如果显卡不支持D3D11,我们能够通过设置这个参数，使用D3D10,或者9
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 创建交换链，设备以及设备上下文
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, 
		&featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	//得到 D3D11CreateDeviceAndSwapChain函数内部使用接口工厂
	IDXGIDevice* pDXGIDevice;
	result = m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	IDXGIAdapter* pDXGIAdapter;
	result = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	IDXGIFactory* pDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);

	//禁止alt+enter全屏
	pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

	// 释放资源
	pDXGIFactory->Release();
	pDXGIFactory = nullptr;

	pDXGIAdapter->Release();
	pDXGIAdapter = nullptr;

	pDXGIDevice->Release();
	pDXGIDevice = nullptr;

	// 得到交换链中的后缓冲指针
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// 用后缓冲创建渲染目标视图
	result = m_device->CreateRenderTargetView(backBufferPtr, nullptr, &m_renderTargetView);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	//释放后缓冲(引用计数减1)
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	// 初始化深度缓冲描述
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//设置深度缓冲描述
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;	//对于深度缓冲为1
	depthBufferDesc.ArraySize = 1;	//对于深度缓冲为1，对于纹理，这2个参数有更多用途
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 创建深度缓冲
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// 初始化深度模版状态描述
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 设置深度模版状态描述
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	// D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 对于front face 像素使用的模版操作操作
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 对于back face像素使用的模版操作模式
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//创建深度模版状态
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// 设置深度模版状态，使其生效
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 初始化深度模版视图
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 设置深度模版视图描述
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 创建深度模版视图
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	 // 绑定渲染目标视图和深度缓冲到渲染管线
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 设置光栅化描述，指定多边形如何被渲染
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

	// 创建光栅化状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		HR(result);
		return false;
	}

	// 设置光栅化状态，使其生效
	m_deviceContext->RSSetState(m_rasterState);

	// 设置视口，显示全部后缓冲内容
	viewport.Width = (FLOAT)screenWidth;
	viewport.Height = (FLOAT)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 创建视口
	m_deviceContext->RSSetViewports(1, &viewport);

	// 设置透视投影矩阵
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 创建透视投影矩阵
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//初始化world矩阵为单位矩阵
	//该矩阵实现局部坐标到世界坐标的转换
	D3DXMatrixIdentity(&m_worldMatrix);

	// 创建正交投影矩阵，主要用来实施2D渲染
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::ShutDown()
{
	// 释放交换链资源前，先设置为窗口模式，否则可能会产生异常
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

	// 设置清除后缓冲颜色
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除后缓冲
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//清除深度缓冲
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	//渲染完成后，把后缓冲内容present到前缓冲
	if(m_vsync_enabled)
		// 锁定屏幕刷新率
		m_swapChain->Present(1, 0);
	else
		// 尽可能快的present后缓冲
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