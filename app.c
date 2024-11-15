#include <windows.h>
#include <curl/curl.h>
#include <stdio.h>

#define URL "https://anki.hoonhub.com/new"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void download_file(HWND hwnd) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    const wchar_t *outfilename = L"241115_정보보안기사.apkg";

    curl = curl_easy_init();
    if (curl) {
        fp = _wfopen(outfilename, L"wb");
        if (fp == NULL) {
            MessageBoxW(hwnd, L"파일을 열 수 없습니다.", L"오류", MB_OK | MB_ICONERROR);
            curl_easy_cleanup(curl);
            PostQuitMessage(0);
            return;
        }
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        fclose(fp);

        if (res != CURLE_OK) {
            _wremove(outfilename); // Remove the file if download failed
            MessageBoxW(hwnd, L"다운로드 실패!", L"오류", MB_OK | MB_ICONERROR);
        } else {
            MessageBoxW(hwnd, L"다운로드 완료!", L"정보", MB_OK);
        }

        curl_easy_cleanup(curl);
        PostQuitMessage(0);
    } else {
        MessageBoxW(hwnd, L"CURL 초기화 실패!", L"오류", MB_OK | MB_ICONERROR);
        PostQuitMessage(0);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                download_file(hwnd);
            }
            return 0;
        case WM_CREATE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int buttonWidth = 120;
            int buttonHeight = 30;
            int buttonX = (rect.right - buttonWidth) / 2;
            int buttonY = (rect.bottom - buttonHeight) / 2 + 10;

            CreateWindowW(L"STATIC", L"최신 Deck: 2024-11-15-A", WS_VISIBLE | WS_CHILD | SS_CENTER,
                          0, buttonY - 50, rect.right, 20, hwnd, (HMENU)2,
                          (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            CreateWindowW(L"BUTTON", L"Deck 다운로드", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          buttonX, buttonY, buttonWidth, buttonHeight, hwnd, (HMENU)1, 
                          (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            return 0;
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Anki Helper - v2411.1", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 200,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

// x86_64-w64-mingw32-gcc -o AnkiHelper.exe app.c -I./curl-8.11.0_1-win64-mingw/include -L./curl-8.11.0_1-win64-mingw/lib -lcurl -mwindows -municode
