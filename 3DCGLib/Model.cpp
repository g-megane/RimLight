#include <d3dcompiler.h>
#include <vector>
#include "Model.h"
#include "MyMath.h"

namespace Lib
{
    // コンストラクタ
    Model::Model()
    {
        world = Matrix::Identify;
        vertexCount = 0;
        init();
    }

    Model::Model(const int SEGMENT)
    {
        world = Matrix::Identify;
        vertexCount = 0;
        initSqhere(SEGMENT);
    }

    // デストラクタ
    Model::~Model()
    {
    }

    // モデルの描画
    void Model::render(Color &color)
    {
        auto &directX = DirectX11::getInstance();

        float eye[4] = { -directX.getViewMatrix().m41, -directX.getViewMatrix().m42, -directX.getViewMatrix().m43, 1.0f };

        ConstantBuffer cb;
        cb.world      = Matrix::transpose(world);
        cb.view       = Matrix::transpose(directX.getViewMatrix());
        cb.projection = Matrix::transpose(directX.getProjectionMatrix());
        directX.getDeviceContext()->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

        ConstantBuffer2 cb2;
        memcpy(cb2.eyePos, eye, sizeof(eye));
        directX.getDeviceContext()->UpdateSubresource(constantBuffer2.Get(), 0, nullptr, &cb2, 0, 0);

        directX.getDeviceContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
        directX.getDeviceContext()->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        directX.getDeviceContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
        directX.getDeviceContext()->PSSetConstantBuffers(0, 1, constantBuffer2.GetAddressOf());
        directX.getDeviceContext()->DrawIndexed(vertexCount, 0, 0);
    }

    // ワールド行列を設定
    void Model::setWorldMatrix(Matrix & _world)
    {
        world = _world;
    }

    // ワールド行列を取得
    Matrix Model::getWorldMatrix() const
    {
        return world;
    }

