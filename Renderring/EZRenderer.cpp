#include "EZRenderer.h"
#include "Helper.h"

EZ::Renderer::Renderer(HWND windowHandle, RendererSettings settings) {
	_settings = settings;

	if (_settings.OptimizeForSingleThread) {
		if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_factory) != S_OK) {
			PrintLastError();
		}
	}
	else {
		if (D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &_factory) != S_OK) {
			PrintLastError();
		}
	}

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
	if (_settings.DpiX > 0.0f) {
		renderTargetProperties.dpiX = _settings.DpiX;
	}
	if (_settings.DpiY > 0.0f) {
		renderTargetProperties.dpiY = _settings.DpiY;
	}
	if (_settings.RequireLatestDX) {
		renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_10;
	}
	else {
		renderTargetProperties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	}
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(); // TODO
	if (_settings.RenderMode == EZ::RendererMode::Software) {
		renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_SOFTWARE;
	}
	else if (_settings.RenderMode == EZ::RendererMode::Hardware) {
		renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	}
	else {
		renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	}
	renderTargetProperties.usage = D2D1_RENDER_TARGET_USAGE_NONE;

	D2D1_HWND_RENDER_TARGET_PROPERTIES windowRenderTargetProperties = D2D1::HwndRenderTargetProperties(windowHandle);
	// windowRenderTargetProperties.hwnd = windowHandle;
	if (_settings.FixedSizeBuffer) {
		windowRenderTargetProperties.pixelSize = D2D1::SizeU(_settings.BufferWidth, _settings.BufferHeight);
	}
	else {
		RECT output;
		GetWindowRect(_windowHandle, &output);
		windowRenderTargetProperties.pixelSize = D2D1::SizeU(static_cast<UINT32>(output.right - output.left), static_cast<UINT32>(output.bottom - output.top));
	}
	// Always use the D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS option because it's speedy to not clear the buffer.
	windowRenderTargetProperties.presentOptions = D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS;
	if (_settings.UseVSync) {
		windowRenderTargetProperties.presentOptions |= D2D1_PRESENT_OPTIONS_IMMEDIATELY;
	}

	if (_factory->CreateHwndRenderTarget(renderTargetProperties, windowRenderTargetProperties, &_windowRenderTarget) != S_OK) {
		PrintLastError();
	}
}
void EZ::Renderer::BeginDraw() {
	if (!_settings.FixedSizeBuffer) {
		RECT output;
		GetWindowRect(_windowHandle, &output);
		_windowRenderTarget->Resize(D2D1::SizeU(static_cast<UINT32>(output.right - output.left), static_cast<UINT32>(output.bottom - output.top)));
	}
	_windowRenderTarget->BeginDraw();
}
void EZ::Renderer::Clear(D2D1_COLOR_F color) {
	_windowRenderTarget->Clear(color);
}
ID2D1Bitmap* EZ::Renderer::LoadBitmap(Asset asset) {
	// TODO
}
ID2D1Bitmap* EZ::Renderer::LoadBitmap(void* buffer, INT32 width, INT32 height, D2D1_PIXEL_FORMAT pixelFormat) {
	// TODO
}
void EZ::Renderer::DrawBitmap(ID2D1Bitmap* bitmap, INT32 x, INT32 y) {
	D2D1_SIZE_F size = bitmap->GetSize();
	float width = size.width;
	float height = size.height;

	_windowRenderTarget->DrawBitmap(
		bitmap,
		D2D1::RectF(x, y, x + width, y + height),
		1,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		D2D1::RectF(0, 0, width, height)
	);
}
void EZ::Renderer::EndDraw() {
	_windowRenderTarget->EndDraw();
}
EZ::Renderer::~Renderer() {
	_windowRenderTarget->Release();
	_factory->Release();
}

HWND EZ::Renderer::GetWindowHandle() const {
	return _windowHandle;
}
ID2D1Factory* EZ::Renderer::GetFactory() const {
	return _factory;
}
ID2D1HwndRenderTarget* EZ::Renderer::GetWindowRenderTarget() const {
	return _windowRenderTarget;
}
EZ::RendererSettings EZ::Renderer::GetSettings() const {
	return _settings;
}