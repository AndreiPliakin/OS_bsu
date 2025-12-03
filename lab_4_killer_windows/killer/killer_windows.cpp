#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <tlhelp32.h>
#include <cstdlib>

namespace windows_utils {
    std::vector<DWORD> get_pids_by_name(const std::string& name) {
        std::vector<DWORD> result;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return result;
        }

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe)) {
            do {
                std::string exeName = pe.szExeFile;
                if (_stricmp(exeName.c_str(), name.c_str()) == 0) {
                    result.push_back(pe.th32ProcessID);
                }
            } while (Process32Next(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);
        return result;
    }

    std::string get_process_name(DWORD pid) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return "";
        }

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe)) {
            do {
                if (pe.th32ProcessID == pid) {
                    CloseHandle(hSnapshot);
                    return std::string(pe.szExeFile);
                }
            } while (Process32Next(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);
        return "";
    }

    bool kill_process(DWORD pid) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess == nullptr) {
            return false;
        }

        bool success = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return success;
    }
}

std::vector<std::string> parse_proc_to_kill_env() {
    std::vector<std::string> processes;
    char* env_value;
    size_t len;
    errno_t err = _dupenv_s(&env_value, &len, "PROC_TO_KILL");

    if (err == 0 && env_value != nullptr) {
        std::stringstream ss(env_value);
        std::string process;

        while (std::getline(ss, process, ',')) {
            process.erase(0, process.find_first_not_of(" \t"));
            process.erase(process.find_last_not_of(" \t") + 1);

            if (!process.empty()) {
                processes.push_back(process);
            }
        }
        free(env_value);
    }
    return processes;
}

void kill_by_id(DWORD pid) {
    std::cout << "=== Killing process by ID: " << pid << " ===" << std::endl;

    std::string name = windows_utils::get_process_name(pid);
    if (!name.empty()) {
        std::cout << "Found: " << pid << " (" << name << ")" << std::endl;
    } else {
        std::cout << "Process " << pid << " not found!" << std::endl;
        return;
    }

    if (windows_utils::kill_process(pid)) {
        std::cout << "Process " << pid << " killed successfully" << std::endl;
    } else {
        std::cout << "Failed to kill process " << pid << std::endl;
    }
}

void kill_by_name(const std::string& name) {
    std::cout << "=== Killing processes by name: " << name << " ===" << std::endl;

    auto pids = windows_utils::get_pids_by_name(name);

    if (pids.empty()) {
        std::cout << "No processes found with name: " << name << std::endl;
        return;
    }

    std::cout << "Found " << pids.size() << " process(es): ";
    for (size_t i = 0; i < pids.size(); ++i) {
        std::cout << pids[i];
        if (i < pids.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    int killed = 0;
    for (DWORD pid : pids) {
        if (windows_utils::kill_process(pid)) {
            killed++;
            std::cout << "Killed PID: " << pid << std::endl;
        }
    }

    std::cout << "Successfully killed " << killed << " process(es)" << std::endl;
}

void kill_by_env_variable() {
    std::cout << "=== Killing processes from PROC_TO_KILL ===" << std::endl;

    auto processes = parse_proc_to_kill_env();

    if (processes.empty()) {
        std::cout << "PROC_TO_KILL is not set or empty" << std::endl;
        return;
    }

    std::cout << "Processes to kill: ";
    for (const auto& proc : processes) {
        std::cout << "'" << proc << "' ";
    }
    std::cout << std::endl;

    for (const auto& process_name : processes) {
        kill_by_name(process_name);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        kill_by_env_variable();
        return 0;
    }

    std::string arg = argv[1];

    if (arg == "--id" && argc > 2) {
        try {
            DWORD pid = std::stoul(argv[2]);
            kill_by_id(pid);
        } catch (...) {
            std::cerr << "Invalid PID: " << argv[2] << std::endl;
            return 1;
        }
    }
    else if (arg == "--name" && argc > 2) {
        kill_by_name(argv[2]);
    }
    else if (arg == "--env") {
        kill_by_env_variable();
    }
    else if (arg == "--help") {
        std::cout << "Usage:" << std::endl;
        std::cout << "  killer.exe                     - Kill from PROC_TO_KILL env var" << std::endl;
        std::cout << "  killer.exe --id <pid>         - Kill process by PID" << std::endl;
        std::cout << "  killer.exe --name <name>      - Kill all processes by name" << std::endl;
        std::cout << "  killer.exe --env              - Kill from PROC_TO_KILL env var" << std::endl;
        std::cout << "  killer.exe --help             - Show this help" << std::endl;
    }
    else {
        std::cerr << "Unknown argument: " << arg << std::endl;
        std::cerr << "Use --help for usage information" << std::endl;
        return 1;
    }

    return 0;
}