#include <d3d11.h>
#include <d3d11on12.h>
#include <d3d12.h>

#include <iostream>

static PFN_D3D11ON12_CREATE_DEVICE getPfnD3D11On12CreateDevice() {
	static PFN_D3D11ON12_CREATE_DEVICE fptr = nullptr;

	if (fptr)
		return fptr;

	HMODULE d3d11module = LoadLibraryA("C:\\Windows\\System32\\d3d11.dll");

	if (!d3d11module)
		return nullptr;

	fptr = reinterpret_cast<PFN_D3D11ON12_CREATE_DEVICE>(
		GetProcAddress(d3d11module, "D3D11On12CreateDevice"));
	return fptr;
}



HRESULT __stdcall D3D11CreateDevice(
	IDXGIAdapter            *pAdapter,
	D3D_DRIVER_TYPE         DriverType,
	HMODULE                 Software,
	UINT                    Flags,
	const D3D_FEATURE_LEVEL *pFeatureLevels,
	UINT                    FeatureLevels,
	UINT                    SDKVersion,
	ID3D11Device            **ppDevice,
	D3D_FEATURE_LEVEL       *pFeatureLevel,
	ID3D11DeviceContext     **ppImmediateContext) {
	ID3D12Device* d3d12device;

	HRESULT hr = D3D12CreateDevice(pAdapter,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&d3d12device));
	
	if (FAILED(hr))
		return hr;

	D3D12_COMMAND_QUEUE_DESC desc;
	desc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask	= 0;

	ID3D12CommandQueue* d3d12queue;

	hr = d3d12device->CreateCommandQueue(&desc,
		IID_PPV_ARGS(&d3d12queue));

	if (FAILED(hr))
		return hr;

	auto pfnD3D11on12CreateDevice = getPfnD3D11On12CreateDevice();

	if (!pfnD3D11on12CreateDevice)
		return E_FAIL;

	hr = (*pfnD3D11on12CreateDevice)(d3d12device,
		Flags, pFeatureLevels, FeatureLevels,
		reinterpret_cast<IUnknown**>(&d3d12queue),
		1, 0, ppDevice, ppImmediateContext, pFeatureLevel);
	
	d3d12device->Release();
	return hr;
}


HRESULT __stdcall D3D11CreateDeviceAndSwapChain(
	IDXGIAdapter*         pAdapter,
	D3D_DRIVER_TYPE       DriverType,
	HMODULE               Software,
	UINT                  Flags,
	const D3D_FEATURE_LEVEL*    pFeatureLevels,
	UINT                  FeatureLevels,
	UINT                  SDKVersion,
	const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	IDXGISwapChain**      ppSwapChain,
	ID3D11Device**        ppDevice,
	D3D_FEATURE_LEVEL*    pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext) {
	if (ppSwapChain && !pSwapChainDesc)
		return E_INVALIDARG;

	ID3D11Device*        d3d11Device;
	ID3D11DeviceContext* d3d11Context;

	HRESULT status = D3D11CreateDevice(pAdapter, DriverType,
		Software, Flags, pFeatureLevels, FeatureLevels,
		SDKVersion, &d3d11Device, pFeatureLevel, &d3d11Context);

	if (FAILED(status))
		return status;

	if (ppSwapChain) {
		IDXGIDevice*  dxgiDevice = nullptr;
		IDXGIAdapter* dxgiAdapter = nullptr;
		IDXGIFactory* dxgiFactory = nullptr;

		HRESULT hr = d3d11Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
		
		if (FAILED(hr))
			return E_INVALIDARG;

		hr = dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter));
		dxgiDevice->Release();

		if (FAILED(hr))
			return E_INVALIDARG;

		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		dxgiAdapter->Release();

		if (FAILED(hr))
			return E_INVALIDARG;

		DXGI_SWAP_CHAIN_DESC desc = *pSwapChainDesc;
		hr = dxgiFactory->CreateSwapChain(d3d11Device, &desc, ppSwapChain);
		dxgiFactory->Release();

		if (FAILED(hr))
			return hr;
	}

	if (ppDevice != nullptr)
		*ppDevice = d3d11Device;
	else
		d3d11Device->Release();

	if (ppImmediateContext != nullptr)
		*ppImmediateContext = d3d11Context;
	else
		d3d11Context->Release();

	return S_OK;
}