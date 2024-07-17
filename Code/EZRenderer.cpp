#include "EZRenderer.h"
#include "Helper.h"
#include <wincodec.h>
#include <iostream>

#pragma comment(lib, "windowscodecs.lib")

// Converts a rectangle which is in pixel space and defines a 2d rectangular area on a texture or render target
// into a rectangle that is in dip space and represents the same area but with DPI accounted for.
// Additionally accounts for the spacial shift in y axis between game engines and text/renderring engines.
// Finally uses the size of the texture/render target in both pixels and DIPs to preform scaling.
D2D1_RECT_F TransformRect(D2D1_RECT_L source, D2D1_SIZE_F dipSize, D2D1_SIZE_U pixelSize) {
	FLOAT left = (static_cast<FLOAT>(source.left) * dipSize.width) / pixelSize.width;
	FLOAT top = dipSize.height - ((static_cast<FLOAT>(source.top) * dipSize.height) / pixelSize.height);
	FLOAT right = (static_cast<FLOAT>(source.right) * dipSize.width) / pixelSize.width;
	FLOAT bottom = dipSize.height - ((static_cast<FLOAT>(source.bottom) * dipSize.height) / pixelSize.height);
	return D2D1::RectF(left, top, right, bottom);
}

D2D1_RECT_F EZ::RectF(FLOAT x, FLOAT y, FLOAT width, FLOAT height) {
	return D2D1::RectF(x, y + height, x + width, y);
}
D2D1_RECT_L EZ::RectL(INT32 x, INT32 y, INT32 width, INT32 height) {
	return D2D1::RectL(x, y + height, x + width, y);
}
D2D1_RECT_U EZ::RectU(UINT32 x, UINT32 y, UINT32 width, UINT32 height) {
	return D2D1::RectU(x, y + height, x + width, y);
}

