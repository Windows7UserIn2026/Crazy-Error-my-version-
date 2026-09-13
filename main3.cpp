/*
 * Marisa Stole the Precious Thing! - Error MessageBoxes + MP3 + bouncing Explorer
 *
 * 1. Starts the MP3 immediately, then shows a WARNING MessageBox.
 * 2. On YES:
 *    - Opens File Explorer
 *    - Makes the Explorer window bounce around the screen
 *    - Runs the timed ERROR MessageBoxes for the melody
 *
 * Put the MP3 in the same folder as the .exe:
 *    "IOSYS - Marisa Stole the Precious Thing!.mp3"
 *
 * Compile (MinGW):
 *    g++ -o MarisaError.exe MarisaErrorMelody.cpp -mwindows
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")

typedef int (WINAPI *PFN_MessageBoxTimeoutW)(
    HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,
    UINT uType, WORD wLanguageId, DWORD dwMilliseconds);

typedef DWORD (WINAPI *PFN_mciSendStringW)(
    LPCWSTR lpszCommand, LPWSTR lpszReturnString,
    UINT cchReturn, HANDLE hwndCallback);

struct Note {
    int dur;
    const wchar_t* text;    // NULL = rest
};

static const Note melody[] = {
    {  88, L"ERROR: F5\n\nMarisa is approaching!" },
    {  88, L"ERROR: G#5\n\nSomething precious is missing..." },
    {  88, L"ERROR: A5\n\nHakurei Shrine inventory fail!" },
    {  88, NULL },
    {  88, L"ERROR: D5\n\nMini-Hakkero overcharging!" },
    {  88, NULL },
    {  88, L"ERROR: B5\n\nMASTER SPARK INCOMING!" },
    { 794, NULL },
    {  88, L"ERROR: F5\n\nShe stole it!" },
    {  88, L"ERROR: A5\n\nReimu is furious!" },
    {  88, L"ERROR: D6\n\nToo fast!" },
    {  88, NULL },
    {  88, L"ERROR: A5\n\nWhere is the precious thing?!" },
    {  88, NULL },
    {  88, L"ERROR: D5\n\nGensokyo security breach!" },
    { 794, NULL },
    {  88, L"ERROR: F5\n\nStill missing!" },
    {  88, L"ERROR: A5\n\nInventory corrupted!" },
    {  88, L"ERROR: B5\n\nIncident level rising!" },
    {  88, NULL },
    {  88, L"ERROR: D#5\n\nUnstable magic!" },
    {  88, NULL },
    {  88, L"ERROR: B4\n\nShrine barrier failing!" },
    {  88, NULL },
    {  88, L"ERROR: C5\n\nReimu loading spell card!" },
    {  88, NULL },
    {  88, L"ERROR: C6\n\nHigh altitude chase!" },
    {  88, NULL },
    {  88, L"ERROR: F5\n\nMarisa is too quick!" },
    {  88, NULL },
    {  88, L"ERROR: B4\n\nNeed backup!" },
    {  88, NULL },
    {  88, L"ERROR: C5\n\nContinue pursuit!" },
    {  88, NULL },
    {  88, L"ERROR: C6\n\nAlmost had her!" },
    {  88, L"ERROR: A5\n\nShe's escaping!" },
    {  88, L"ERROR: F5\n\nPrecious thing: STOLEN" },
    { 265, NULL },

    { 176, L"ERROR: B5\n\nMarisa stole the precious thing!" },
    { 176, L"ERROR: A5\n\nYoink! It's gone!" },
    { 176, L"ERROR: E5\n\nReimu: MARISAAAA!" },
    { 176, L"ERROR: A5\n\nWhere did she go?!" },
    { 176, L"ERROR: G5\n\nMini-Hakkero charging..." },
    { 176, L"ERROR: D5\n\nSpark detected!" },
    { 176, L"ERROR: B5\n\nMASTER SPARK!!" },
    { 176, L"ERROR: A5\n\nToo bright!" },
    { 176, L"ERROR: E5\n\nShrine on fire (again)" },
    { 176, L"ERROR: A5\n\nInventory still empty" },
    { 176, L"ERROR: G5\n\nNeed more ofuda" },
    { 176, L"ERROR: D5\n\nChasing through the forest" },
    { 176, L"ERROR: E5\n\nShe's laughing" },
    { 176, L"ERROR: E5\n\nStill laughing" },
    { 176, L"ERROR: A5\n\nPrecious thing not recovered" },
    { 176, NULL },

    { 176, L"ERROR: B5\n\nRound 2: Marisa steals again!" },
    { 176, L"ERROR: A5\n\nSame move, different day" },
    { 176, L"ERROR: E5\n\nReimu rage increasing" },
    { 176, L"ERROR: A5\n\nSpell card ready" },
    { 176, L"ERROR: G5\n\nFantasy Seal loading..." },
    { 176, L"ERROR: D5\n\nAlmost hit her!" },
    { 176, L"ERROR: B5\n\nShe dodged with style" },
    { 176, L"ERROR: A5\n\nTypical Marisa" },
    { 176, L"ERROR: E5\n\nIt's not like I stole it!" },
    { 176, L"ERROR: A5\n\nYes you did" },
    { 176, L"ERROR: G5\n\nEvidence: missing item" },
    { 176, L"ERROR: D5\n\nGensokyo police alerted" },
    { 176, L"ERROR: E5\n\n(there is no police)" },
    { 176, L"ERROR: E5\n\nJust Reimu" },
    { 176, L"ERROR: A5\n\nAnd she is angry" },
    { 176, NULL },

    { 176, L"ERROR: B5\n\nFinal chorus!" },
    { 176, L"ERROR: A5\n\nStill no precious thing" },
    { 176, L"ERROR: E5\n\nMarisa: Hehe~" },
    { 176, L"ERROR: A5\n\nReimu: GIVE IT BACK!" },
    { 176, L"ERROR: G5\n\nChase continues" },
    { 176, L"ERROR: D5\n\nThrough the misty lake" },
    { 176, L"ERROR: B5\n\nMaster Spark again?!" },
    { 176, L"ERROR: A5\n\nDodged!" },
    { 176, L"ERROR: E5\n\nThis is taking forever" },
    { 176, L"ERROR: A5\n\nClassic Touhou incident" },
    { 176, L"ERROR: G5\n\nStatus: unresolved" },
    { 176, L"ERROR: D5\n\nWill repeat tomorrow" },
    { 176, L"ERROR: E5\n\nBecause it's fun" },
    { 176, L"ERROR: E5\n\nAnd the music is catchy" },
    { 176, L"ERROR: A5\n\nPrecious thing permanently yoinked" },
    { 176, NULL },

    { 176, L"ERROR: A5\n\nA5 hold..." },
    { 176, NULL },
    { 176, L"ERROR: A5\n\nStill holding..." },
    { 400, L"FATAL ERROR\n\nMarisa successfully stole\nthe precious thing.\n\nCase closed (for now)." },
};

static const int NUM_NOTES = sizeof(melody) / sizeof(melody[0]);

static PFN_mciSendStringW pMciSendStringW = NULL;
static volatile BOOL g_Bounce = FALSE;
static HWND g_ExplorerWnd = NULL;

bool StartMp3(const wchar_t* path) {
    if (!pMciSendStringW) return false;
    wchar_t cmd[1024];
    pMciSendStringW(L"close marisa", NULL, 0, NULL);
    wsprintfW(cmd, L"open \"%s\" type mpegvideo alias marisa", path);
    if (pMciSendStringW(cmd, NULL, 0, NULL) != 0) {
        wsprintfW(cmd, L"open \"%s\" alias marisa", path);
        if (pMciSendStringW(cmd, NULL, 0, NULL) != 0)
            return false;
    }
    return pMciSendStringW(L"play marisa", NULL, 0, NULL) == 0;
}

void StopMp3() {
    if (!pMciSendStringW) return;
    pMciSendStringW(L"stop marisa", NULL, 0, NULL);
    pMciSendStringW(L"close marisa", NULL, 0, NULL);
}

BOOL CALLBACK FindExplorerProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[64];
    if (GetClassNameW(hwnd, className, 64) &&
        (wcscmp(className, L"CabinetWClass") == 0 ||
         wcscmp(className, L"ExploreWClass") == 0)) {
        if (IsWindowVisible(hwnd)) {
            g_ExplorerWnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

DWORD WINAPI BounceThread(LPVOID) {
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    // DVD-logo style: constant velocity, bounce off edges
    int x = screenW / 4;
    int y = screenH / 4;
    int dx = 6;   // horizontal speed (pixels per frame)
    int dy = 5;   // vertical speed
    int w = 800;
    int h = 600;

    // Try to get real size of the Explorer window once
    if (g_ExplorerWnd && IsWindow(g_ExplorerWnd)) {
        RECT rc;
        GetWindowRect(g_ExplorerWnd, &rc);
        w = rc.right - rc.left;
        h = rc.bottom - rc.top;
        if (w < 200) w = 800;
        if (h < 150) h = 600;
        x = rc.left;
        y = rc.top;
    }

    while (g_Bounce) {
        if (g_ExplorerWnd && IsWindow(g_ExplorerWnd)) {
            // Move
            x += dx;
            y += dy;

            // Bounce off left/right edges
            if (x <= 0) {
                x = 0;
                dx = -dx;
            } else if (x + w >= screenW) {
                x = screenW - w;
                dx = -dx;
            }

            // Bounce off top/bottom edges
            if (y <= 0) {
                y = 0;
                dy = -dy;
            } else if (y + h >= screenH) {
                y = screenH - h;
                dy = -dy;
            }

            SetWindowPos(g_ExplorerWnd, HWND_TOP, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
        Sleep(16); // ~60 FPS for smooth DVD bounce
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");
    PFN_MessageBoxTimeoutW pMessageBoxTimeoutW = NULL;
    if (hUser32)
        pMessageBoxTimeoutW = (PFN_MessageBoxTimeoutW)GetProcAddress(hUser32, "MessageBoxTimeoutW");

    HMODULE hWinmm = LoadLibraryW(L"winmm.dll");
    if (hWinmm)
        pMciSendStringW = (PFN_mciSendStringW)GetProcAddress(hWinmm, "mciSendStringW");

    // Start MP3 immediately (plays while the warning MessageBox is shown)
    wchar_t mp3Path[MAX_PATH] = L"IOSYS - Marisa Stole the Precious Thing!.mp3";
    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (lastSlash) {
            *(lastSlash + 1) = 0;
            wcscat(exePath, L"IOSYS - Marisa Stole the Precious Thing!.mp3");
            if (GetFileAttributesW(exePath) != INVALID_FILE_ATTRIBUTES)
                wcscpy(mp3Path, exePath);
        }
    }
    bool playing = StartMp3(mp3Path);

    // ========== FIRST MESSAGEBOX (WARNING) ==========
    int result = MessageBoxW(NULL,
        L"Firow didn't open because of an error.\n"
        L"Do you want to restart Firow?",
        L"Firow",
        MB_YESNO | MB_ICONWARNING);

    // FIX: Check for IDYES instead of IDOK
    if (result != IDYES) {
        StopMp3();
        if (hWinmm) FreeLibrary(hWinmm);
        if (hUser32) FreeLibrary(hUser32);
        return 0;
    }

    // --- User clicked YES on the warning ---

    // 1. Open File Explorer
    ShellExecuteW(NULL, L"open", L"explorer.exe", L".", NULL, SW_SHOWNORMAL);
    Sleep(800);

    // 2. Find Explorer window and start bouncing
    EnumWindows(FindExplorerProc, 0);
    if (!g_ExplorerWnd) {
        Sleep(600);
        EnumWindows(FindExplorerProc, 0);
    }

    g_Bounce = TRUE;
    HANDLE hBounce = CreateThread(NULL, 0, BounceThread, NULL, 0, NULL);

    // 3. ERROR MessageBox sequence starts now
    for (int i = 0; i < NUM_NOTES; ++i) {
        const Note& n = melody[i];

        if (n.text == NULL) {
            Sleep((DWORD)n.dur);
            continue;
        }

        UINT type = MB_OK | MB_TOPMOST | MB_ICONERROR;

        if (pMessageBoxTimeoutW) {
            pMessageBoxTimeoutW(
                NULL, n.text,
                L"ERROR - Marisa Stole the Precious Thing!",
                type, 0, (DWORD)n.dur);
        } else {
            MessageBoxW(NULL, n.text,
                L"ERROR - Marisa Stole the Precious Thing!", type);
        }
    }

    g_Bounce = FALSE;
    if (hBounce) {
        WaitForSingleObject(hBounce, 1000);
        CloseHandle(hBounce);
    }
    StopMp3();

    MessageBoxW(NULL,
        L"SEQUENCE COMPLETE\n\n"
        L"All error notes have been displayed.\n"
        L"The precious thing remains stolen.\n"
        L"Explorer can stop bouncing now.",
        L"Incident Log Closed",
        MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

    if (hWinmm) FreeLibrary(hWinmm);
    if (hUser32) FreeLibrary(hUser32);
    return 0;
}
