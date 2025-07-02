#include "lab_equipment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

std::string getCurrentDate() {
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%02d.%02d.%04d", ltm->tm_mday, ltm->tm_mon + 1, 1900 + ltm->tm_year);
    return std::string(buffer);
}

std::string toLower(const std::string& str) {
    std::string lower = str;
    std::transform(str.begin(), str.end(), lower.begin(), ::tolower);
    return lower;
}

bool containsIgnoreCase(const std::string& str, const std::string& substr) {
    return toLower(str).find(toLower(substr)) != std::string::npos;
}

void checkFiles() {
    std::ifstream f1("devices.txt");
    if (!f1) {
        std::ofstream create("devices.txt");
        std::cout << "[!] Файл базы приборов не найден. Создан новый.\n";
    }
    std::ifstream f2("requests.txt");
    if (!f2) {
        std::ofstream create("requests.txt");
        std::cout << "[!] Файл базы заявок не найден. Создан новый.\n";
    }
}

void loadDevices(std::vector<Device>& devices) {
    devices.clear();
    std::ifstream file("devices.txt");
    Device d;
    while (file >> d.id) {
        file.ignore();
        std::getline(file, d.name);
        std::getline(file, d.status);
        devices.push_back(d);
    }
}

void saveDevices(const std::vector<Device>& devices) {
    std::ofstream file("devices.txt", std::ios::trunc);
    for (const auto& d : devices) {
        file << d.id << "\n" << d.name << "\n" << d.status << "\n";
    }
}

void loadRequests(std::vector<RepairRequest>& requests) {
    requests.clear();
    std::ifstream file("requests.txt");
    RepairRequest r;
    while (file >> r.requestId) {
        file.ignore();
        std::getline(file, r.date);
        std::getline(file, r.name);
        file >> r.deviceId;
        file.ignore();
        std::getline(file, r.description);
        std::getline(file, r.status);
        std::getline(file, r.comment);
        std::getline(file, r.endDate);
        requests.push_back(r);
    }
}

void saveRequests(const std::vector<RepairRequest>& requests) {
    std::ofstream file("requests.txt", std::ios::trunc);
    for (const auto& r : requests) {
        file << r.requestId << "\n"
             << r.date << "\n"
             << r.name << "\n"
             << r.deviceId << "\n"
             << r.description << "\n"
             << r.status << "\n"
             << r.comment << "\n"
             << r.endDate << "\n";
    }
}

void syncDeviceStatus(std::vector<Device>& devices, const std::vector<RepairRequest>& requests) {
    for (auto& d : devices) {
        auto it = std::find_if(requests.begin(), requests.end(), [&](const RepairRequest& r) {
            return r.deviceId == d.id && (r.status == "В ремонте" || r.status == "На рассмотрении");
        });
        d.status = (it != requests.end()) ? "В ремонте" : "Исправен";
    }
}

void showMainMenu() {
    std::cout << "\n=== СИСТЕМА УЧЁТА ОБОРУДОВАНИЯ ===\n"
              << "1. Оборудование\n"
              << "2. Ремонт\n"
              << "0. Выход\n";
}

void handleDeviceMenu(std::vector<Device>& devices, std::vector<RepairRequest>& requests) {
    while (true) {
        std::cout << "\n--- Меню оборудования ---\n"
                  << "1. Добавить\n"
                  << "2. Редактировать\n"
                  << "3. Удалить\n"
                  << "4. Показать список\n"
                  << "5. Поиск\n"
                  << "0. Назад\n"
                  << "Выбор: ";
        int ch;
        std::cin >> ch;
        std::cin.ignore();

        switch (ch) {
            case 1: addDevice(devices); break;
            case 2: editDevice(devices); break;
            case 3: deleteDevice(devices); break;
            case 4: listDevices(devices); break;
            case 5: searchDevices(devices); break;
            case 0: syncDeviceStatus(devices, requests); return;
            default: std::cout << "Неверный выбор.\n";
        }
    }
}

void addDevice(std::vector<Device>& devices) {
    Device d;
    std::cout << "Название прибора: ";
    std::getline(std::cin, d.name);

    int nextId = 1;
    std::vector<int> ids;
    for (const auto& dev : devices) ids.push_back(dev.id);
    while (std::find(ids.begin(), ids.end(), nextId) != ids.end()) ++nextId;
    d.id = nextId;

    d.status = "Исправен";
    devices.push_back(d);
    std::cout << "Прибор добавлен с ID " << d.id << ".\n";
}

void editDevice(std::vector<Device>& devices) {
    int id;
    std::cout << "Введите ID прибора для редактирования: ";
    std::cin >> id;
    std::cin.ignore();

    for (auto& d : devices) {
        if (d.id == id) {
            std::cout << "Новое название (текущее: " << d.name << "): ";
            std::getline(std::cin, d.name);
            std::cout << "Изменено.\n";
            return;
        }
    }
    std::cout << "Прибор не найден.\n";
}

void deleteDevice(std::vector<Device>& devices) {
    int id;
    std::cout << "Введите ID прибора для удаления: ";
    std::cin >> id;
    std::cin.ignore();

    auto it = std::remove_if(devices.begin(), devices.end(), [=](const Device& d) {
        return d.id == id;
    });

    if (it != devices.end()) {
        devices.erase(it, devices.end());
        std::cout << "Прибор удалён.\n";
    } else {
        std::cout << "Прибор не найден.\n";
    }
}

