// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBufferTri;                // the pointer to the vertex buffer
ID3D11Buffer *pVBufferRec;                // the pointer to the vertex buffer
ID3D11Buffer *pCBuffer;                // the pointer to the constant buffer
ID3D11RasterizerState * g_pRasterState;

// various buffer structs
struct VERTEX{FLOAT X, Y, Z; D3DXCOLOR Color;};
struct PERFRAME{D3DXCOLOR Color; FLOAT X, Y, Z;};
struct FACE{FLOAT X, Y, Z;};

// function prototypes
void InitD3D(HWND hWnd);    // sets up and initializes Direct3D
void RenderFrame(void);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
void InitGraphics(void);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL,
                          L"WindowClass",
                          L"Our First Direct3D Program",
                          WS_OVERLAPPEDWINDOW,
                          300,
                          300,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D
    InitD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }

        RenderFrame();
    }

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                   // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
    scd.BufferDesc.Width = SCREEN_WIDTH;                   // set the back buffer width
    scd.BufferDesc.Height = SCREEN_HEIGHT;                 // set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
    scd.OutputWindow = hWnd;                               // the window to be used
    scd.SampleDesc.Count = 4;                              // how many multisamples
    scd.Windowed = TRUE;                                   // windowed/full-screen mode
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &dev,
                                  NULL,
                                  &devcon);


    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);


    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    devcon->RSSetViewports(1, &viewport);

    InitPipeline();
    InitGraphics();
}


