#include "lab_equipment.hpp"
#include <iostream>

int main() {
    std::vector<Device> devices;
    std::vector<RepairRequest> requests;

    checkFiles();
    loadDevices(devices);
    loadRequests(requests);
    syncDeviceStatus(devices, requests);

    while (true) {
        showMainMenu();
        int choice;
        std::cout << "Выберите пункт: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                handleDeviceMenu(devices, requests);
                break;
            case 2:
                handleRepairMenu(devices, requests);
                break;
            case 0:
                saveDevices(devices);
                saveRequests(requests);
                std::cout << "Выход из программы...\n";
                return 0;
            default:
                std::cout << "Неверный выбор. Повторите ввод.\n";
        }
    }
}
