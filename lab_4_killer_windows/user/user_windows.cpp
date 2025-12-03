#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <vector>

using namespace std::chrono_literals;
namespace fs = std::filesystem;

DWORD get_pid_by_name(const std::string& name) {
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            std::string exeName = pe.szExeFile;
            if (_stricmp(exeName.c_str(), name.c_str()) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return pid;
}

std::vector<DWORD> get_all_pids_by_name(const std::string& name) {
    std::vector<DWORD> pids;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return pids;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            std::string exeName = pe.szExeFile;
            if (_stricmp(exeName.c_str(), name.c_str()) == 0) {
                pids.push_back(pe.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return pids;
}

bool process_exists(DWORD pid) {
    if (pid == 0) return false;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess == NULL) return false;

    DWORD exitCode;
    bool exists = GetExitCodeProcess(hProcess, &exitCode) && (exitCode == STILL_ACTIVE);
    CloseHandle(hProcess);
    return exists;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== USER APP - COMPLETE KILLER TEST ===\n" << std::endl;
    std::cout << "Testing 3 modes of killer.exe:\n";
    std::cout << "1. By PID (--id) - Telegram\n";
    std::cout << "2. By name (--name) - Calculator\n";
    std::cout << "3. By env variable (PROC_TO_KILL) - Word\n" << std::endl;

    std::cout << "1. PREPARING PATHS..." << std::endl;

    char current_dir_buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, current_dir_buffer);
    std::string current_dir_str(current_dir_buffer);

    std::string killer_path;
    std::string possible_paths[] = {
        "killer.exe",
        ".\\killer.exe",
        "..\\killer.exe",
        current_dir_str + "\\killer.exe",
        current_dir_str + "\\..\\killer.exe",
        current_dir_str + "\\..\\build\\killer.exe",
        current_dir_str + "\\..\\build\\killer\\killer.exe",
        "D:\\OS_bsu\\lab_4_killer_windows\\build\\killer\\killer.exe"
    };

    bool found = false;
    for (const auto& path : possible_paths) {
        if (fs::exists(path)) {
            killer_path = fs::absolute(path).string();
            std::cout << "   Found killer.exe at: " << killer_path << std::endl;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "   ERROR: killer.exe not found!" << std::endl;
    }

    std::cout << "\n2. STARTING TEST APPLICATIONS..." << std::endl;

    std::cout << "   Starting Calculator..." << std::endl;
    system("start calc.exe");
    std::this_thread::sleep_for(3s);

    std::cout << "   Starting Word (winword.exe)..." << std::endl;
    system("start winword.exe");
    std::this_thread::sleep_for(3s);

    std::cout << "\n\n3. TEST 1: KILL BY PID (--id) - TELEGRAM" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::vector<DWORD> telegram_pids = get_all_pids_by_name("Telegram.exe");
    if (!telegram_pids.empty()) {
        DWORD telegram_pid = telegram_pids[0];
        std::cout << "Telegram PID: " << telegram_pid << std::endl;

        std::string cmd = "\"" + killer_path + "\" --id " + std::to_string(telegram_pid);
        std::cout << "Running: " << cmd << std::endl;

        system(cmd.c_str());
        std::this_thread::sleep_for(2s);

        if (process_exists(telegram_pid)) {
            std::cout << "   ✗ FAILED: Telegram still running!" << std::endl;
        } else {
            std::cout << "   ✓ SUCCESS: Telegram killed!" << std::endl;
        }
    } else {
        std::cout << "   Telegram not running, skipping --id test" << std::endl;
    }

    std::cout << "\n\n4. TEST 2: KILL BY NAME (--name) - CALCULATOR" << std::endl;
    std::cout << "==============================================" << std::endl;

    std::vector<DWORD> calc_pids = get_all_pids_by_name("CalculatorApp.exe");
    if (calc_pids.empty()) {
        calc_pids = get_all_pids_by_name("calc.exe");
    }

    if (!calc_pids.empty()) {
        std::cout << "Calculator PIDs found: ";
        for (DWORD pid : calc_pids) std::cout << pid << " ";
        std::cout << std::endl;

        std::string cmd = "\"" + killer_path + "\" --name CalculatorApp.exe";
        std::cout << "Running: " << cmd << std::endl;

        system(cmd.c_str());
        std::this_thread::sleep_for(2s);

        calc_pids = get_all_pids_by_name("CalculatorApp.exe");
        if (calc_pids.empty()) calc_pids = get_all_pids_by_name("calc.exe");

        if (calc_pids.empty()) {
            std::cout << "   ✓ SUCCESS: Calculator killed!" << std::endl;
        } else {
            std::cout << "   ✗ FAILED: Calculator still running!" << std::endl;
        }
    } else {
        std::cout << "   Calculator not running" << std::endl;
    }

    std::cout << "\n\n5. TEST 3: KILL BY ENV VARIABLE - WORD" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "Setting PROC_TO_KILL=WINWORD.EXE" << std::endl;
    _putenv_s("PROC_TO_KILL", "WINWORD.EXE");

    std::vector<DWORD> word_pids = get_all_pids_by_name("WINWORD.EXE");
    if (!word_pids.empty()) {
        std::cout << "Word PIDs found: ";
        for (DWORD pid : word_pids) std::cout << pid << " ";
        std::cout << std::endl;

        std::string cmd = "\"" + killer_path + "\"";
        std::cout << "Running: " << cmd << " (reads PROC_TO_KILL)" << std::endl;

        system(cmd.c_str());
        std::this_thread::sleep_for(2s);

        word_pids = get_all_pids_by_name("WINWORD.EXE");
        if (word_pids.empty()) {
            std::cout << "   ✓ SUCCESS: Word killed!" << std::endl;
        } else {
            std::cout << "   ✗ FAILED: Word still running!" << std::endl;
        }
    } else {
        std::cout << "   Word not running" << std::endl;
    }

    std::cout << "\n\n6. CLEANUP" << std::endl;
    std::cout << "==========" << std::endl;

    std::cout << "Removing PROC_TO_KILL..." << std::endl;
    _putenv_s("PROC_TO_KILL", "");ы

    std::cout << "\n=== TEST COMPLETED ===" << std::endl;
    system("pause");
    return 0;
}