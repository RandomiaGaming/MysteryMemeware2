#include "EzRenderer.h"
#include "EzError.h"
#include <D3D11.h>
#include <wincodec.h>
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "windowscodecs.lib")

D2D1_RECT_F EzTransformRect(D2D1_RECT_L source, D2D1_SIZE_F dipSize, D2D1_SIZE_U pixelSize) {
	FLOAT left = (static_cast<FLOAT>(source.left) * dipSize.width) / pixelSize.width;
	FLOAT top = dipSize.height - ((static_cast<FLOAT>(source.top) * dipSize.height) / pixelSize.height);
	FLOAT right = (static_cast<FLOAT>(source.right) * dipSize.width) / pixelSize.width;
	FLOAT bottom = dipSize.height - ((static_cast<FLOAT>(source.bottom) * dipSize.height) / pixelSize.height);
	return D2D1::RectF(left, top, right, bottom);
}

D2D1_RECT_F EzRectF(FLOAT x, FLOAT y, FLOAT width, FLOAT height) {
	return D2D1::RectF(x, y + height, x + width, y);
}
D2D1_RECT_L EzRectL(INT32 x, INT32 y, INT32 width, INT32 height) {
	return D2D1::RectL(x, y + height, x + width, y);
}
D2D1_RECT_U EzRectU(UINT32 x, UINT32 y, UINT32 width, UINT32 height) {
	return D2D1::RectU(x, y + height, x + width, y);
}

