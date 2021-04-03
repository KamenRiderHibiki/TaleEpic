#include "DirectX.h"
//#include "GameTimer.h"

using namespace std;

extern int SCREENW;
extern int SCREENH;
bool gameover = false;
GameTimer mTimer;
static WINDOWSTATE wstat;
HWND* hwnd;

void OnResize(HWND window)
{
	D3DPRESENT_PARAMETERS d3dpp;
	// D3DPRESENT_PARAMETERS is already defined as md3dPP
	//RECT R = { 0, 0, 640, 480 };
	//AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = (!wstat.mFullscreenState);
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferHeight = SCREENH;
	d3dpp.BackBufferWidth = SCREENW;
	d3dpp.hDeviceWindow = window;
	d3dpp.Windowed = true;
	d3ddev->Reset(&d3dpp);
	//D3D�豸
	(*d3d).CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (!d3ddev)
		return ;
	//��ȡ������ָ��
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	//��������ʵ��
	D3DXCreateSprite(d3ddev, &spriteobj);
}

void CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		string fpsStr = to_string(fps);
		string mspfStr = to_string(mspf);

		string windowText = APPTITLE +
			"    fps: " + fpsStr +
			"   mspf: " + mspfStr+
			"   deltaT: " +to_string(mTimer.DeltaTime()*1000);
		SetWindowText(*hwnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//�رմ���
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
		break;
		//case WM_EXITSIZEMOVE:

		// WM_ACTIVATE���ڻ״̬ 
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			wstat.mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			wstat.mAppPaused = false;
			mTimer.Start();
		}
		break;
		//�ı��С
	case WM_SIZE:
		// ���洰�ڴ�С
		SCREENW = LOWORD(lParam);
		SCREENH = HIWORD(lParam);
		if (d3ddev)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				wstat.mAppPaused = true;
				wstat.mMinimized = true;
				wstat.mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				wstat.mAppPaused = false;
				wstat.mMinimized = false;
				wstat.mMaximized = true;
				//OnResize(hwnd);
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (wstat.mMinimized)
				{
					wstat.mAppPaused = false;
					wstat.mMinimized = false;
					//OnResize(hwnd);
				}

				// Restoring from maximized state?
				else if (wstat.mMaximized)
				{
					wstat.mAppPaused = false;
					wstat.mMaximized = false;
					//OnResize(hwnd);
				}
				else if (wstat.mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					//OnResize(hwnd);
				}
			}
		}
		break;
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		wstat.mAppPaused = true;
		wstat.mResizing = true;
		mTimer.Stop();
		break;
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		wstat.mAppPaused = false;
		wstat.mResizing = false;
		mTimer.Start();
		//OnResize(hwnd);
		break;
		// �������Ϣ���Է�ֹ���ڹ�С
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);//��С�ǽṹ��С
	wc.style = CS_HREDRAW | CS_VREDRAW;//�ı���/�߶�ʱ�ػ�
	wc.lpfnWndProc = (WNDPROC)WinProc;//��win��Ϣֵ��ͬʱ���ûص�����
	wc.cbClsExtra = 0;//�����ڴ�
	wc.cbWndExtra = 0;//ͬ��
	wc.hInstance = hInstance;//ʵ��ѡ��
	wc.hIcon = NULL;//ͼ��
	wc.hIconSm = NULL;//Сͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//�α�
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//������ɫ
	wc.lpszMenuName = NULL;//����˵�
	wc.lpszClassName = "MainWindowClass";//���贰��������������Ϣ����
	RegisterClassEx(&wc);
	HWND window = CreateWindow("MainWindowClass", APPTITLE.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SCREENW, SCREENH, (HWND)NULL, (HMENU)NULL, hInstance, (LPVOID)NULL);//�ޱ��������˵�
	if (window == 0)
		return 0;
	hwnd = &window;
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);
	if (!GameInit(window))
		return 0;
	MSG message;
	mTimer.Reset();//ʹ��ǰ���ü�ʱ��

	while (!gameover)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			//����֡��������50(1000/50=20)�Խ�ʡ����
			//if (mTimer.DeltaTime()<20)
			//	Sleep(20 - mTimer.DeltaTime());
			mTimer.Tick();
			//һ���뷨�������������һ��ֵ�洢delta time����ĳ��ʱ��������������ʱ�䣬���磺delta time ����20ms������time buffer�洢��6ms��
			//���ȡ12ms���Ǵ���12*2ms��Ϊ����ʱ�䣬time bufferΪ6+20-12*2=4������ʵ�ֲ�ͬ������ʱ������ֵͬ������С��ʱ�䷴�����΢С��ֵ��
			//�򸡵������㾫�ȶ�ʧ���µ����
			GameRun(window,mTimer);
			CalculateFrameStats();
		}
		
	}
	GameEnd();
	return message.wParam;
}