    // 初期化
    HRESULT Model::init()
    {
        auto &directX = DirectX11::getInstance();
        auto hr = S_OK;

        // VertexShaderの読み込み
        auto VSBlob = shaderCompile(L"VertexShader.hlsl", "VS", "vs_4_0");
        if (VSBlob == nullptr) {
            MessageBox(nullptr, L"shaderCompile()の失敗(VS)", L"Error", MB_OK);
            return hr;
        }

        // VertexShaderの作成
        hr = directX.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"VSコンパイル失敗", L"Error", MB_OK);
            return hr;
        }

        // InputLayouの定義
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            {   "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // InputLayoutの作成
        hr = directX.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), vertexLayout.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateInputLayoutの失敗 : ", L"Error", MB_OK);
            return hr;
        }

        // InputLayoutをセット
        directX.getDeviceContext()->IASetInputLayout(vertexLayout.Get());

        // PixelShaderの読み込み
        auto PSBlob = shaderCompile(L"PixelShader.hlsl", "PS", "ps_4_0");
        if (PSBlob == nullptr) {
            MessageBox(nullptr, L"shaderCompile()の失敗(PS)", L"Error", MB_OK);
            return hr;
        }

        // VertexShaderの作成
        hr = directX.getDevice()->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createPixelShader()の失敗", L"Error", MB_OK);
            return hr;
        }

        // VertexBufferの定義
        SimpleVertex vertices[] =
        {
            { { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f} },
            { {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f} },
            { {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f} },
            { { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f} },

            { { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f} },
            { {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f} },
            { {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f} },
            { { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f} },

            { { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f} },
            { { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f} },
            { { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f} },
            { { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f} },

            { {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f} },
            { {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f} },
            { {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f} },
            { {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f} },

            { { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },
            { {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },
            { {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },
            { { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f} },

            { { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
            { {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
            { {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
            { { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f} },
        };

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 24;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = vertices;
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, vertexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // VertexBufferをセット
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        directX.getDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

        // 頂点バッファの作成
        WORD indices[] =
        {
             3,  1,  0,
             2,  1,  3,

             6,  4,  5,
             7,  4,  6,
             
            11,  9,  8,
            10,  9, 11,

            14, 12, 13,
            15, 12, 14,

            19, 17, 16,
            18, 17, 19,

            22, 20, 21,
            23, 20, 22
        };
        vertexCount = 36;

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * vertexCount; // 36頂点、12三角形
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        initData.pSysMem = indices;
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, indexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // 頂点バッファをセット
        directX.getDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        // PrimitiveTopologyをセット
        directX.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // ConstantBufferの作成
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(ConstantBuffer);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = directX.getDevice()->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(ConstantBuffer2);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = directX.getDevice()->CreateBuffer(&bd, nullptr, constantBuffer2.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        return S_OK;
    }

     // 初期化（球体）
    HRESULT Model::initSqhere(const int SEGMENT)
    {
        auto &directX = DirectX11::getInstance();
        auto hr = S_OK;

        // VertexShaderの読み込み
        auto VSBlob = shaderCompile(L"VertexShader.hlsl", "VS", "vs_4_0");
        if (VSBlob == nullptr) {
            MessageBox(nullptr, L"shaderCompile()の失敗(VS)", L"Error", MB_OK);
            return hr;
        }

        // VertexShaderの作成
        hr = directX.getDevice()->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"VSコンパイル失敗", L"Error", MB_OK);
            return hr;
        }

        // InputLayouの定義
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            {   "NORMAL", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // InputLayoutの作成
        hr = directX.getDevice()->CreateInputLayout(layout, numElements, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), vertexLayout.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"CreateInputLayoutの失敗(VS)", L"Error", MB_OK);
            return hr;
        }

        // InputLayoutをセット
        directX.getDeviceContext()->IASetInputLayout(vertexLayout.Get());
        
        // PixelShaderの読み込み
        auto PSBlobLight = shaderCompile(L"PixelShader.hlsl", "PS", "ps_4_0");
        if (PSBlobLight == nullptr) {
            MessageBox(nullptr, L"shaderCompile()の失敗(VS)", L"Error", MB_OK);
            return hr;
        }

        // PixelShaderの作成
        hr = directX.getDevice()->CreatePixelShader(PSBlobLight->GetBufferPointer(), PSBlobLight->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createPixelShader()の失敗", L"Error", MB_OK);
            return hr;
        }

        // VertexBufferの定義
        const int COUNT = (SEGMENT + 1) * (SEGMENT / 2 + 1);
        std::vector<SimpleVertex> vertices(COUNT);
        
        for (int i = 0; i <= (SEGMENT / 2); ++i) {
            float irad = MyMath::PI * 2.0f / static_cast<float>(SEGMENT) * static_cast<float>(i);
            float y = static_cast<float>(std::cosf(irad));
            float r = static_cast<float>(std::sinf(irad));
            for (int j = 0; j <= SEGMENT; ++j) {
                float jrad = MyMath::PI * 2.0f / static_cast<float>(SEGMENT) * static_cast<float>(j);
                float x = r * static_cast<float>(std::cosf(jrad));
                float z = r * static_cast<float>(std::sinf(jrad));
                int   inx = i * (SEGMENT + 1) + j;                
                vertices[inx].pos[0] = x;
                vertices[inx].pos[1] = y;
                vertices[inx].pos[2] = z;
                vertices[inx].normal[0] = x;
                vertices[inx].normal[1] = y;
                vertices[inx].normal[2] = z;                
            }
        }

        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * COUNT;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = vertices.data();
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, vertexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // VertexBufferをセット
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        directX.getDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

        // 頂点バッファの作成
        const int COUNT2 = SEGMENT * 3 + SEGMENT * (SEGMENT / 2 - 1) * 6 + SEGMENT * 3;
        std::vector<WORD> indices(COUNT2);

        int count = 0;
        int i = 0;
        for (int j = 0; j < SEGMENT; ++j) {
            indices[count] = i * (SEGMENT + 1) + j;
            indices[count + 1] = (i + 1) * (SEGMENT + 1) + j + 1;
            indices[count + 2] = (i + 1) * (SEGMENT + 1) + j;
            count += 3;
        }
        for (i = 1; i < SEGMENT / 2; ++i) {
            for (int j = 0; j < SEGMENT; ++j) {
                indices[count] = i * (SEGMENT + 1) + j;
                indices[count + 1] = i * (SEGMENT + 1) + j + 1;
                indices[count + 2] = (i + 1) * (SEGMENT + 1) + j;
                count += 3;
                indices[count] = i * (SEGMENT + 1) + j + 1;
                indices[count + 1] = (i + 1) * (SEGMENT + 1) + j + 1;
                indices[count + 2] = (i + 1) * (SEGMENT + 1) + j;
                count += 3;
            }
        }
        i = SEGMENT / 2;
        for (int j = 0; j < SEGMENT; ++j) {
            indices[count] = i * (SEGMENT + 1) + j;
            indices[count + 1] = (i + 1) * (SEGMENT + 1) + j + 1;
            indices[count + 2] = (i + 1) * (SEGMENT + 1) + j;
            count += 3;
        }

        vertexCount = count;

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * count; // 36頂点、12三角形
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        initData.pSysMem = indices.data();
        hr = directX.getDevice()->CreateBuffer(&bd, &initData, indexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        // 頂点バッファをセット
        directX.getDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        // PrimitiveTopologyをセット
        directX.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // ConstantBufferの作成
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(ConstantBuffer);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = directX.getDevice()->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(ConstantBuffer2);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = directX.getDevice()->CreateBuffer(&bd, nullptr, constantBuffer2.GetAddressOf());
        if (FAILED(hr)) {
            MessageBox(nullptr, L"createBuffer()の失敗", L"Error", MB_OK);
            return hr;
        }

        return S_OK;
    }

    // シェーダーの読み込み
    Microsoft::WRL::ComPtr<ID3DBlob> Model::shaderCompile(WCHAR * filename, LPCSTR entryPoint, LPCSTR shaderModel)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> blobOut = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

        DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        shaderFlags |= D3DCOMPILE_DEBUG;
#endif

        auto hr = D3DCompileFromFile(
            filename,
            nullptr,
            nullptr,
            entryPoint,
            shaderModel,
            shaderFlags,
            0,
            blobOut.GetAddressOf(),
            errorBlob.GetAddressOf()
        );

        if (FAILED(hr)) {
            if (errorBlob != nullptr) {
                MessageBox(nullptr, static_cast<LPWSTR>(errorBlob->GetBufferPointer()), nullptr, MB_OK);
            }
            if (errorBlob) {
                MessageBox(nullptr, static_cast<LPWSTR>(errorBlob->GetBufferPointer()), nullptr, MB_OK);
                errorBlob.Get()->Release();
            }
        }
        if (errorBlob) {
            MessageBox(nullptr, static_cast<LPWSTR>(errorBlob->GetBufferPointer()), nullptr, MB_OK);
            errorBlob.Get()->Release();
        }

        return Microsoft::WRL::ComPtr<ID3DBlob>(blobOut);
    }
}