void EzCreateHwndRenderer(HWND windowHandle, const EzHwndRendererSettings* settings, ID2D1Factory** pFactory, ID2D1HwndRenderTarget** pWindowRenderTarget) {
	if (!IsWindow(windowHandle)) {
		throw EzError(L"windowHandle must be a valid HWND.", __FILE__, __LINE__);
	}

	D2D1_FACTORY_TYPE factoryType = D2D1_FACTORY_TYPE_MULTI_THREADED;
	if (settings->OptimizeForSingleThread) {
		factoryType = D2D1_FACTORY_TYPE_SINGLE_THREADED;
	}
	ID2D1Factory1* factory = NULL;
	EzError::ThrowFromHR(D2D1CreateFactory(factoryType, &factory), __FILE__, __LINE__);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = { };
	if (settings->RendererType == EzRendererType::Software) {
		renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_SOFTWARE;
	}
	else if (settings->RendererType == EzRendererType::Hardware) {
		renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	}
	else {
		renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	}
	renderTargetProperties.pixelFormat.format = settings->PixelFormat;
	renderTargetProperties.pixelFormat.alphaMode = settings->AlphaMode;
	if (settings->DpiX == 0.0f) {
		renderTargetProperties.dpiX = 96.0f;
	}
	else {
		renderTargetProperties.dpiX = settings->DpiX;
	}
	if (settings->DpiY == 0.0f) {
		renderTargetProperties.dpiY = 96.0f;
	}
	else {
		renderTargetProperties.dpiY = settings->DpiY;
	}
	renderTargetProperties.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	if (settings->RequireLatestDX) {
		renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_10;
	}
	else {
		renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	}

	D2D1_HWND_RENDER_TARGET_PROPERTIES windowRenderTargetProperties = { };
	windowRenderTargetProperties.hwnd = windowHandle;
	RECT windowRect = { };
	if (!GetWindowRect(windowHandle, &windowRect)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (settings->BufferWidth == 0) {
		windowRenderTargetProperties.pixelSize.width = windowRect.right - windowRect.left;
	}
	else {
		windowRenderTargetProperties.pixelSize.width = settings->BufferWidth;
	}
	if (settings->BufferHeight == 0) {
		windowRenderTargetProperties.pixelSize.height = windowRect.bottom - windowRect.top;
	}
	else {
		windowRenderTargetProperties.pixelSize.height = settings->BufferHeight;
	}
	windowRenderTargetProperties.presentOptions = D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS;
	if (!settings->UseVSync) {
		windowRenderTargetProperties.presentOptions |= D2D1_PRESENT_OPTIONS_IMMEDIATELY;
	}

	ID2D1HwndRenderTarget* windowRenderTarget = NULL;
	EzError::ThrowFromHR(factory->CreateHwndRenderTarget(renderTargetProperties, windowRenderTargetProperties, &windowRenderTarget), __FILE__, __LINE__);

	if (pFactory == NULL) {
		factory->Release();
	}
	else {
		*pFactory = factory;
	}
	if (pWindowRenderTarget == NULL) {
		windowRenderTarget->Release();
	}
	else {
		*pWindowRenderTarget = windowRenderTarget;
	}
}
void EzCreateSwapChainRenderer(HWND windowHandle, const EzSwapChainRendererSettings* settings, ID2D1Factory1** pFactory, ID2D1RenderTarget** pRenderTarget, IDXGISwapChain** pSwapChain) {
	/* KNOWN ISSUE:
	Fullscreen optimizations is a lie. Not only is it slightly slower than not using fullscreen
	optimizations but it also defeats the entire purpose of exclusive fullscreen mode.
	The whole point of using exclusive fullscreen instead of just a boarderless fullscreen
	window is to have exclusive control over the display. With fullscreen optimizations enabled
	apps that request exclusive fullscreen access will be lied to and told that they achived this
	level of access when in fact they are simply rendering to a normal boarderless window.
	https://devblogs.microsoft.com/directx/demystifying-full-screen-optimizations/
	*/


	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = settings->BufferWidth;
	swapChainDesc.BufferDesc.Height = settings->BufferHeight;
	if (settings->RefreshRate == 0) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = settings->RefreshRate;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	if (settings->PixelFormat != DXGI_FORMAT_UNKNOWN) {
		swapChainDesc.BufferDesc.Format = settings->PixelFormat;
	}
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	swapChainDesc.SampleDesc.Count = 1; // The default sampler mode, with no anti-aliasing, has a count of 1 and a quality level of 0.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.Windowed = FALSE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0; // From DXGI_SWAP_CHAIN_FLAG

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG; // Required for interop with D2D1.
	if (settings->OptimizeForSingleThread) {
		createDeviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	}
	ID3D11Device* d3dDevice = NULL;
	IDXGISwapChain* swapChain = NULL;
	EzError::ThrowFromHR(D3D11CreateDeviceAndSwapChain(NULL, // No specific adapter.
		D3D_DRIVER_TYPE_HARDWARE, NULL, // Use hardware driver not custom driver.
		createDeviceFlags,
		NULL, 0, // Don't care which feature level is selected.
		D3D11_SDK_VERSION, // Use latest SDK version
		&swapChainDesc, &swapChain, &d3dDevice, NULL, NULL), __FILE__, __LINE__);

	D2D1_FACTORY_TYPE factoryType = D2D1_FACTORY_TYPE_MULTI_THREADED;
	if (settings->OptimizeForSingleThread) {
		factoryType = D2D1_FACTORY_TYPE_SINGLE_THREADED;
	}
	ID2D1Factory1* factory = NULL;
	EzError::ThrowFromHR(D2D1CreateFactory(factoryType, &factory), __FILE__, __LINE__);

	IDXGIDevice* dxgiDevice = NULL;
	EzError::ThrowFromHR(d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)), __FILE__, __LINE__);
	d3dDevice->Release();

	ID2D1Device* device = NULL;
	EzError::ThrowFromHR(factory->CreateDevice(dxgiDevice, &device), __FILE__, __LINE__);
	dxgiDevice->Release();

	D2D1_DEVICE_CONTEXT_OPTIONS deviceContextOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
	if (settings->HyperthreadDeviceContext) {
		deviceContextOptions = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
	}
	ID2D1DeviceContext* deviceContext = NULL;
	EzError::ThrowFromHR(device->CreateDeviceContext(deviceContextOptions, &deviceContext), __FILE__, __LINE__);
	device->Release();

	IDXGISurface* dxgiBackBuffer = NULL;
	EzError::ThrowFromHR(swapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&dxgiBackBuffer)), __FILE__, __LINE__);

	D2D1_BITMAP_PROPERTIES1 backBufferProperties = { };
	backBufferProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	backBufferProperties.colorContext = NULL;
	backBufferProperties.dpiX = 96.0f; // TODO
	backBufferProperties.dpiY = 96.0f; // TODO
	backBufferProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	if (settings->PixelFormat != DXGI_FORMAT_UNKNOWN) {
		backBufferProperties.pixelFormat.format = settings->PixelFormat;
	}
	backBufferProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	if (settings->AlphaMode != D2D1_ALPHA_MODE_UNKNOWN) {
		backBufferProperties.pixelFormat.alphaMode = settings->AlphaMode;
	}
	ID2D1Bitmap1* backBuffer = NULL;
	EzError::ThrowFromHR(deviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &backBufferProperties, &backBuffer), __FILE__, __LINE__);
	dxgiBackBuffer->Release();

	deviceContext->SetTarget(backBuffer);
	backBuffer->Release();

	ID2D1RenderTarget* renderTarget = NULL;
	EzError::ThrowFromHR(deviceContext->QueryInterface(__uuidof(ID2D1RenderTarget), reinterpret_cast<void**>(&renderTarget)), __FILE__, __LINE__);
	deviceContext->Release();

	if (pFactory == NULL) {
		factory->Release();
	}
	else {
		*pFactory = factory;
	}
	if (pRenderTarget == NULL) {
		renderTarget->Release();
	}
	else {
		*pRenderTarget = renderTarget;
	}
	if (pSwapChain == NULL) {
		swapChain->Release();
	}
	else {
		*pSwapChain = swapChain;
	}
}
void EzResizeSwapChain() {

}
void EzDrawBitmap(ID2D1RenderTarget* renderTarget, ID2D1Bitmap* bitmap, D2D1_POINT_2L position) {
	D2D1_SIZE_U bitmapSize = bitmap->GetPixelSize();
	D2D1_RECT_L rect = EzRectL(position.x, position.y, bitmapSize.width, bitmapSize.height);
	D2D1_RECT_F transRect = EzTransformRect(rect, renderTarget->GetSize(), renderTarget->GetPixelSize());
	renderTarget->DrawBitmap(bitmap, transRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
}
void EzDrawBitmap(ID2D1RenderTarget* renderTarget, ID2D1Bitmap* bitmap, D2D1_RECT_L destination) {
	D2D1_RECT_F transDestination = EzTransformRect(destination, renderTarget->GetSize(), renderTarget->GetPixelSize());
	renderTarget->DrawBitmap(bitmap, transDestination, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
}
void EzDrawBitmap(ID2D1RenderTarget* renderTarget, ID2D1Bitmap* bitmap, D2D1_RECT_L source, D2D1_RECT_L destination) {
	D2D1_RECT_F transSource = EzTransformRect(source, bitmap->GetSize(), bitmap->GetPixelSize());
	D2D1_RECT_F transDestination = EzTransformRect(destination, renderTarget->GetSize(), renderTarget->GetPixelSize());
	renderTarget->DrawBitmap(bitmap, transDestination, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, transSource);
}
ID2D1Bitmap* EzLoadBitmap(ID2D1RenderTarget* renderTarget, LPCWSTR filePath) {
	EzError::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	IWICImagingFactory* factory = NULL;
	EzError::ThrowFromHR(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)), __FILE__, __LINE__);

	IWICBitmapDecoder* decoder = NULL;
	EzError::ThrowFromHR(factory->CreateDecoderFromFilename(filePath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder), __FILE__, __LINE__);

	IWICBitmapFrameDecode* frame = NULL;
	EzError::ThrowFromHR(decoder->GetFrame(0, &frame), __FILE__, __LINE__);

	IWICFormatConverter* converter = NULL;
	EzError::ThrowFromHR(factory->CreateFormatConverter(&converter), __FILE__, __LINE__);

	EzError::ThrowFromHR(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom), __FILE__, __LINE__);

	ID2D1Bitmap* output = NULL;
	EzError::ThrowFromHR(renderTarget->CreateBitmapFromWicBitmap(converter, NULL, &output), __FILE__, __LINE__);

	converter->Release();
	frame->Release();
	decoder->Release();
	factory->Release();
	CoUninitialize();

	return output;
}
ID2D1Bitmap* EzLoadBitmap(ID2D1RenderTarget* renderTarget, const EzBitmapAsset* asset) {
	ID2D1Bitmap* output;
	D2D1_SIZE_U bitmapSize = D2D1::SizeU(asset->Width, asset->Height);
	D2D1_BITMAP_PROPERTIES bitmapProperties = { };
	FLOAT renderTargetDpiX = 0.0f;
	FLOAT renderTargetDpiY = 0.0f;
	renderTarget->GetDpi(&renderTargetDpiX, &renderTargetDpiY);
	if (asset->DpiX == -1.0f) {
		bitmapProperties.dpiX = renderTargetDpiX;
	}
	else {
		bitmapProperties.dpiX = asset->DpiX;
	}
	if (asset->DpiY == -1.0f) {
		bitmapProperties.dpiY = renderTargetDpiY;
	}
	else {
		bitmapProperties.dpiY = asset->DpiY;
	}
	bitmapProperties.pixelFormat.format = asset->PixelFormat;
	bitmapProperties.pixelFormat.alphaMode = asset->AlphaMode;
	EzError::ThrowFromHR(renderTarget->CreateBitmap(bitmapSize, asset->Buffer, asset->Width * 4, &bitmapProperties, &output), __FILE__, __LINE__);
	return output;
}