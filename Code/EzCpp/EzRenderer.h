// Approved 11/14/2024

#pragma once
#include <Windows.h>
#include <D2D1.h>
#include <D2D1_1Helper.h>
#include <DXGI.h>

struct EzBitmapAsset {
	FLOAT DpiX;
	FLOAT DpiY;
	DXGI_FORMAT PixelFormat;
	D2D1_ALPHA_MODE AlphaMode;
	UINT32 Width;
	UINT32 Height;
	const BYTE* Buffer;
};

// Converts a rectangle which is in pixel space and defines a 2d rectangular area on a texture or render target
// into a rectangle that is in dip space and represents the same area but with DPI accounted for.
// Additionally accounts for the spacial shift in y axis between game engines and text/renderring engines.
// Finally uses the size of the texture/render target in both pixels and DIPs to preform scaling.
D2D1_RECT_F EzTransformRect(D2D1_RECT_L source, D2D1_SIZE_F dipSize, D2D1_SIZE_U pixelSize);

// These methods allow users to create rects with x, y, width, and height instead of left, top, right, and bottom.
D2D1_RECT_F EzRectF(FLOAT x, FLOAT y, FLOAT width, FLOAT height);
D2D1_RECT_L EzRectL(INT32 x, INT32 y, INT32 width, INT32 height);
D2D1_RECT_U EzRectU(UINT32 x, UINT32 y, UINT32 width, UINT32 height);

enum class EzRendererType : BYTE {
	// Allows DirectX to choose the renderer mode automatically.
	DontCare = 0,
	// Forces DirectX to use a software renderer.
	// This means graphics will be rendered completely on the CPU.
	// This offers maximum compatibility with systems that lack a GPU but at the cost of preformance.
	Software = 1,
	// Forces DirectX to use a hardware renderer.
	// This means all graphics will be rendered on the GPU.
	// This offers better preformance but may cause the glitches or crashes if the GPU
	// lacks features required by DirectX.
	Hardware = 2,
};
struct EzHwndRendererSettings {
	// Stores the width of the render buffer in pixels.
	// If BufferWidth == 0 then the width of the window is used.
	UINT32 BufferWidth;
	// Stores the height of the render buffer in pixels.
	// If BufferHeight == 0 then the height of the window is used.
	UINT32 BufferHeight;
	// Specifies the Dpi along the X axis.
	// If DpiX == 0 then the default DpiX of 96.0f is used.
	FLOAT DpiX;
	// Specifies the Dpi along the Y axis.
	// If DpiY == 0 then the default DpiY of 96.0f is used.
	FLOAT DpiY;
	// Specifies the pixel format the back buffer of the renderer uses.
	// By default this is DXGI_FORMAT_UNKNOWN which allows D2D1 to decide automatically.
	DXGI_FORMAT PixelFormat;
	// Specifies the alpha mode the the back buffer of the renderer uses.
	// By default this is D2D1_ALPHA_MODE_UNKNOWN which allows D2D1 to decide automatically.
	D2D1_ALPHA_MODE AlphaMode;
	// If OptimizeForSingleThread == TRUE then the DirectX renderer is set to single threaded mode.
	// This improves preformance but removes multi-thread protections and should only be used for applications
	// where the renderer will be used by one and only one thread.
	// Else the DirectX renderer is set to its default multi-thread safe mode.
	BOOL OptimizeForSingleThread;
	// If RequireLatestDX == TRUE then the latest version of DirectX is required by this renderer.
	// Else any version of DirectX is fine. This may cause some advanced features to be unavailible
	// however it is generally recommended not to require the latest DirectX unless necesary so older
	// software and hardware can be supported.
	BOOL RequireLatestDX;
	// Determines weather this is a software or hardware renderer.
	// See RendererMode enum for detailed info on each option.
	EzRendererType RendererType;
	// If UseVSync == TRUE then vertical sync is enabled for this renderer.
	// This causes the renderer to wait before presenting a frame for the physical display to be ready.
	// This artificially decreases FPS to match the monitor's refresh rate however it can help remove
	// visual artifacts such as tearing or flickering.
	// Else vertical sync is not used and frames are drawn as soon as they are rendered.
	// It is recommended to enable VSync for release builds for less artifacting and power consumption
	// but to disable VSync for debug builds so the true FPS potential of your app can be tested.
	BOOL UseVSync;
};
struct EzSwapChainRendererSettings {
	// Stores the width of the render buffer in pixels.
	// If BufferWidth == 0 then the width of the window is used.
	UINT32 BufferWidth;
	// Stores the height of the render buffer in pixels.
	// If BufferHeight == 0 then the height of the window is used.
	UINT32 BufferHeight;
	// Stores the refresh rate of the swap chain. 
	// If RefreshRate == 0 then the default refresh rate of the monitor is used.
	UINT32 RefreshRate;
	// Specifies the Dpi along the X axis.
	// If DpiX == 0 then the default DpiX of 96.0f is used.
	FLOAT DpiX;
	// Specifies the Dpi along the Y axis.
	// If DpiY == 0 then the default DpiY of 96.0f is used.
	FLOAT DpiY;
	// Specifies the pixel format the back buffer of the renderer uses.
	// By default this is DXGI_FORMAT_UNKNOWN which allows D2D1 to decide automatically.
	DXGI_FORMAT PixelFormat;
	// Specifies the alpha mode the the back buffer of the renderer uses.
	// By default this is D2D1_ALPHA_MODE_UNKNOWN which allows D2D1 to decide automatically.
	D2D1_ALPHA_MODE AlphaMode;
	// If OptimizeForSingleThread == TRUE then the DirectX renderer is set to single threaded mode.
	// This improves preformance but removes multi-thread protections and should only be used for applications
	// where the renderer will be used by one and only one thread.
	// Else the DirectX renderer is set to its default multi-thread safe mode.
	BOOL OptimizeForSingleThread;
	// If HyperthreadDeviceContext == TRUE then the ID2D1DeviceContext renders on multiple threads in parallel.
	// By default this is disabled for compatability reasons with older hardware.
	BOOL HyperthreadDeviceContext;
};

void EzCreateHwndRenderer(HWND windowHandle, const EzHwndRendererSettings* settings, ID2D1Factory** pFactory, ID2D1HwndRenderTarget** pWindowRenderTarget);
void EzCreateSwapChainRenderer(HWND windowHandle, const EzSwapChainRendererSettings* settings, ID2D1Factory1** pFactory, ID2D1RenderTarget** pRenderTarget, IDXGISwapChain** pSwapChain);
void EzDrawBitmap(ID2D1RenderTarget* windowRenderTarget, ID2D1Bitmap* bitmap, D2D1_POINT_2L position);
void EzDrawBitmap(ID2D1RenderTarget* windowRenderTarget, ID2D1Bitmap* bitmap, D2D1_RECT_L destination);
void EzDrawBitmap(ID2D1RenderTarget* windowRenderTarget, ID2D1Bitmap* bitmap, D2D1_RECT_L source, D2D1_RECT_L destination);
ID2D1Bitmap* EzLoadBitmap(ID2D1RenderTarget* windowRenderTarget, LPCWSTR filePath);
ID2D1Bitmap* EzLoadBitmap(ID2D1RenderTarget* windowRenderTarget, const EzBitmapAsset* asset);