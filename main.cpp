#include <ctime>
#include <windows.h> 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // callback functions of the window

// get screen metrics
int width  = GetSystemMetrics(SM_CXSCREEN),
	height = GetSystemMetrics(SM_CYSCREEN);

struct Shape {
	public:
		int left, top, right, bottom;
		Shape(int left, int top, int right, int bottom) {
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}
};

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPervInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow) {
	UNREFERENCED_PARAMETER(hPervInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MSG msg;
	TCHAR szClassName[] = "Tetris";
	HWND hMainWnd;
	WNDCLASSEX wc = { sizeof(wc), CS_HREDRAW | CS_VREDRAW, WndProc, NULL, NULL, hInstance, LoadIcon(NULL, IDI_WINLOGO),
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)CreateSolidBrush(RGB(9, 12, 18)), NULL, szClassName };
	if (!RegisterClassEx(&wc)) {
		return NULL;
	}
	// creating window
	hMainWnd = CreateWindow(szClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, width, height,
		(HWND)NULL, NULL, HINSTANCE(hInstance), NULL);
	if (!hMainWnd) {
		return NULL;
	}
	SetWindowLong(hMainWnd, GWL_STYLE, GetWindowLong(hMainWnd, GWL_STYLE) || WS_SYSMENU || WS_CAPTION);
	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);
	// main cycle message
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

HBRUSH hBrush_red  = CreateSolidBrush(RGB(155, 1, 34)),
	   hBrush_blue = CreateSolidBrush(RGB(32, 79, 146)),
	   hBrush_back = CreateSolidBrush(RGB(9, 12, 18));
Shape brick((width / 2) - 110, height - 100, (width / 2) + 110, height - 50), eat(0, 0, 0, 0);
RECT brick_rect = { brick.left, brick.top, brick.right, brick.bottom },
	 eat_rect   = { eat.left, eat.top, eat.right, height - 100 };
enum PaintType { brickPaint, eatPaint, lastPaint } paintType;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg) {
		case WM_CREATE:
			srand(time(NULL));
			eat.left = rand() % width;
			if (eat.left <= 30) 
				eat.left += 30;
			if (eat.left >= width - 30)
				eat.left -= 30;
			eat.right = eat.left + 30;
			eat.top = 0;
			eat.bottom = 30;
			SetTimer(hWnd, 1, 20, 0);
			break;
		case WM_TIMER:
			if (eat.bottom + 8 <= brick.top + 2) {
				eat.top += 8;
				eat.bottom += 8;
				paintType = eatPaint;
				eat_rect = { eat.left, eat.top - 8, eat.right, eat.bottom };
				InvalidateRect(hWnd, &eat_rect, TRUE);
			}
			else {
				eat.left = rand() % (width - 50);
				eat.right = eat.left + 30;
				eat.top = 0;
				eat.bottom = 30;
				paintType = lastPaint;
				InvalidateRect(hWnd, &eat_rect, TRUE);
			}
			break;
		case WM_KEYDOWN:
			paintType = brickPaint;
			if (LOWORD(wParam) == VK_LEFT && brick.left - 20 >= 20) {
				brick.left -= 20;
				brick.right -= 20;
				brick_rect = { brick.left, brick.top, brick.right + 20, brick.bottom };
				InvalidateRect(hWnd, &brick_rect, TRUE);
			}
			else if (LOWORD(wParam) == VK_RIGHT && brick.right + 20 <= width - 20) {
				brick.left += 20;
				brick.right += 20;
				brick_rect = { brick.left - 20, brick.top, brick.right, brick.bottom };
				InvalidateRect(hWnd, &brick_rect, TRUE);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (paintType == eatPaint) {
				SelectObject(hdc, hBrush_red);
				RoundRect(hdc, eat.left, eat.top, eat.right, eat.bottom, 20, 20);
			}
			else if (paintType == brickPaint) {
				SelectObject(hdc, hBrush_blue);
				RoundRect(hdc, brick.left, brick.top, brick.right, brick.bottom, 40, 40);
			}
			else if (paintType == lastPaint) {
				SelectObject(hdc, hBrush_back);
				RoundRect(hdc, eat.left, eat.top, eat.right, eat.bottom, 20, 20);
			}
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(NULL);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return NULL;
}