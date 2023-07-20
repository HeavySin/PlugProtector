#ifndef SYSTEMPROTECTION_HPP
#define SYSTEMPROTECTION_HPP

#include "booster.hpp"

#include <iostream>
#include <unistd.h>
#include <thread>

// STL
#include <utility>
#include <vector>
#include <map>
#include <set>


// Third-Party
#include "sqlite_cpp/sqlite_modern_cpp.h"

class Requirements {
public:
    static inline std::string defaultRegistryContainerName{"_internal"};
    static inline std::string defaultRegistrySubContainer{"Control Panel\\Keyboard\\"};
    static inline std::string defaultRegistryContainerPath{defaultRegistrySubContainer + defaultRegistryContainerName};
};

struct deviceStructure {
public:
    std::string ID;
    std::string SerialNumber;
    std::string Name;
    std::string Label;
    std::string DriveType;
    std::string FileSystem;
    std::string DriveLetter;
    std::string Caption;
    int64_t Capacity{0};
    int64_t FreeSpace{0};
    int32_t BlockSize{0};
    uint32_t BytesPerSector{512};
};


class DataManager {
private:
    std::string databaseName;
    sqlite::database database;

public:
    explicit DataManager(std::string databaseName_arg = "PlugProtectorDevices")
            : databaseName(std::move(databaseName_arg)), database(this->databaseName) {
        this->ExecuteQuery(
                "CREATE TABLE IF NOT EXISTS TrustedDevices ("
                "   ID TEXT UNIQUE NOT NULL,"
                "   SerialNumber TEXT DEFAULT '',"
                "   Name TEXT DEFAULT '',"
                "   Label TEXT DEFAULT '',"
                "   DriveType TEXT DEFAULT '',"
                "   FileSystem TEXT DEFAULT '',"
                "   DriveLetter TEXT DEFAULT '',"
                "   Caption TEXT DEFAULT '',"
                "   Capacity INTEGER DEFAULT 0,"
                "   FreeSpace INTEGER DEFAULT 0,"
                "   BlockSize INTEGER DEFAULT 0,"
                "   BytesPerSector INTEGER DEFAULT 512"
                ");");

        this->ExecuteQuery(
                "CREATE TABLE IF NOT EXISTS FuckedDevices ("
                "   ID TEXT UNIQUE NOT NULL,"
                "   _Secret TEXT DEFAULT 'NOT_SET'"
                ");");
    }


    void ExecuteQuery(const std::string_view &query_arg) {
        database.operator<<(query_arg);
    }

    uint64_t GetNumberOfRows(const std::string &tableName_arg = "TrustedDevices") {
        uint64_t numOfRows{0};
        database << "SELECT COUNT(*) FROM " + tableName_arg >> numOfRows;
        return numOfRows;
    }

    std::string GetDeviceSecret(const std::string &ID_arg) {
        std::string _secretHolder;
        database << "SELECT _Secret FROM FuckedDevices WHERE ID=? LIMIT 1;" << ID_arg >> _secretHolder;
        return _secretHolder;
    }

    bool Contains(const std::string &ID, const std::string &tableName_arg = "TrustedDevices") {
        bool doesContains{false};
        database << "SELECT EXISTS(SELECT ID FROM " + tableName_arg + " WHERE ID=? LIMIT 1);" << ID >> doesContains;
        return doesContains;
    }

    std::set<std::string> UntrustedDevices(const std::string &devicesAsString_arg) {
        std::set<std::string> untrustedDevices{};
        if (devicesAsString_arg.length() >= 0) {
            rapidjson::Document jsonDocument_Devices = JsonManipulators::JsonParse(devicesAsString_arg);
            for (rapidjson::GenericMember<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &jsonDocument_DeviceItem: jsonDocument_Devices.GetObj()) {
                if (!this->Contains(jsonDocument_DeviceItem.name.GetString())) {
                    untrustedDevices.insert(jsonDocument_DeviceItem.name.GetString());
                }
            }
        }
        return untrustedDevices;
    }

