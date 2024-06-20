#pragma once
#include <Windows.h>
#include <D2D1.h>
#pragma comment(lib, "D2D1.lib")

namespace EZ {
	enum RendererMode : BYTE {
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
	struct RendererSettings {
		// If FixedSizeBuffer == FALSE then the buffer resizes to always match the width and height of the window.
		// This resizing happens during the call to BeginDraw().
		// Else the buffer is a constant size defined by BufferWidth and BufferHeight.
		BOOL FixedSizeBuffer;
		UINT32 BufferWidth = 256;
		UINT32 BufferHeight = 144;
		// If OptimizeForSingleThread == TRUE then the DirectX renderer is set to single threaded mode.
		// This improves preformance but removes multi-thread protections and should only be used for applications
		// where the renderer will be used by one and only one thread.
		// Else the DirectX renderer is set to its default multi-thread safe mode.
		BOOL OptimizeForSingleThread;
		// DpiX and DpiY specify the dot's per inch for the x and y axis which allow DirectX to convert from inches to pixels and vise versa.
		// Values <= 0 are ignored and replaced with the default DPI.
		FLOAT DpiX;
		FLOAT DpiY;
		// If RequireLatestDX == TRUE then the latest version of DirectX is required by this renderer.
		// Else any version of DirectX is fine. This may cause some advanced features to be unavailible
		// however it is generally recommended not to require the latest DirectX unless necesary so older
		// software and hardware can be supported.
		BOOL RequireLatestDX;
		// Determines weather this is a software or hardware renderer.
		// See RendererMode enum for detailed info on each option.
		RendererMode RenderMode;
		// If UseVSync == TRUE then vertical sync is enabled for this renderer.
		// This causes the renderer to wait before presenting a frame for the physical display to be ready.
		// This artificially decreases FPS to match the monitor's refresh rate however it can help remove
		// visual artifacts such as tearing or flickering.
		// Else vertical sync is not used and frames are drawn as soon as they are rendered.
		// It is recommended to enable VSync for release builds for less artifacting and power consumption
		// but to disable VSync for debug builds so the true FPS potential of your app can be tested.
		BOOL UseVSync;
	};
	class Renderer {
	public:
		Renderer(HWND windowHandle, RendererSettings settings);
		// In addition to initializing the renderer and getting read to draw
		// this method also resizes the buffer if FixedSizeBuffer == FALSE.
		void BeginDraw();
		void Clear(D2D1_COLOR_F color);
		// Loads a bitmap from an EZAssetManager asset.
		ID2D1Bitmap* LoadBitmap(Asset asset);
		// Loads a bitmap the old fasioned way with a buffer, width, height, and a pixel format.
		ID2D1Bitmap* LoadBitmap(void* buffer, INT32 width, INT32 height, D2D1_PIXEL_FORMAT pixelFormat);
		void DrawBitmap(ID2D1Bitmap* bitmap, INT32 x, INT32 y);
		void EndDraw();
		~Renderer();

		HWND GetWindowHandle() const;
		ID2D1Factory* GetFactory() const; // Plz don't delete hehe.
		ID2D1HwndRenderTarget* GetWindowRenderTarget() const; // Plz don't delete hehe.
		RendererSettings GetSettings() const;

	private:
		HWND _windowHandle;
		ID2D1Factory* _factory;
		ID2D1HwndRenderTarget* _windowRenderTarget;
		RendererSettings _settings;
	};
}