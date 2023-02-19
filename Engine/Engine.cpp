#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Light.h"

#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

#include "Resources.h"



void Engine::Init(const WindowInfo& info)
{
	_window = info;
	
	
	//그려질 화면 크기를 설정
	//viewport(topleftX, topleftY, width, height, MinDepth, MaxDepth)
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	//typedef RECT D3D12_RECT;
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDeivce(), _swapChain);
	_computeCmdQueue->Init(_device->GetDeivce());

	_swapChain->Init(info, _device->GetDeivce(), _device->getDXGI(), _graphicsCmdQueue->GetCmdQueue());
	_rootSignature->Init();
	//_cb->Init(sizeof(Transform), 256); 단일 contantbuffer
	_graphicsDescHeap->Init(256);
	_computeDescHeap->Init();

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256);
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);

	CreateRenderTargetGroups();

	ResizeWindow(info.width, info.height);

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();

}



void Engine::Update()
{
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();
	GET_SINGLE(SceneManager)->Update();

	Render();

	ShowFPS();
}



void Engine::Render()
{
	RenderBegin();

	GET_SINGLE(SceneManager)->Render();

	RenderEnd();
}

void Engine::RenderBegin()
{
	_graphicsCmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::RenderEnd()
{
	_graphicsCmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{

	_window.width = width;
	_window.height = height;

	//::표준 라이브러리 사용했다는 의미로
	RECT rect{ 0, 0, width, height };
	//AdjustwindowRect(클라이언트 영역, window style, menu(false))
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	//setwindowpos(어떤 윈도우인가, 윈도우의 위치 0 = 가장 앞, posX, posY, width, height, 창크기 조정)
	::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0);

}

void Engine::ShowFPS()
{
	uint32 fps = GET_SINGLE(Timer)->getFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(_window.hwnd, text);

}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
	uint8 typeInt = static_cast<uint8>(reg);
	assert(_constantBuffers.size() == typeInt);

	shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
	buffer->Init(reg, bufferSize, count);
	_constantBuffers.push_back(buffer);
}

void Engine::CreateRenderTargetGroups()
{
	//DepthStencil
	shared_ptr<Texture> dsTexture = GET_SINGLE(Resources)->CreateTexture(
		L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT,
		_window.width,
		_window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	//SwapChainGroup
	{
		vector<RenderTarget> rtVec(SWAP_CAHIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CAHIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + std::to_wstring(i);

			ComPtr<ID3D12Resource> resource;
			_swapChain->getSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]
			= make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(
			RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN,
			rtVec,
			dsTexture
		);
	}
	
	//Deferred Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(
			L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			_window.width,
			_window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);


		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(
			L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			_window.width,
			_window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(
			L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			_window.width,
			_window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]
			= make_shared<RenderTargetGroup>();

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(
			RENDER_TARGET_GROUP_TYPE::G_BUFFER,
			rtVec,
			dsTexture);
	}

	//Lighting Group 
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(
			L"DiffuseLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			_window.width,
			_window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(
			L"SpecularLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM,
			_window.width,
			_window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]
			= make_shared<RenderTargetGroup>();

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]
			->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, dsTexture);
	}
}
