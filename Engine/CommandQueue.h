#pragma once
#include "SwapChain.h"
class SwapChain;
class DescriptorHeap;

//*******************************
//GraphicsCommandQueue
//*******************************

/*
	ID3DCommandQueue는 DX12에 나온 것으로
	GPU에는 명령 대기열(Command Queue)이 있다.
	CPU는 명령 목록(Command List)를 DirectX3D API를 통해 대기열을 제출한다.
	GPU는 목록을 바로 받는 것이 아닌, 이전에 제출된 명령들을 처리한 이후에 받는다.
	단, 명령 대기열이 비면GPU는 할일을 잃고, 대기열이 꽉차면 CPU가 놀게된다.
	게임에서는 이러한 상황을 바람직하지 않게본다.
	제출 방법 : CommandQueue의 ExcuteCommandList()를 통해 GPU에 Command(명령)한다.
	생성 방법 : CreateCommandQueue()
	필수 인자 : D3D12_Command_Queue_Desc
*/

/*
	ID3D12CommanAllocator
	CommandQueue 내부에 있는 Command List들을 할당한다.
	Allocator와 List의 비율은 1:1
*/

/*
	ID3DCommandList
	List의 함수들은 Setter, Draw(그리기 호출)
	List를 만들기 위해서는 Allocator 인터페이스 포인터 필요
	Open/Close를 이용해 명령 추가, 추가 불가 상태 이후 명령 제출
*/


class GraphicsCommandQueue
{
private:

	ComPtr<ID3D12CommandQueue> _cmdQueue;		//
	ComPtr<ID3D12CommandAllocator> _cmdAlloc;	//일감 할당(helper)
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	//일감 list


	//-------------resource Node용
	ComPtr<ID3D12CommandAllocator> _resCmdAlloc;
	ComPtr<ID3D12GraphicsCommandList> _resCmdList;

	//cpu, gpu 동기화 도구, CommandQueue와 같이 사용
	//모든 일처리가 끝날 때 까지 대기?..
	ComPtr<ID3D12Fence> _fence;
	uint32 _fenceValue{ 0 };
	HANDLE _fenceEvent{ INVALID_HANDLE_VALUE };

	shared_ptr<SwapChain> _swapChain;
 
public:

	~GraphicsCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetGraphicsCmdList() { return _cmdList; }

	ComPtr<ID3D12GraphicsCommandList> GetResourceCmdList() { return _resCmdList; }

public:

	void Init(
		ComPtr<ID3D12Device> device,
		shared_ptr<SwapChain> swapChain
	);



	void WaitSync();

	void RenderBegin();
	void RenderEnd();


	void FlushResourceCommandQueue();
};

//*******************************
//Compute CommandQueue
//*******************************

class ComputeCommandQueue
{
public:
	~ComputeCommandQueue();

public:

	void Init(ComPtr<ID3D12Device> device);
	void WaitSync();
	void FlushComputeCommandQueue();

public:

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:

	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};