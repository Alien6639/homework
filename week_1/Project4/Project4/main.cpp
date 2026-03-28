#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

HWND g_hWnd = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11RasterizerState* g_pRasterState = nullptr;

ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;

ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11InputLayout* g_pInputLayout = nullptr;

struct Vertex
{
    float x, y, z;
    float r, g, b, a;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool InitWindow(HINSTANCE hInstance, int width, int height)
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX11Hexagon";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);

    g_hWnd = CreateWindowEx(0, L"DX11Hexagon", L"DX11 ������", WS_OVERLAPPEDWINDOW, 100, 100, width, height, NULL, NULL, hInstance, NULL);
    ShowWindow(g_hWnd, SW_SHOW); UpdateWindow(g_hWnd);
    return true;
}

bool InitD3D(int width, int height)
{
    DXGI_SWAP_CHAIN_DESC scDesc = { 0 };
    scDesc.BufferCount = 1;
    scDesc.BufferDesc.Width = width;
    scDesc.BufferDesc.Height = height;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = g_hWnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &scDesc, &g_pSwapChain, &g_pd3dDevice, NULL, &g_pImmediateContext);

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    pBackBuffer->Release();

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

    D3D11_VIEWPORT vp = { 0, 0, (float)width, (float)height, 0.0f, 1.0f };
    g_pImmediateContext->RSSetViewports(1, &vp);

    // ====================== �޸��ؼ����رձ����޳� ======================
    D3D11_RASTERIZER_DESC rasterDesc = { 0 };
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;  // �ر��޳�������
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    g_pd3dDevice->CreateRasterizerState(&rasterDesc, &g_pRasterState);
    g_pImmediateContext->RSSetState(g_pRasterState);

    return true;
}

bool InitShaders()
{
    const char* shaderCode = R"(
        struct VSInput { float3 pos : POSITION; float4 color : COLOR; };
        struct PSInput { float4 pos : SV_POSITION; float4 color : COLOR; };
        PSInput VS(VSInput input) { PSInput output; output.pos = float4(input.pos, 1.0f); output.color = input.color; return output; }
        float4 PS(PSInput input) : SV_Target { return input.color; }
    )";

    ID3DBlob* vsBlob, * psBlob;
    D3DCompile(shaderCode, strlen(shaderCode), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &vsBlob, NULL);
    D3DCompile(shaderCode, strlen(shaderCode), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &psBlob, NULL);

    g_pd3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &g_pVertexShader);
    g_pd3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &g_pPixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    g_pd3dDevice->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_pInputLayout);

    g_pImmediateContext->IASetInputLayout(g_pInputLayout);
    g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

    vsBlob->Release(); psBlob->Release();
    return true;
}

bool InitGeometry()
{
    Vertex vertices[] = {
        { 0.0f,   0.0f,   0.0f, 1,1,1,1 },
        { 0.25f,  0.0f,   0.0f, 1,0,0,1 },
        { 0.125f, 0.216f, 0.0f, 0,1,0,1 },
        {-0.125f, 0.216f, 0.0f, 0,0,1,1 },
        {-0.25f,  0.0f,   0.0f, 1,1,0,1 },
        {-0.125f,-0.216f, 0.0f, 1,0,1,1 },
        { 0.125f,-0.216f, 0.0f, 0,1,1,1 }
    };

    WORD indices[] = { 0,1,2, 0,2,3, 0,3,4, 0,4,5, 0,5,6, 0,6,1 };

    D3D11_BUFFER_DESC vbDesc = { 0 };
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vbData = { vertices };
    g_pd3dDevice->CreateBuffer(&vbDesc, &vbData, &g_pVertexBuffer);

    D3D11_BUFFER_DESC ibDesc = { 0 };
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibData = { indices };
    g_pd3dDevice->CreateBuffer(&ibDesc, &ibData, &g_pIndexBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return true;
}

void Render()
{
    float clearColor[] = { 0.1f, 0.2f, 0.4f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
    g_pImmediateContext->DrawIndexed(18, 0, 0);
    g_pSwapChain->Present(1, 0);
}

void Cleanup()
{
    if (g_pRasterState) g_pRasterState->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pInputLayout) g_pInputLayout->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdShow)
{
    InitWindow(hInstance, 800, 600);
    InitD3D(800, 600);
    InitShaders();
    InitGeometry();

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Render();
        }
    }
    Cleanup();
    return 0;
}