void listDevices(const std::vector<Device>& devices) {
    if (devices.empty()) {
        std::cout << "Список пуст.\n";
        return;
    }

    std::cout << "\nID | Название | Статус\n";
    for (const auto& d : devices) {
        std::cout << d.id << " | " << d.name << " | " << d.status << "\n";
    }
}

void searchDevices(const std::vector<Device>& devices) {
    std::string query;
    std::cout << "Введите строку для поиска по названию или ID: ";
    std::getline(std::cin, query);

    std::cout << "\nРезультаты поиска:\n";
    bool found = false;
    for (const auto& d : devices) {
        if (containsIgnoreCase(d.name, query) || std::to_string(d.id).find(query) != std::string::npos) {
            std::cout << d.id << " | " << d.name << " | " << d.status << "\n";
            found = true;
        }
    }
    if (!found) std::cout << "Ничего не найдено.\n";
}

void handleRepairMenu(std::vector<Device>& devices, std::vector<RepairRequest>& requests) {
    while (true) {
        std::cout << "\n--- Меню ремонта ---\n"
                  << "1. Список заявок\n"
                  << "2. Добавить заявку на ремонт\n"
                  << "3. Редактировать заявку\n"
                  << "4. Удалить заявку\n"
                  << "0. Назад\n"
                  << "Выбор: ";
        int ch;
        std::cin >> ch;
        std::cin.ignore();

        switch (ch) {
            case 1: listRequests(requests); break;
            case 2: addRequest(requests, devices); break;
            case 3: editRequest(requests, devices); break;
            case 4: deleteRequest(requests); break;
            case 0: syncDeviceStatus(devices, requests); return;
            default: std::cout << "Неверный выбор.\n";
        }
    }
}

void addRequest(std::vector<RepairRequest>& requests, const std::vector<Device>& devices) {
    if (devices.empty()) {
        std::cout << "Нет доступных приборов для подачи заявки.\n";
        return;
    }

    RepairRequest r;
    r.date = getCurrentDate();

    int nextId = 1;
    std::vector<int> ids;
    for (const auto& req : requests) ids.push_back(req.requestId);
    while (std::find(ids.begin(), ids.end(), nextId) != ids.end()) ++nextId;
    r.requestId = nextId;

    std::cout << "Название заявки: ";
    std::getline(std::cin, r.name);

    std::cout << "Введите ID прибора: ";
    std::cin >> r.deviceId;
    std::cin.ignore();

    auto it = std::find_if(devices.begin(), devices.end(), [&](const Device& d) {
        return d.id == r.deviceId;
    });

    if (it == devices.end()) {
        std::cout << "Прибор с таким ID не найден.\n";
        return;
    }

    std::cout << "Краткое описание поломки: ";
    std::getline(std::cin, r.description);

    r.status = "На рассмотрении";
    r.comment = "-";
    r.endDate = "-";

    requests.push_back(r);
    std::cout << "Заявка добавлена с номером " << r.requestId << ".\n";
}

void editRequest(std::vector<RepairRequest>& requests, std::vector<Device>& devices) {
    int id;
    std::cout << "Введите номер заявки: ";
    std::cin >> id;
    std::cin.ignore();

    for (auto& r : requests) {
        if (r.requestId == id) {
            std::cout << "Комментарий (от ремонтника): ";
            std::getline(std::cin, r.comment);

            std::cout << "Новый статус:\n"
                      << "1. В ремонте\n"
                      << "2. Готов к эксплуатации\n"
                      << "3. Ремонту не подлежит\n"
                      << "Выбор: ";
            int statusChoice;
            std::cin >> statusChoice;
            std::cin.ignore();

            switch (statusChoice) {
                case 1:
                    r.status = "В ремонте";
                    break;
                case 2:
                    r.status = "Готов к эксплуатации";
                    std::cout << "Введите дату окончания ремонта (дд.мм.гггг): ";
                    std::getline(std::cin, r.endDate);
                    break;
                case 3:
                    r.status = "Ремонту не подлежит";
                    std::cout << "Введите дату окончания ремонта (дд.мм.гггг): ";
                    std::getline(std::cin, r.endDate);
                    break;
                default:
                    std::cout << "Неверный выбор. Статус не изменён.\n";
                    return;
            }

            syncDeviceStatus(devices, requests);
            std::cout << "Заявка обновлена.\n";
            return;
        }
    }
    std::cout << "Заявка не найдена.\n";
}

void deleteRequest(std::vector<RepairRequest>& requests) {
    int id;
    std::cout << "Введите номер заявки для удаления: ";
    std::cin >> id;
    std::cin.ignore();

    auto it = std::remove_if(requests.begin(), requests.end(), [=](const RepairRequest& r) {
        return r.requestId == id;
    });

    if (it != requests.end()) {
        requests.erase(it, requests.end());
        std::cout << "Заявка удалена.\n";
    } else {
        std::cout << "Заявка не найдена.\n";
    }
}

void listRequests(const std::vector<RepairRequest>& requests) {
    if (requests.empty()) {
        std::cout << "Заявок нет.\n";
        return;
    }

    std::cout << "\nID | Дата | Название | Прибор ID | Статус | Конец | Комментарий\n";
    for (const auto& r : requests) {
        std::cout << r.requestId << " | "
                  << r.date << " | "
                  << r.name << " | "
                  << r.deviceId << " | "
                  << r.status << " | "
                  << r.endDate << " | "
                  << r.comment << "\n";
    }
}