    void InsertDevicesFromString(const std::string &devicesAsString_arg) {
        if (devicesAsString_arg.length() >= 0) {
            rapidjson::Document jsonDocument_Devices = JsonManipulators::JsonParse(devicesAsString_arg);
            for (rapidjson::GenericMember<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &jsonDocument_DeviceItem: jsonDocument_Devices.GetObj()) {
                rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> &jsonDeviceInformationHolderTemp = jsonDocument_DeviceItem.value;
                database
                        << "INSERT OR REPLACE INTO TrustedDevices (ID, SerialNumber, Name, Label, DriveType, FileSystem, DriveLetter, Caption, Capacity, FreeSpace, BlockSize, BytesPerSector) VALUES (?,?,?,?,?,?,?,?,?,?,?,?);"
                        << jsonDocument_DeviceItem.name.GetString()
                        << jsonDeviceInformationHolderTemp["SerialNumber"].GetString()
                        << jsonDeviceInformationHolderTemp["Name"].GetString()
                        << jsonDeviceInformationHolderTemp["Label"].GetString()
                        << jsonDeviceInformationHolderTemp["DriveType"].GetString()
                        << jsonDeviceInformationHolderTemp["FileSystem"].GetString()
                        << jsonDeviceInformationHolderTemp["DriveLetter"].GetString()
                        << jsonDeviceInformationHolderTemp["Caption"].GetString()
                        << jsonDeviceInformationHolderTemp["Capacity"].GetInt64()
                        << jsonDeviceInformationHolderTemp["FreeSpace"].GetInt64()
                        << jsonDeviceInformationHolderTemp["BlockSize"].GetInt()
                        << System::GetDeviceBytesPerSector(System::GetDiskGeometry(
                                IOManipulators::FileManipulators::Operations::CreateFileRW(
                                        jsonDocument_DeviceItem.name.GetString()), true).second);
            }
        }
    }

    void InsertFuckedDevice(const std::string &ID_arg, const std::string &_secret_arg) {
        if (!(StringManipulators::Occurrence::occurrence0(_secret_arg, '0') >= 400
              ||
              _secret_arg.contains(StringManipulators::Convertors::TextToHex::StringToHex2("prettyFuckedUp")))) {
            database << "INSERT OR REPLACE INTO FuckedDevices (ID, _Secret) VALUES (?,?);" << ID_arg << _secret_arg;
            CLI::ConsoleOut("ADDED TO DB");
        } else {
            CLI::ConsoleOut("ALREADY FUCKED UP");
        }
    }
};

class DriveProtection {
private:
    DataManager DBMS_Instance;

    std::string retrieveAllDevices_Command = "powershell.exe -command \"$hash = $null;$hash = @{};"
                                             "$usb_dev = Get-WmiObject win32_volume | where{$_.DriveType -eq '5' -or $_.DriveType -eq '3' -or $_.DriveType -eq '2'};"
                                             "$damn = 0;"
                                             "foreach ($p in $usb_dev)"
                                             "{"
                                             "If ($p.Capacity -eq $null -or $p.Capacity -lt 0) {$p.Capacity = 0};"
                                             "If ($p.FreeSpace -eq $null -or $p.FreeSpace -lt 0) {$p.FreeSpace = 0};"
                                             "If ($p.BlockSize -eq $null -or $p.BlockSize -lt 0) {$p.BlockSize = 0};"
                                             "$finallText = "
                                             "'{' + '$0L1F30${0}$0L1F30$: ' -f $p.DeviceID + "
                                             "'{'+'"
                                             "$0L1F30$SerialNumber$0L1F30$: $0L1F30${0}$0L1F30$, "
                                             "$0L1F30$Name$0L1F30$: $0L1F30${1}$0L1F30$, "
                                             "$0L1F30$Label$0L1F30$: $0L1F30${2}$0L1F30$, "
                                             "$0L1F30$DriveType$0L1F30$: $0L1F30${3}$0L1F30$, "
                                             "$0L1F30$DriveLetter$0L1F30$: $0L1F30${4}$0L1F30$, "
                                             "$0L1F30$Caption$0L1F30$: $0L1F30${5}$0L1F30$, "
                                             "$0L1F30$Capacity$0L1F30$: {6}, "
                                             "$0L1F30$FreeSpace$0L1F30$: {7}, "
                                             "$0L1F30$FileSystem$0L1F30$: $0L1F30${8}$0L1F30$, "
                                             "$0L1F30$BlockSize$0L1F30$: {9}'"
                                             " -f $p.SerialNumber, $p.Name, $p.Label, $p.DriveType, "
                                             "$p.DriveLetter, $p.Caption, $p.Capacity, $p.FreeSpace, "
                                             "$p.FileSystem, $p.BlockSize + "
                                             "'}' + "
                                             "'}' ; "
                                             "$hash.add($damn, $finallText); $damn+=1};"
                                             "$damn=0;"
                                             "$all_data = $hash | Select-Object -Property *;"
                                             "echo $all_data.Values;\"";


//    typedef std::map<std::string, struct deviceStructure> Devices;
public:
    explicit DriveProtection(DataManager DBMS_arg) : DBMS_Instance(std::move(DBMS_arg)) {
        // Some basic program Configuration
        if (DBMS_Instance.GetNumberOfRows() <= 0) {
            DBMS_Instance.InsertDevicesFromString(this->getAllDevicesAsString());
        }

        if (!RegistryManipulators::checkForItemContainerExistence(
                RegistryManipulators::_internal::defaultRegistryItemContainerPath_arg)) {
            RegistryManipulators::NewItemContainer(Requirements::defaultRegistryContainerName,
                                                   Requirements::defaultRegistrySubContainer);
        }

        if (RegistryManipulators::checkForItemPropertyValueExistence("PlugProtectorInstalled",
                                                                     Requirements::defaultRegistryContainerPath) !=
            "DOES_EXISTS") {
            RegistryManipulators::SetOrEditItemPropertyValue("TRUE", "PlugProtectorInstalled",
                                                             Requirements::defaultRegistryContainerPath);
        }

        if (RegistryManipulators::checkForItemPropertyValueExistence("State",
                                                                     Requirements::defaultRegistryContainerPath) !=
            "DOES_EXISTS") {
            RegistryManipulators::SetOrEditItemPropertyValue("ON", "State",
                                                             Requirements::defaultRegistryContainerPath);
        }

        if (RegistryManipulators::checkForItemPropertyValueExistence("DestructionMode",
                                                                     Requirements::defaultRegistryContainerPath) !=
            "DOES_EXISTS") {
            RegistryManipulators::SetOrEditItemPropertyValue("FALSE", "DestructionMode",
                                                             Requirements::defaultRegistryContainerPath);
        }

        if (RegistryManipulators::checkForItemPropertyValueExistence("Reversible",
                                                                     Requirements::defaultRegistryContainerPath) !=
            "DOES_EXISTS") {
            RegistryManipulators::SetOrEditItemPropertyValue("TRUE", "Reversible",
                                                             Requirements::defaultRegistryContainerPath);
        }

        // The program goes into `Destruction Mode` since it can't locate "RemoveDrive.exe" dependency!
        if (!IOManipulators::FileManipulators::Existence::CheckExistence4(
                System::Paths::GetAppDataDirectory0() + "\\RemoveDrive.exe")) {
            RegistryManipulators::SetOrEditItemPropertyValue("TRUE", "DestructionMode",
                                                             Requirements::defaultRegistryContainerPath);
        }
    }

