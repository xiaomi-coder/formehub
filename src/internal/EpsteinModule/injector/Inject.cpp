// ---------------------------------------------------------------------------
// Minimal LoadLibrary injector for "Cs2 Epstein Rage.dll".
//
// Deliberately NOT manual mapping. The payload leans on SEH (Game::Read and
// Game::Write wrap every dereference in __try/__except) and on the CRT
// (printf, std::mutex, std::map). A manually mapped image gets neither its
// exception directory registered nor its CRT initialised, so the __except
// handlers silently stop catching and the first bad read takes the game down —
// which is exactly the failure we are trying to get away from.
//
// LoadLibrary is the more visible technique of the two. That trade is accepted
// here: an injector that reliably runs is worth more right now than a stealthy
// one that crashes.
// ---------------------------------------------------------------------------
#include <windows.h>
#include <tlhelp32.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <string>

namespace
{
    constexpr const char* kGameProcess = "cs2.exe";
    constexpr const char* kPayloadName = "Cs2 Epstein Rage.dll";

    DWORD FindProcessId(const char* imageName)
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return 0;

        PROCESSENTRY32 entry{};
        entry.dwSize = sizeof(entry);

        DWORD pid = 0;
        if (Process32First(snapshot, &entry))
        {
            do
            {
                if (_stricmp(entry.szExeFile, imageName) == 0)
                {
                    pid = entry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return pid;
    }

    // Resolve the payload next to this executable, so the injector works the
    // same whether it is launched from a shell, a shortcut or Explorer.
    std::string PayloadPathNextToExe()
    {
        char self[MAX_PATH]{};
        if (!GetModuleFileNameA(nullptr, self, MAX_PATH))
            return kPayloadName;

        std::string path = self;
        const size_t slash = path.find_last_of("\\/");
        if (slash == std::string::npos)
            return kPayloadName;

        return path.substr(0, slash + 1) + kPayloadName;
    }

    bool AlreadyInjected(DWORD pid, const char* moduleName)
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        if (snapshot == INVALID_HANDLE_VALUE)
            return false;

        MODULEENTRY32 entry{};
        entry.dwSize = sizeof(entry);

        bool found = false;
        if (Module32First(snapshot, &entry))
        {
            do
            {
                if (_stricmp(entry.szModule, moduleName) == 0)
                {
                    found = true;
                    break;
                }
            } while (Module32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return found;
    }
}

int main()
{
    SetConsoleTitleA("Epstein Injector");

    const std::string payload = PayloadPathNextToExe();
    printf("[*] Payload: %s\n", payload.c_str());

    if (GetFileAttributesA(payload.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        printf("[-] DLL topilmadi. Uni injector yonida qo'ying.\n");
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    const DWORD pid = FindProcessId(kGameProcess);
    if (!pid)
    {
        printf("[-] cs2.exe ishlamayapti. Avval o'yinni oching.\n");
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }
    printf("[+] cs2.exe PID: %lu\n", pid);

    if (AlreadyInjected(pid, kPayloadName))
    {
        printf("[!] DLL allaqachon yuklangan. Qayta yuklash uchun o'yinni qayta oching.\n");
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    HANDLE process = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);

    if (!process)
    {
        printf("[-] OpenProcess xato: %lu (administrator sifatida ishga tushiring)\n", GetLastError());
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    const SIZE_T bytes = payload.size() + 1;
    void* remotePath = VirtualAllocEx(process, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remotePath)
    {
        printf("[-] VirtualAllocEx xato: %lu\n", GetLastError());
        CloseHandle(process);
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    if (!WriteProcessMemory(process, remotePath, payload.c_str(), bytes, nullptr))
    {
        printf("[-] WriteProcessMemory xato: %lu\n", GetLastError());
        VirtualFreeEx(process, remotePath, 0, MEM_RELEASE);
        CloseHandle(process);
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    // kernel32 is mapped at the same base in every process of a session, so the
    // local addresses of these exports are valid inside the target too.
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    auto loadLibrary  = reinterpret_cast<std::uint64_t>(GetProcAddress(kernel32, "LoadLibraryA"));
    auto getLastError = reinterpret_cast<std::uint64_t>(GetProcAddress(kernel32, "GetLastError"));

    // A bare CreateRemoteThread(LoadLibraryA) can only report the HMODULE, and
    // only through a 32-bit exit code — which both truncates the handle and
    // throws away the reason for a failure. Run a small stub instead so we get
    // the full 64-bit handle AND the GetLastError that goes with it.
    void* remoteResult = VirtualAllocEx(process, nullptr, 16, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    void* remoteStub   = VirtualAllocEx(process, nullptr, 128, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!remoteResult || !remoteStub)
    {
        printf("[-] VirtualAllocEx (stub) xato: %lu\n", GetLastError());
        CloseHandle(process);
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    const auto pathAddr   = reinterpret_cast<std::uint64_t>(remotePath);
    const auto resultAddr = reinterpret_cast<std::uint64_t>(remoteResult);

    std::uint8_t stub[69]{};
    size_t o = 0;
    auto emit  = [&](std::initializer_list<std::uint8_t> b) { for (auto v : b) stub[o++] = v; };
    auto emit64 = [&](std::uint64_t v) { memcpy(stub + o, &v, 8); o += 8; };

    emit({ 0x48, 0x83, 0xEC, 0x28 });        // sub  rsp, 0x28   (shadow space)
    emit({ 0x48, 0xB9 }); emit64(pathAddr);  // mov  rcx, path
    emit({ 0x48, 0xB8 }); emit64(loadLibrary);
    emit({ 0xFF, 0xD0 });                    // call rax         (LoadLibraryA)
    emit({ 0x48, 0xB9 }); emit64(resultAddr);
    emit({ 0x48, 0x89, 0x01 });              // mov  [rcx], rax  (HMODULE)
    emit({ 0x48, 0xB8 }); emit64(getLastError);
    emit({ 0xFF, 0xD0 });                    // call rax         (GetLastError)
    emit({ 0x48, 0xB9 }); emit64(resultAddr);
    emit({ 0x89, 0x41, 0x08 });              // mov  [rcx+8], eax
    emit({ 0x48, 0x83, 0xC4, 0x28 });        // add  rsp, 0x28
    emit({ 0xC3 });                          // ret

    if (!WriteProcessMemory(process, remoteStub, stub, sizeof(stub), nullptr))
    {
        printf("[-] WriteProcessMemory (stub) xato: %lu\n", GetLastError());
        CloseHandle(process);
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    HANDLE thread = CreateRemoteThread(process, nullptr, 0,
                                       reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteStub),
                                       nullptr, 0, nullptr);
    if (!thread)
    {
        printf("[-] CreateRemoteThread xato: %lu\n", GetLastError());
        VirtualFreeEx(process, remotePath, 0, MEM_RELEASE);
        CloseHandle(process);
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    const DWORD waited = WaitForSingleObject(thread, 15000);
    CloseHandle(thread);

    struct { std::uint64_t hModule; std::uint32_t lastError; std::uint32_t pad; } result{};
    ReadProcessMemory(process, remoteResult, &result, sizeof(result), nullptr);

    VirtualFreeEx(process, remotePath, 0, MEM_RELEASE);
    VirtualFreeEx(process, remoteStub, 0, MEM_RELEASE);
    VirtualFreeEx(process, remoteResult, 0, MEM_RELEASE);
    CloseHandle(process);

    if (waited == WAIT_TIMEOUT)
    {
        printf("[-] Stub 15s ichida tugamadi (loader lock?).\n");
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    if (result.hModule == 0)
    {
        printf("[-] LoadLibraryA NULL qaytardi. GetLastError = %lu\n", result.lastError);
        switch (result.lastError)
        {
        case 2:   printf("    (2 FILE_NOT_FOUND) cs2.exe faylni ko'rmayapti.\n"); break;
        case 5:   printf("    (5 ACCESS_DENIED) fayl ruxsati yoki himoya bloklayapti.\n"); break;
        case 126: printf("    (126 MOD_NOT_FOUND) bog'liq DLL yetishmayapti.\n"); break;
        case 193: printf("    (193 BAD_EXE_FORMAT) x64 emas.\n"); break;
        case 1114:printf("    (1114 DLL_INIT_FAILED) DllMain FALSE qaytardi.\n"); break;
        default:  printf("    Kutilmagan xato — anti-cheat bloklayotgan bo'lishi mumkin.\n"); break;
        }
        printf("\nChiqish uchun Enter...");
        (void)getchar();
        return 1;
    }

    printf("[+] Yuklandi! HMODULE = 0x%llX\n", result.hModule);

    printf("[+] Yuklandi! O'yinda konsol oynasi ochilishi kerak.\n");
    printf("[+] Menyu: INSERT tugmasi.\n");
    printf("\nChiqish uchun Enter...");
    (void)getchar();
    return 0;
}
