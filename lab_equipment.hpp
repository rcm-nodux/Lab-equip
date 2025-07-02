#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <string>
#include <vector>

struct Device {
    int id;
    std::string name;
    std::string status;
};

struct RepairRequest {
    int requestId;
    std::string date;
    std::string name;
    int deviceId;
    std::string description;
    std::string status;
    std::string comment;
    std::string endDate;
};

void checkFiles();
void loadDevices(std::vector<Device>& devices);
void saveDevices(const std::vector<Device>& devices);
void loadRequests(std::vector<RepairRequest>& requests);
void saveRequests(const std::vector<RepairRequest>& requests);

void showMainMenu();
void handleDeviceMenu(std::vector<Device>& devices, std::vector<RepairRequest>& requests);
void handleRepairMenu(std::vector<Device>& devices, std::vector<RepairRequest>& requests);

void addDevice(std::vector<Device>& devices);
void editDevice(std::vector<Device>& devices);
void deleteDevice(std::vector<Device>& devices);
void listDevices(const std::vector<Device>& devices);
void searchDevices(const std::vector<Device>& devices);

void syncDeviceStatus(std::vector<Device>& devices, const std::vector<RepairRequest>& requests);

void addRequest(std::vector<RepairRequest>& requests, const std::vector<Device>& devices);
void editRequest(std::vector<RepairRequest>& requests, std::vector<Device>& devices);
void deleteRequest(std::vector<RepairRequest>& requests);
void listRequests(const std::vector<RepairRequest>& requests);

std::string getCurrentDate();
std::string toLower(const std::string& str);
bool containsIgnoreCase(const std::string& str, const std::string& substr);

#endif