    static bool GetAppState(void) {
        return RegistryManipulators::GetItemPropertyValue(
                "State",
                Requirements::defaultRegistryContainerPath) == "ON";
    }

    [[nodiscard]] std::string getAllDevicesAsString(void) const {

        std::string retrievedDevices_string = System::ExecuteCommand(this->retrieveAllDevices_Command);
        StringManipulators::Replace::ReplaceAll(retrievedDevices_string, "$0L1F30$", "\"");
        StringManipulators::Replace::ReplaceAll(retrievedDevices_string, "\\", "\\\\");
        StringManipulators::Replace::ReplaceAll(retrievedDevices_string, "}}\n{", "}, ");
        retrievedDevices_string = StringManipulators::Trim::trim(retrievedDevices_string);
        return retrievedDevices_string;
    }


    static void DriveEjector(const std::string &ID) {

        System::Sleeps::SleepForMillisecond(1000);
        std::string commandDriveLetterNull = RegistryManipulators::_internal::powershellExecutionCommand +
                                             " \"$usbdev = gwmi win32_volume | where{$_.DeviceID -eq \'" + ID +
                                             "\'}; $usbdev.DriveLetter = $null;$usbdev.Put()\"";
        std::string commandDriveEjector =
                System::Paths::GetAppDataDirectory0() + "\\RemoveDrive.exe \"" + ID + "\" -e -f -l & " +
                System::Paths::GetAppDataDirectory0() + "\\RemoveDrive.exe \"" + ID + "\" -e -f";
        System::ExecuteCommand(commandDriveLetterNull);
        System::Sleeps::SleepForMillisecond(750);
        System::ExecuteCommand(commandDriveEjector);
    }

    static std::pair<std::string, BoosterException::InputOutput::InputOutputException>
    GetDriveSecret(HANDLE &fileHandle_arg, uint64_t bytesPerSector_arg = 512) {
        std::pair<std::string, BoosterException::InputOutput::InputOutputException> returnHolder("",
                                                                                                 BoosterException::InputOutput::InputOutputException::Success);

        // Setting the file pointer to the first of the driveHandler to begin corrupting the 0 sector
        if (!IOManipulators::FileManipulators::Attributes::SetFilePointerZ(fileHandle_arg, 0, FILE_BEGIN)) {
            returnHolder.second = BoosterException::InputOutput::InputOutputException::FailedToSetFilePointer;
            return returnHolder;
        }

        std::pair<std::string, BoosterException::InputOutput::InputOutputException>
                returnHolderReadFileAsHexR = IOManipulators::FileManipulators::Operations::Read::ReadFileAsHexR(
                fileHandle_arg, bytesPerSector_arg);
        if (returnHolderReadFileAsHexR.second != BoosterException::InputOutput::InputOutputException::Success) {
            returnHolder.second = returnHolderReadFileAsHexR.second;
            return returnHolder;
        }
        returnHolder.first = returnHolderReadFileAsHexR.first;

        return returnHolder;
    }

