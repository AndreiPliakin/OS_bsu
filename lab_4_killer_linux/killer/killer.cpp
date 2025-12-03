#include <algorithm>
#include <csignal>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

std::vector<pid_t> get_all_pids() {
  std::vector<pid_t> pids;
  DIR *dir = opendir("/proc");

  if (!dir) {
    return pids;
  }

  struct dirent *entry;
  while ((entry = readdir(dir))) {
    if (entry->d_type != DT_DIR) {//pid представлены как поддиректории
      continue;
    }

    std::string name = entry->d_name;

    bool is_number = std::all_of(name.begin(), name.end(),
                                 [](char c) { return std::isdigit(c); });

    if (is_number && !name.empty()) {
      pids.push_back(std::stoi(name));
    }
  }

  closedir(dir);
  return pids;
}

std::string get_process_name(pid_t pid) {
  std::string procPath = "/proc/" + std::to_string(pid) + "/comm"; ///proc/[PID]/comm хранит имя файла
  std::ifstream file(procPath);
  std::string name;
  if (file) {
    std::getline(file, name);
  }
  return name;
}

bool kill_process(pid_t pid) {
  std::cout << "Убиваю процесс с PID: " << pid << std::endl;

  if (kill(pid, SIGTERM) == 0) {
    std::cout << "Отправил SIGTERM процессу " << pid << std::endl;

    sleep(1);

    if (kill(pid, 0) == 0) {
      std::cout << "Процесс " << pid << " еще жив, отправляю SIGKILL"
                << std::endl;
      kill(pid, SIGKILL);
    }
    return true;
  } else {
    std::cout << "Не удалось убить процесс " << pid << std::endl;
    return false;
  }
}

std::vector<pid_t> get_pids_by_name(const std::string &name) {
  std::vector<pid_t> result;
  auto all_pids = get_all_pids();

  for (pid_t pid : all_pids) {
    std::string proc_name = get_process_name(pid);
    if (!proc_name.empty() && proc_name == name) {
      result.push_back(pid);
    }
  }
  return result;
}

std::vector<std::string> parse_proc_to_kill_env() {
  std::vector<std::string> processes;
  const char *env_value = std::getenv("PROC_TO_KILL");

  if (env_value) {
    std::stringstream ss(env_value);
    std::string process;

    while (std::getline(ss, process, ',')) {
      process.erase(0, process.find_first_not_of(" \t"));
      process.erase(process.find_last_not_of(" \t") + 1);

      if (!process.empty()) {
        processes.push_back(process);
      }
    }
  }
  return processes;
}

void kill_by_id(pid_t pid) {
  std::cout << "\n=== Убийство процесса по ID: " << pid << " ===" << std::endl;

  if (kill(pid, 0) == 0) {
    std::string name = get_process_name(pid);
    if (!name.empty()) {
      std::cout << "Найден процесс " << pid << " (" << name << ")" << std::endl;
    }
    kill_process(pid);
  } else {
    std::cout << "Процесс с PID " << pid << " не существует" << std::endl;
  }
}

void kill_by_name(const std::string &name) {
  std::cout << "\n=== Убийство процессов по имени: " << name
            << " ===" << std::endl;

  auto pids = get_pids_by_name(name);

  if (pids.empty()) {
    std::cout << "Не найдено процессов с именем: " << name << std::endl;
    return;
  }

  std::cout << "Найдено " << pids.size() << " процессов с именем '" << name
            << "': ";
  for (size_t i = 0; i < pids.size(); ++i) {
    std::cout << pids[i];
    if (i < pids.size() - 1)
      std::cout << ", ";
  }
  std::cout << std::endl;

  int killed = 0;
  for (pid_t pid : pids) {
    if (kill_process(pid)) {
      killed++;
    }
  }
  std::cout << "Убито " << killed << " процессов" << std::endl;
}

void kill_by_env_variable() {
  std::cout << "\n=== Убийство процессов из PROC_TO_KILL ===" << std::endl;

  auto processes = parse_proc_to_kill_env();

  std::cout << "Процессы для убийства: ";
  for (const auto &proc : processes) {
    std::cout << "'" << proc << "' ";
  }
  std::cout << std::endl;

  for (const auto &process_name : processes) {
    kill_by_name(process_name);
  }
}

void show_menu() {
  std::cout << "\n=== МЕНЮ===" << std::endl;
  std::cout << "1. Убить процесс по ID" << std::endl;
  std::cout << "2. Убить процессы по имени" << std::endl;
  std::cout << "3. Убить процессы из PROC_TO_KILL" << std::endl;
  std::cout << "4. Проверить PROC_TO_KILL" << std::endl;
  std::cout << "5. Выход" << std::endl;
  std::cout << "Выберите опцию (1-5): ";
}

int main() {

  while (true) {
    show_menu();

    int choice;
    std::cin >> choice;

    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
      std::cout << "Некорректный ввод!" << std::endl;
      continue;
    }

    switch (choice) {
    case 1: {
      std::cout << "Введите PID процесса: ";
      int pid;
      std::cin >> pid;
      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Некорректный PID!" << std::endl;
      } else {
        kill_by_id(pid);
      }
      break;
    }

    case 2: {
      std::cout << "Введите имя процесса: ";
      std::string name;
      std::cin >> name;
      kill_by_name(name);
      break;
    }

    case 3: {
      kill_by_env_variable();
      break;
    }

    case 4: {
      std::cout << "\n=== ПРОВЕРКА PROC_TO_KILL ===" << std::endl;
      auto processes = parse_proc_to_kill_env();
      if (processes.empty()) {
        std::cout << "PROC_TO_KILL не установлена" << std::endl;
      } else {
        std::cout << "PROC_TO_KILL содержит: ";
        for (const auto &proc : processes) {
          std::cout << proc << " ";
        }
        std::cout << std::endl;
      }
      break;
    }

    case 5: {
      std::cout << "Выход..." << std::endl;
      return 0;
    }

    default: {
      std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
      break;
    }
    }

    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
  }
}