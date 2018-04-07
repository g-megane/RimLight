#include <Windows.h>
#include <sstream>
#include <locale>
#include "Window.h"
#include "DirectX11.h"
#include "Model.h"
#include "Matrix.h"
#include "MyMath.h"
#include "Time.h"

using namespace Lib;

const float FPS   = 60.0f;   // ���s������fps
const float SPEED = 1800.0f; // ���f���̈ړ����x

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // �E�B���h�E�̍쐬
    auto w = std::make_shared<Lib::Window>(L"3DCGLib", 1026, 768);

    // DirectX�̃C���X�^���X�̎擾
    auto& directX = Lib::DirectX11::getInstance();
    directX.initDevice(w);

    // ViewMatrix�̏�����
    Vector3 eye = Vector3(0.0f, 2.0f, -5.0f); // �J�����̍��W
    Vector3 at  = Vector3(0.0f, 1.0f,  0.0f); // �����Ώ�
    Vector3 up  = Vector3::UP;                // ���݂̃��[���h���W�̏����
    auto view   = Matrix::LookAtLH(eye, at, up);
    directX.setViewMatrix(view);

    // ProjectionMatrix�̏�����
    LONG windowWidth  = w->getWindowRect().right  - w->getWindowRect().left;
    LONG windowHeight = w->getWindowRect().bottom - w->getWindowRect().top;
    auto projection   = Matrix::perspectiveFovLH(MyMath::PIDIV2, windowWidth / static_cast<float>(windowHeight), 0.01f, 100.0f);
    directX.setProjectionMatrix(projection);
    
    // ���f���̍쐬
    Model model = Model(36);
    Matrix world;
    world = Matrix::Identify;

    float rotX = 0.0f;
    float rotY = 0.0f;

    // ����
    MessageBox(w->getHWND(), L"�uW�v�uA�v�uS�v�uD�v�Ń��f���̉�]", L"�������", MB_OK | MB_ICONINFORMATION);

    // �X�V����
    Time time = Time();
    int fps = 0;
    float countTime = 0.0f;
    float deltaTime = 0.0f;
    std::ostringstream oss;
    WCHAR wcstr[50];
    size_t size = 0;
    setlocale(LC_ALL, "japanese"); // ���mbstowcs_s�ׂ̈̏���
    
    while (w->Update().message != WM_QUIT) {
        directX.begineFrame();

        // FPS�̌Œ�
        if (!time.timeOver(1000.0f / FPS)) {
            continue;
        }

        deltaTime = time.getDeltaTime();
        countTime += deltaTime;

        // 1�b�ɂP��s������
        if (countTime > 1000.0f) {
            // fps���f�o�b�K�ɏo��
            oss.str("");
            oss << "fps: " << fps << std::endl;
            mbstowcs_s(&size, wcstr, 20, oss.str().c_str(), _TRUNCATE);
            OutputDebugString(wcstr);
            // �ϐ��̃��Z�b�g
            fps = 0;
            countTime = 0.0f;
        }
        time.reset();
        ++fps;

        // ��]
        if (w->getKeyDown('W')) {
            rotX += MyMath::PIDIV2 / SPEED * deltaTime;
        }
        if (w->getKeyDown('S')) {
            rotX -= MyMath::PIDIV2 / SPEED * deltaTime;
        }
        if (w->getKeyDown('A')) {
            rotY += MyMath::PIDIV2 / SPEED * deltaTime;
        }
        if (w->getKeyDown('D')) {
            rotY -= MyMath::PIDIV2 / SPEED * deltaTime;
        }

        // �I�[�o�[�t���[�̐���
        rotX = MyMath::rollup<float>(rotX, MyMath::PI2);
        rotY = MyMath::rollup<float>(rotY, MyMath::PI2);

        // ���f���̐���
        auto mtrX = Matrix::rotateX(rotX);
        auto mtrY = Matrix::rotateY(rotY);
        auto mtt  = Matrix::translate(Vector3(0.0f, -1.0f, 0.0f));
        world = mtrX * mtrY * mtt;
        model.setWorldMatrix(world);

        // �`��
        model.render(Lib::Color(Lib::Color::BLUE));

        directX.endFrame();
    }

    return 0;
}