#ifndef CONTROLPANEL_DBMS_HPP
#define CONTROLPANEL_DBMS_HPP

#include "Related.hpp"

// STL
#include <set>

#include "sqlite_cpp/sqlite_modern_cpp.h"

class DataManager {
private:
    std::string databaseName;
    sqlite::database database;

public:
    explicit DataManager(std::string databaseName_arg = "PlugProtectorDevices")
            : databaseName(std::move(databaseName_arg)), database(this->databaseName) {
    }

    static bool GetAppState(void) {
        return RegistryManipulators::GetItemPropertyValue("State", Requirements::defaultRegistryContainerPath) == "ON";
    }

    static bool GetDestructionState(void) {
        return RegistryManipulators::GetItemPropertyValue(
                "DestructionMode",
                Requirements::defaultRegistryContainerPath) == "TRUE";
    }

    static bool GetDestructionMode(void) {
        return RegistryManipulators::GetItemPropertyValue(
                "Reversible",
                Requirements::defaultRegistryContainerPath) == "TRUE";
    }

    static void PrintState(const std::string &startMessageArg, const std::string &stateArg, bool topPaddingArg = false,
                           bool bottomPaddingArg = true) {
        CLI::ConsoleOut((topPaddingArg ? "\n" : "") + startMessageArg + "[" +
                        stateArg + "]" + (bottomPaddingArg ? "\n" : ""));
    }

    static void ChangeAppState(bool stateArg = true, uint32_t sleepInMsArg = 0) {
        if (sleepInMsArg > 15) {
            System::Sleeps::SleepForMillisecond(sleepInMsArg);
        }
        RegistryManipulators::SetOrEditItemPropertyValue(
                (stateArg ? "ON" : "OFF"), "State",
                Requirements::defaultRegistryContainerPath);
    }

    static std::string RetrieveAllDevicesAsString(void) {

        std::string retrievedDevices_string = System::ExecuteCommand(Requirements::retrieveAllDevices_Command);
        StringManipulators::Replace::ReplaceAll(retrievedDevices_string, "$0L1F30$", "\"");
        StringManipulators::Replace::ReplaceAll(retrievedDevices_string, "\\", "\\\\");
        StringManipulators::Replace::ReplaceAll(retrievedDevices_string, "}}\n{", "}, ");
        retrievedDevices_string = StringManipulators::Trim::trim(retrievedDevices_string);
        return retrievedDevices_string;
    }

    Devices RetrieveTrustedDevices(void) {
        Devices deviceHolderTemp;
        for (std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, int64_t, int64_t, int32_t, uint32_t> row
                : database
                << "SELECT ID, SerialNumber, Name, Label, DriveType, FileSystem, DriveLetter, Caption, Capacity, FreeSpace, BlockSize, BytesPerSector FROM TrustedDevices") {

            deviceHolderTemp.insert(std::make_pair(std::string(std::get<0>(row)),
                                                   deviceStructure(
                                                           std::get<0>(row), std::get<1>(row), std::get<2>(row),
                                                           std::get<3>(row), std::get<4>(row), std::get<5>(row),
                                                           std::get<6>(row), std::get<7>(row), std::get<8>(row),
                                                           std::get<9>(row), std::get<10>(row), std::get<11>(row))));
        }

        return deviceHolderTemp;
    }

    bool Contains(const std::string &ID, const std::string &tableName_arg = "TrustedDevices") {
        bool doesContains{false};
        database << "SELECT EXISTS(SELECT ID FROM " + tableName_arg + " WHERE ID=? LIMIT 1);" << ID >> doesContains;
        return doesContains;
    }

    Devices RetrieveUntrustedDevices(void) {
        Devices deviceHolderTemp;
        std::string devicesAsString_arg = RetrieveAllDevicesAsString();
        if (devicesAsString_arg.length() >= 0) {
            rapidjson::Document jsonDocument_Devices = JsonManipulators::JsonParse(devicesAsString_arg);
            for (rapidjson::GenericMember<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &jsonDocument_DeviceItem: jsonDocument_Devices.GetObj()) {
                rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &jsonDeviceInformationHolderTemp = jsonDocument_DeviceItem.value;
                if (!this->Contains(jsonDocument_DeviceItem.name.GetString())) {
                    deviceHolderTemp.insert(
                            std::make_pair(
                                    jsonDocument_DeviceItem.name.GetString(),
                                    deviceStructure(
                                            jsonDocument_DeviceItem.name.GetString(),
                                            jsonDeviceInformationHolderTemp["SerialNumber"].GetString(),
                                            jsonDeviceInformationHolderTemp["Name"].GetString(),
                                            jsonDeviceInformationHolderTemp["Label"].GetString(),
                                            jsonDeviceInformationHolderTemp["DriveType"].GetString(),
                                            jsonDeviceInformationHolderTemp["FileSystem"].GetString(),
                                            jsonDeviceInformationHolderTemp["DriveLetter"].GetString(),
                                            jsonDeviceInformationHolderTemp["Caption"].GetString(),
                                            jsonDeviceInformationHolderTemp["Capacity"].GetInt64(),
                                            jsonDeviceInformationHolderTemp["FreeSpace"].GetInt64(),
                                            jsonDeviceInformationHolderTemp["BlockSize"].GetInt(),
                                            System::GetDeviceBytesPerSector(System::GetDiskGeometry(
                                                    IOManipulators::FileManipulators::Operations::CreateFileRW(
                                                            jsonDocument_DeviceItem.name.GetString()), true).second)
                                    )
                            ));
                }
            }
        }
        return deviceHolderTemp;
    }

    void RemoveTrustedDevice(const std::string &ID_arg) {
        database << "DELETE FROM TrustedDevices WHERE ID=?;" << ID_arg;
    }

    void InsertNewDevice(const deviceStructure &deviceArg) {
        database
                << "INSERT OR REPLACE INTO TrustedDevices (ID, SerialNumber, Name, Label, DriveType, FileSystem, DriveLetter, Caption, Capacity, FreeSpace, BlockSize, BytesPerSector) VALUES (?,?,?,?,?,?,?,?,?,?,?,?);"
                << deviceArg.ID
                << deviceArg.SerialNumber
                << deviceArg.Name
                << deviceArg.Label
                << deviceArg.DriveType
                << deviceArg.FileSystem
                << deviceArg.DriveLetter
                << deviceArg.Caption
                << deviceArg.Capacity
                << deviceArg.FreeSpace
                << deviceArg.BlockSize
                << deviceArg.BytesPerSector;
    }
};

#endif //CONTROLPANEL_DBMS_HPP