// this is the function used to render a single frame
void RenderFrame(void)
{
	// clear the back buffer to black
    devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));


	/*
	 *
	 * Render the triangle
	 *
	 */
    D3DXMATRIX matRotateTriY, matRotateTriZ, matTranslateTri, matViewTri, matProjectionTri, matFinalTri;

    static float TimeTri = 0.0f; TimeTri += 0.001f;

    // create a rotation matrix
    D3DXMatrixRotationY(&matRotateTriY, TimeTri + 3.14f);
	D3DXMatrixRotationZ(&matRotateTriZ, TimeTri + 3.14f);

	// create a translation matrix
	D3DXMatrixTranslation(&matTranslateTri, 1.5f, 0.5f, 0.0f);

    // create a view matrix
    D3DXMatrixLookAtLH(&matViewTri,
                       &D3DXVECTOR3(0.0f, 0.0f, 4.0f),    // the camera position
                       &D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
                       &D3DXVECTOR3(0.0f, 1.0f, 0.0f));   // the up direction

    // create a projection matrix
    D3DXMatrixPerspectiveFovLH(&matProjectionTri,
                               (FLOAT)D3DXToRadian(45),                    // field of view
                               (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
                               1.0f,                                       // near view-plane
                               100.0f);                                    // far view-plane

    // create the final transform
    matFinalTri = matRotateTriY * matRotateTriZ * matTranslateTri * matViewTri * matProjectionTri;

    // set the new values for the constant buffer
    devcon->UpdateSubresource(pCBuffer, 0, 0, &matFinalTri, 0, 0);



        // select which vertex buffer to display
        UINT strideTri = sizeof(VERTEX);
        UINT offsetTri = 0;
        devcon->IASetVertexBuffers(0, 1, &pVBufferTri, &strideTri, &offsetTri);

        // select which primtive type we are using
        devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw the vertex buffer to the back buffer
        devcon->Draw(3, 0);



	/*
	 *
	 * Render the Rectangle
	 *
	 */
	D3DXMATRIX matRotateRec, matTranslateRec, matViewRec, matProjectionRec, matFinalRec;

    static float TimeRec = 0.0f; TimeRec += 0.001f;

    // create a rotation matrix
    D3DXMatrixRotationY(&matRotateRec, TimeRec + 3.14f);

	// create a translation matrix
	D3DXMatrixTranslation(&matTranslateRec, -1.5f, -0.5f, 0.0f);

    // create a view matrix
    D3DXMatrixLookAtLH(&matViewRec,
                       &D3DXVECTOR3(0.0f, 0.0f, 4.0f),    // the camera position
                       &D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
                       &D3DXVECTOR3(0.0f, 1.0f, 0.0f));   // the up direction

    // create a projection matrix
    D3DXMatrixPerspectiveFovLH(&matProjectionRec,
                               (FLOAT)D3DXToRadian(45),                    // field of view
                               (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
                               1.0f,                                       // near view-plane
                               100.0f);                                    // far view-plane

    // create the final transform
    matFinalRec = matRotateRec * matTranslateRec * matViewRec * matProjectionRec;

    // set the new values for the constant buffer
    devcon->UpdateSubresource(pCBuffer, 0, 0, &matFinalRec, 0, 0);


        // select which vertex buffer to display
        UINT strideRec = sizeof(VERTEX);
        UINT offsetRec = 0;
        devcon->IASetVertexBuffers(0, 1, &pVBufferRec, &strideRec, &offsetRec);

        // select which primtive type we are using
        devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw the vertex buffer to the back buffer
        devcon->Draw(6, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}


// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    pLayout->Release();
    pVS->Release();
    pPS->Release();
    pVBufferTri->Release();
	pVBufferRec->Release();
    pCBuffer->Release();
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}


// this is the function that creates the shape to render
void InitGraphics()
{
    /*
	 *
	 * create a triangle
	 *
	 */
    VERTEX triVertices[] =
    {
        {0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {-0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };

	// create the vertex buffer
    D3D11_BUFFER_DESC bdTri;
    ZeroMemory(&bdTri, sizeof(bdTri));

    bdTri.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bdTri.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
    bdTri.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bdTri.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    dev->CreateBuffer(&bdTri, NULL, &pVBufferTri);       // create the buffer

	// copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE msTri;
    devcon->Map(pVBufferTri, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msTri);    // map the buffer
    memcpy(msTri.pData, triVertices, sizeof(triVertices));                 // copy the data
    devcon->Unmap(pVBufferTri, NULL);                                      // unmap the buffer


    /*
	 *
	 * create a rectangle
	 *
	 */
    VERTEX recVertices[] =
    {
        {-0.5f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {0.5f, 0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {-0.5f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)},
		{0.5f, 0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {0.5f, -0.5, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {-0.5f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };

    // create the vertex buffer
    D3D11_BUFFER_DESC bdRec;
    ZeroMemory(&bdRec, sizeof(bdRec));

    bdRec.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bdRec.ByteWidth = sizeof(VERTEX) * 6;             // size is the VERTEX struct * 3
    bdRec.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bdRec.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    dev->CreateBuffer(&bdRec, NULL, &pVBufferRec);       // create the buffer

    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE msRec;
    devcon->Map(pVBufferRec, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msRec); // map the buffer
    memcpy(msRec.pData, triVertices, sizeof(triVertices));                 // copy the data
	memcpy(msRec.pData, recVertices, sizeof(recVertices));                 // copy the data
    devcon->Unmap(pVBufferRec, NULL);                                      // unmap the buffer
}


void drawCircle(float x, float y, float z, float radius, float normal[])
{

}



void drawSphere()
{
    
}


// this function loads and prepares the shaders
void InitPipeline()
{
	D3D11_RASTERIZER_DESC rasterizerState;

	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerState.FillMode = D3D11_FILL_SOLID;
    rasterizerState.CullMode = D3D11_CULL_NONE;

	dev->CreateRasterizerState(&rasterizerState, &g_pRasterState);
	devcon->RSSetState(g_pRasterState);
	

    // load and compile the two shaders
    ID3D10Blob *VS, *PS;
    D3DX11CompileFromFile(L"shaders.hlsl", 0, 0, "VShader", "vs_5_0", 0, 0, 0, &VS, 0, 0);
    D3DX11CompileFromFile(L"shaders.hlsl", 0, 0, "PShader", "ps_5_0", 0, 0, 0, &PS, 0, 0);

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = 64;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    dev->CreateBuffer(&bd, NULL, &pCBuffer);
    devcon->VSSetConstantBuffers(0, 1, &pCBuffer);
}
