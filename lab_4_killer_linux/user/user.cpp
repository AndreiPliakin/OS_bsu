#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <string>

pid_t get_pid_by_name(const std::string& name) {
    std::string cmd = "pgrep -f " + name + " | head -1"; //команда для поиска
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return -1;

    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe)) {//читаем строку из потока в выделенный буфер
        pid_t pid = std::stoi(buffer);
        pclose(pipe);
        return pid;
    }

    pclose(pipe);
    return -1;
}

bool process_exists(pid_t pid) {
    return kill(pid, 0) == 0;
}

int main() {
    std::cout << "=== USER APP - УБИЙСТВО РЕАЛЬНЫХ ПРОЦЕССОВ ===" << std::endl;

    std::cout << "\n1. Устанавливаю PROC_TO_KILL = firefox,clion,soffice.bin,libreoffice.wra" << std::endl;
    setenv("PROC_TO_KILL", "firefox,clion,soffice.bin,libreoffice.wra", 1);

    std::string killer = "../killer/killer";

    std::cout << "\n3. ЗАПУСК KILLER:" << std::endl;

    std::cout << "\n   а) Убиваю Firefox по имени:" << std::endl;
    std::string cmd1 = killer + " --name firefox";
    system(cmd1.c_str());

    sleep(2);
    std::cout << "   Проверка Firefox: ";
    system("pgrep -f firefox >/dev/null 2>&1 && echo 'жив ✗' || echo 'убит ✓'");

    pid_t clion_pid = get_pid_by_name("clion");

    if (clion_pid > 0) {
        std::cout << "\n   б) Убиваю Clion по PID " << clion_pid << ":" << std::endl;
        std::string cmd2 = killer + " --id " + std::to_string(clion_pid);
        system(cmd2.c_str());

        sleep(2);
        std::cout << "   Проверка Clion (PID " << clion_pid << "): ";
        if (process_exists(clion_pid)) {
            std::cout << "все еще жив ✗" << std::endl;
        } else {
            std::cout << "убит ✓" << std::endl;
        }
    } else {
        std::cout << "\n   б) Clion не найден, пропускаю убийство по PID" << std::endl;
    }

    std::cout << "\n   в) Убиваю через PROC_TO_KILL (firefox,clion,soffice.bin,libreoffice.wra):" << std::endl;
    system(killer.c_str());

    sleep(2);
    std::cout << "\n   Проверка после PROC_TO_KILL:" << std::endl;
    std::cout << "   - Firefox: ";
    system("pgrep -f firefox >/dev/null 2>&1 && echo 'жив ✗' || echo 'убит ✓'");
    std::cout << "   - Clion: ";
    system("pgrep -f clion >/dev/null 2>&1 && echo 'жив ✗' || echo 'убит ✓'");
    std::cout << "   - soffice.bin (LibreOffice): ";
    system("pgrep -f soffice.bin >/dev/null 2>&1 && echo 'жив ✗' || echo 'убит ✓'");
    std::cout << "   - libreoffice.wra (wrapper): ";
    system("pgrep -f libreoffice.wra >/dev/null 2>&1 && echo 'жив ✗' || echo 'убит ✓'");

    std::cout << "\n4. Удаляю переменную PROC_TO_KILL" << std::endl;
    unsetenv("PROC_TO_KILL");

    char* env_val = getenv("PROC_TO_KILL");
    if (!env_val) {
        std::cout << "   ✓ PROC_TO_KILL удалена" << std::endl;
    } else {
        std::cout << "   ✗ PROC_TO_KILL все еще существует: " << env_val << std::endl;
    }

    std::cout << "\n5. ФИНАЛЬНАЯ ПРОВЕРКА:" << std::endl;

    std::cout << "   Оставшиеся процессы:" << std::endl;
    std::cout << "   - Firefox: ";
    system("pgrep -c firefox 2>/dev/null || echo '0'");
    std::cout << "   - Clion: ";
    system("pgrep -c clion 2>/dev/null || echo '0'");
    std::cout << "   - soffice.bin: ";
    system("pgrep -c soffice.bin 2>/dev/null || echo '0'");
    std::cout << "   - libreoffice.wra: ";
    system("pgrep -c libreoffice.wra 2>/dev/null || echo '0'");

    std::cout << "\n6. Вывод всех процессов (фильтр):" << std::endl;
    system("ps aux | grep -E '(firefox|clion|soffice|libreoffice)' | grep -v grep || echo '   Нет процессов'");

    std::cout << "\n=== ТЕСТ ЗАВЕРШЕН ===" << std::endl;
    
    return 0;
}