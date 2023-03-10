#pragma once

//Rootsignature : 그리기 호출 전 응용프로그램이 반드시 Rendering Pipeline에 binding해야
//하는 자원들이 무엇이고, 그 자원들이 쉐이더 입력 레지스터들에 어떻게 대응하는지를 정의한다.
//

class RootSignature
{
private:

	D3D12_STATIC_SAMPLER_DESC _samplerDesc;

	ComPtr<ID3D12RootSignature> _graphicsRootSignature;
	ComPtr<ID3D12RootSignature> _computeRootSignature;

private:

	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

public:

	void Init();

	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return _computeRootSignature; }

};

