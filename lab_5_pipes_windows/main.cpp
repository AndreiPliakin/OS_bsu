#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

int main() {
    HANDLE pipe_MA_read, pipe_MA_write;
    HANDLE pipe_AP_read, pipe_AP_write; // обьявили дескрипторы
    HANDLE pipe_PS_read, pipe_PS_write;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;  //дочерние процессы наследуют дескрипторы
    sa.lpSecurityDescriptor = nullptr;

    if (!CreatePipe(&pipe_MA_read, &pipe_MA_write, &sa, 0) ||
        !CreatePipe(&pipe_AP_read, &pipe_AP_write, &sa, 0) ||
        !CreatePipe(&pipe_PS_read, &pipe_PS_write, &sa, 0)) {
        std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
        return 1;
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;



    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = pipe_MA_write;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags = STARTF_USESTDHANDLES;

    std::string cmdM = "M.exe";
    std::vector<char> cmdM_buffer(cmdM.begin(), cmdM.end());
    cmdM_buffer.push_back('\0');

    if (!CreateProcessA(nullptr, cmdM_buffer.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess M failed: " << GetLastError() << std::endl;
        return 1;
    }
    CloseHandle(pi.hThread);  // закрываем дескриптор потока и пайп
    CloseHandle(pipe_MA_write);



    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.hStdInput = pipe_MA_read;
    si.hStdOutput = pipe_AP_write;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags = STARTF_USESTDHANDLES;

    std::string cmdA = "A.exe";
    std::vector<char> cmdA_buffer(cmdA.begin(), cmdA.end());
    cmdA_buffer.push_back('\0');

    if (!CreateProcessA(nullptr, cmdA_buffer.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess A failed: " << GetLastError() << std::endl;
        return 1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pipe_MA_read);
    CloseHandle(pipe_AP_write);



    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.hStdInput = pipe_AP_read;
    si.hStdOutput = pipe_PS_write;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags = STARTF_USESTDHANDLES;

    std::string cmdP = "P.exe";
    std::vector<char> cmdP_buffer(cmdP.begin(), cmdP.end());
    cmdP_buffer.push_back('\0');

    if (!CreateProcessA(nullptr, cmdP_buffer.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess P failed: " << GetLastError() << std::endl;
        return 1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pipe_AP_read);
    CloseHandle(pipe_PS_write);



    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.hStdInput = pipe_PS_read;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags = STARTF_USESTDHANDLES;

    std::string cmdS = "S.exe";
    std::vector<char> cmdS_buffer(cmdS.begin(), cmdS.end());
    cmdS_buffer.push_back('\0');

    if (!CreateProcessA(nullptr, cmdS_buffer.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "CreateProcess S failed: " << GetLastError() << std::endl;
        return 1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pipe_PS_read);


    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);

    std::cout << "Task done." << std::endl;
    return 0;
}