EZ::Renderer::Renderer(HWND windowHandle, EZ::RendererSettings settings) {
	if (!IsWindow(windowHandle)) {
		throw Error("windowHandle must be a valid HWND. (Window may be destroyed)");
	}
	_windowHandle = windowHandle;
	_settings = settings;

	if (_settings.OptimizeForSingleThread) {
		ThrowSysError(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_factory));
	}
	else {
		ThrowSysError(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &_factory));
	}

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
	// DPI is god awful and so we force it to be off.
	renderTargetProperties.dpiX = 96.0f;
	renderTargetProperties.dpiY = 96.0f;
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

	ThrowSysError(_factory->CreateHwndRenderTarget(renderTargetProperties, windowRenderTargetProperties, &_windowRenderTarget));
}
void EZ::Renderer::BeginDraw() {
	_windowRenderTarget->BeginDraw();
}
void EZ::Renderer::Clear(D2D1_COLOR_F color) {
	_windowRenderTarget->Clear(color);
}
void EZ::Renderer::FillRect(D2D1_RECT_L rect, D2D1_COLOR_F color) {
	ID2D1SolidColorBrush* brush;
	ThrowSysError(_windowRenderTarget->CreateSolidColorBrush(color, &brush));
	D2D1_RECT_F transRect = TransformRect(rect, _windowRenderTarget->GetSize(), _windowRenderTarget->GetPixelSize());
	_windowRenderTarget->FillRectangle(&transRect, brush);
	brush->Release();
}
void EZ::Renderer::Resize(D2D1_SIZE_U newSize) {
	ThrowSysError(_windowRenderTarget->Resize(newSize));
}
void EZ::Renderer::DrawBitmap(ID2D1Bitmap* bitmap, D2D1_POINT_2L position) {
	D2D1_SIZE_U bitmapSize = bitmap->GetPixelSize();
	D2D1_RECT_L rect = EZ::RectL(position.x, position.y, bitmapSize.width, bitmapSize.height);
	D2D1_RECT_F transRect = TransformRect(rect, _windowRenderTarget->GetSize(), _windowRenderTarget->GetPixelSize());
	_windowRenderTarget->DrawBitmap(bitmap, transRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
}
void EZ::Renderer::DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_L destination) {
	D2D1_RECT_F transDestination = TransformRect(destination, _windowRenderTarget->GetSize(), _windowRenderTarget->GetPixelSize());
	_windowRenderTarget->DrawBitmap(bitmap, transDestination, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
}
void EZ::Renderer::DrawBitmap(ID2D1Bitmap* bitmap, D2D1_RECT_L source, D2D1_RECT_L destination) {
	D2D1_RECT_F transSource = TransformRect(source, bitmap->GetSize(), bitmap->GetPixelSize());
	D2D1_RECT_F transDestination = TransformRect(destination, _windowRenderTarget->GetSize(), _windowRenderTarget->GetPixelSize());
	_windowRenderTarget->DrawBitmap(bitmap, transDestination, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, transSource);
}
ID2D1Bitmap* EZ::Renderer::LoadBitmap(LPCWSTR filePath) {
	ThrowSysError(CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY));

	IWICImagingFactory* factory = nullptr;
	ThrowSysError(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));

	IWICBitmapDecoder* decoder = nullptr;
	ThrowSysError(factory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));

	IWICBitmapFrameDecode* frame = nullptr;
	ThrowSysError(decoder->GetFrame(0, &frame));

	IWICFormatConverter* converter = nullptr;
	ThrowSysError(factory->CreateFormatConverter(&converter));

	ThrowSysError(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom));

	ID2D1Bitmap* output = nullptr;
	ThrowSysError(_windowRenderTarget->CreateBitmapFromWicBitmap(converter, nullptr, &output));

	converter->Release();
	frame->Release();
	decoder->Release();
	factory->Release();
	CoUninitialize();

	return output;
}
ID2D1Bitmap* EZ::Renderer::LoadBitmap(IStream* stream) {
	ThrowSysError(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	IWICImagingFactory* factory = nullptr;
	ThrowSysError(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));

	IWICBitmapDecoder* decoder = nullptr;
	ThrowSysError(factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder));

	IWICBitmapFrameDecode* frame = nullptr;
	ThrowSysError(decoder->GetFrame(0, &frame));

	IWICFormatConverter* converter = nullptr;
	ThrowSysError(factory->CreateFormatConverter(&converter));

	ThrowSysError(converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom));

	ID2D1Bitmap* output = nullptr;
	ThrowSysError(_windowRenderTarget->CreateBitmapFromWicBitmap(converter, nullptr, &output));

	converter->Release();
	frame->Release();
	decoder->Release();
	factory->Release();
	CoUninitialize();

	return output;
}
ID2D1Bitmap* EZ::Renderer::LoadBitmap(EZ::BitmapAsset asset) {
	ID2D1Bitmap* output;
	D2D1_SIZE_U bitmapSize = D2D1::SizeU(asset.Width, asset.Height);
	D2D1_BITMAP_PROPERTIES bitmapProperties = {};
	if (asset.DpiX == 0 && asset.DpiY == 0) {
		_windowRenderTarget->GetDpi(&bitmapProperties.dpiX, &bitmapProperties.dpiY);
	}
	else {
		bitmapProperties.dpiX = asset.DpiX;
		bitmapProperties.dpiY = asset.DpiY;
	}
	bitmapProperties.pixelFormat = {};
	bitmapProperties.pixelFormat.format = asset.PixelFormat;
	bitmapProperties.pixelFormat.alphaMode = asset.AlphaMode;
	ThrowSysError(_windowRenderTarget->CreateBitmap(bitmapSize, asset.Buffer, asset.Stride, &bitmapProperties, &output));
	return output;
}
void EZ::Renderer::EndDraw() {
	ThrowSysError(_windowRenderTarget->EndDraw());
}
D2D1_SIZE_U EZ::Renderer::GetSize() {
	return _windowRenderTarget->GetPixelSize();
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