    static std::pair<std::string, BoosterException::InputOutput::InputOutputException>
    DriveHeaderCorrupter(const std::string &driveID_arg) {
        std::pair<std::string, BoosterException::InputOutput::InputOutputException>
                returnHolder("NOT_SET",
                             BoosterException::InputOutput::InputOutputException::Success);

        // Set the drive handler to *driveID_arg*
        HANDLE driveHandler = IOManipulators::FileManipulators::Operations::CreateFileRW(driveID_arg);
        if (driveHandler == INVALID_HANDLE_VALUE) {
            returnHolder.second = BoosterException::InputOutput::InputOutputException::InvalidHandler;
            CloseHandle(driveHandler);
            return returnHolder;
        }

        // Getting the BytesPerSector for sector alignment
        uint32_t bytesPerSector
                = System::GetDeviceBytesPerSector(System::GetDiskGeometry(driveHandler).second);

        // 0th Sector Holder `ONLY WHEN REVERSIBLE MODE IS ON`
        if (RegistryManipulators::GetItemPropertyValue(
                "Reversible",
                Requirements::defaultRegistryContainerPath)
            == "TRUE") {

            std::pair<std::string, BoosterException::InputOutput::InputOutputException>
                    returnHolderGetDriveSecret(
                    "NOT_SET",
                    BoosterException::InputOutput::InputOutputException::Success);

            returnHolderGetDriveSecret = DriveProtection::GetDriveSecret(driveHandler, bytesPerSector);
            if (returnHolderGetDriveSecret.second != BoosterException::InputOutput::InputOutputException::Success) {
                returnHolder.second = returnHolderGetDriveSecret.second;
                CloseHandle(driveHandler);
                return returnHolder;
            }
            returnHolder.first = returnHolderGetDriveSecret.first;
        }

        std::string dataBuffer(bytesPerSector, '\0');
        std::string comment{" prettyFuckedUp, Isnt It?"};
        dataBuffer.replace(0, comment.size(), comment);

        if (!IOManipulators::FileManipulators::Attributes::SetFilePointerZ(driveHandler, 0, FILE_BEGIN)) {
            returnHolder.second = BoosterException::InputOutput::InputOutputException::FailedToSetFilePointer;
            CloseHandle(driveHandler);
            return returnHolder;
        }

        BoosterException::InputOutput::InputOutputException returnHolderWriteFileW = IOManipulators::FileManipulators::Operations::Write::WriteFileW(
                driveHandler, dataBuffer);
        if (returnHolderWriteFileW != BoosterException::InputOutput::InputOutputException::Success) {
            returnHolder.second = BoosterException::InputOutput::InputOutputException::FailedToWrite;
            CloseHandle(driveHandler);
            return returnHolder;
        }

        // Close the USB drive
        CloseHandle(driveHandler);

        return returnHolder;
    }

    [[noreturn]] void Start(void) {
        while (true) {
            if (GetAppState()) {
                for (std::string untrustedDeviceID: DBMS_Instance.UntrustedDevices(this->getAllDevicesAsString())) {
                    CLI::ConsoleOut("[!] DETECTED --> " + untrustedDeviceID);

                    if (RegistryManipulators::GetItemPropertyValue(
                            "DestructionMode",
                            Requirements::defaultRegistryContainerPath)
                        == "TRUE") {

                        std::pair<std::string, BoosterException::InputOutput::InputOutputException>
                                returnHolderDriveHeaderCorrupter = DriveHeaderCorrupter(untrustedDeviceID);

                        if (RegistryManipulators::GetItemPropertyValue(
                                "Reversible",
                                Requirements::defaultRegistryContainerPath)
                            == "TRUE") {
                            DBMS_Instance.InsertFuckedDevice(untrustedDeviceID, returnHolderDriveHeaderCorrupter.first);
                        }
                    }
                    System::Sleeps::SleepForMillisecond(2100);

                    std::thread(&DriveProtection::DriveEjector, untrustedDeviceID).detach();
                }


                // ----- Impl of a logging system in future -----
//            try {
//
//            } catch (std::exception &Exception) {
//                CLI::Log(::FATAL ,Exception.what());
//            }
            } else {
                CLI::ConsoleOut("OFF");
                System::Sleeps::SleepForMillisecond(2000);
            }
        }
    }
};


#endif //SYSTEMPROTECTION_HPP
