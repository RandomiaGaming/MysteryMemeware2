#include "EZRenderer.h"
#include "Helper.h"

EZ::Renderer::Renderer(HWND windowHandle, RendererSettings settings) {
	if (!IsWindow(windowHandle)) {
		throw new Error("windowHandle must be a valid HWND. (Window may be destroyed)");
	}
	_windowHandle = windowHandle;
	_settings = settings;
	_resizeState = ResizeState::Idle;
	_resizeRequestWidth = DefaultRendererWidth;
	_resizeRequestHeight = DefaultRendererHeight;

	if (_settings.OptimizeForSingleThread) {
		if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_factory)) {
			ThrowSysError();
		}
	}
	else {
		if (D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &_factory)) {
			ThrowSysError();
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

	D2D1_HWND_RENDER_TARGET_PROPERTIES windowRenderTargetProperties = D2D1::HwndRenderTargetProperties(_windowHandle);
	// windowRenderTargetProperties.hwnd = _windowHandle;
	if (_settings.BufferWidth == 0) {
		_settings.BufferWidth = DefaultRendererWidth;
	}
	if (_settings.BufferHeight == 0) {
		_settings.BufferHeight = DefaultRendererHeight;
	}
	windowRenderTargetProperties.pixelSize = D2D1::SizeU(_settings.BufferWidth, _settings.BufferHeight);
	// Always use the D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS option because it's speedy to not clear the buffer.
	windowRenderTargetProperties.presentOptions = D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS;
	if (!_settings.UseVSync) {
		windowRenderTargetProperties.presentOptions |= D2D1_PRESENT_OPTIONS_IMMEDIATELY;
	}

	if (_factory->CreateHwndRenderTarget(renderTargetProperties, windowRenderTargetProperties, &_windowRenderTarget) != S_OK) {
		ThrowSysError();
	}
}
void EZ::Renderer::BeginDraw() {
	if (_resizeState == ResizeState::Requested) {
		D2D1_SIZE_U newSize = D2D1::SizeU(_resizeRequestWidth, _resizeRequestHeight);
		_windowRenderTarget->Resize(newSize);
		_resizeState = ResizeState::SizeChanged;
	}
	_windowRenderTarget->BeginDraw();
}
void EZ::Renderer::Clear(D2D1_COLOR_F color) {
	_windowRenderTarget->Clear(color);
}
void EZ::Renderer::RequestSize(UINT32 newWidth, UINT32 newHeight) {
	_resizeRequestWidth = newWidth;
	_resizeRequestHeight = newHeight;
	_resizeState = ResizeState::Requested;
	while (_resizeState != ResizeState::Idle) { }
}
void EZ::Renderer::EndDraw() {
	_windowRenderTarget->EndDraw();
	if (_resizeState == ResizeState::SizeChanged) {
		_resizeState = ResizeState::Idle;
	}
}
EZ::Renderer::~Renderer() {
	_windowRenderTarget->Release();
	_factory->Release();
}

HWND EZ::Renderer::GetWindowHandle() const {
	return _windowHandle;
}
EZ::RendererSettings EZ::Renderer::GetSettings() const {
	return _settings;
}