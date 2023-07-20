#ifndef CONTROLPANEL_HPP
#define CONTROLPANEL_HPP

#include "Styles.hpp"

class ControlPanel {
private:
    DataManager &DBMSReference;

public:
    explicit ControlPanel(DataManager &DBMSInstanceReferenceArg) : DBMSReference(DBMSInstanceReferenceArg) {
    }

    static bool IsRegistered(void) {
        if (RegistryManipulators::checkForItemPropertyValueExistence(
                "PlugProtectorSecret",
                Requirements::defaultRegistryContainerPath) != "DOES_EXISTS"
            || RegistryManipulators::GetItemPropertyValue(
                "PlugProtectorSecret",
                Requirements::defaultRegistryContainerPath).empty()) {
            return false;
        }
        return true;
    }

    static void SetPassword(std::string passwordArg) {
        RegistryManipulators::SetOrEditItemPropertyValue(
                passwordArg,
                "PlugProtectorSecret",
                Requirements::defaultRegistryContainerPath);
    }

    static std::string GetPassword(void) {
        return RegistryManipulators::GetItemPropertyValue(
                "PlugProtectorSecret",
                Requirements::defaultRegistryContainerPath);
    }

    static void SetAppState(bool appStateArg = true) {
        RegistryManipulators::SetOrEditItemPropertyValue(
                appStateArg ? "ON" : "OFF",
                "State",
                Requirements::defaultRegistryContainerPath);
    }

    static void SetDestructionState(bool destructionStateArg = false) {
        RegistryManipulators::SetOrEditItemPropertyValue(
                destructionStateArg ? "TRUE" : "FALSE",
                "DestructionMode",
                Requirements::defaultRegistryContainerPath);
    }

    static void SetDestructionMode(bool destructionModeArg = true) {
        RegistryManipulators::SetOrEditItemPropertyValue(
                destructionModeArg ? "TRUE" : "FALSE",
                "Reversible",
                Requirements::defaultRegistryContainerPath);
    }

    static void SleepOnWrongPassword(uint16_t timesArg) {
        if (timesArg > 50) {
            CLI::ConsoleOut("You've reached the limit, u r probably cracking or somethin...");
            CLI::Halts::PressEnterTo("exit.");
            exit(-1);
        } else if (timesArg > 30) {
            CLI::Sleeps::Countdown(80, "You can try again in ");
        } else if (timesArg > 10) {
            CLI::Sleeps::Countdown(46, "You can try again in ");
        } else if (timesArg > 3) {
            CLI::Sleeps::Countdown(6, "You can try again in ");
        } else if (timesArg > 0) {
            CLI::Sleeps::Countdown(1, "You can try again in ");
        }
    }

    // Checks if the main program is launched and installed
    static bool PlugProtectorInstalled(void) {
        if (!(RegistryManipulators::GetItemPropertyValue(
                "PlugProtectorInstalled",
                Requirements::defaultRegistryContainerPath)
              == "TRUE")
            &&
            !IOManipulators::FileManipulators::Existence::CheckExistence4(
                    System::Paths::GetAppDataDirectory0()
                    + "\\PlugProtectorDevices")) {
            return false;
        } else {
            return true;
        }
    }

    static bool PrintDevices(const Devices &devicesArg, bool refreshWarnArg = false, bool previewArg = false) {
        if (!devicesArg.empty()) {
            for (const auto &trustedDeviceItem: devicesArg) {
                ControlPanel::PrintDevice(trustedDeviceItem.second, previewArg);
            }
            return true;
        } else {
            CLI::ConsoleOut("There is no device at the moment" +
                            std::string(refreshWarnArg ? "\nConnect your device any try again." : ""));
            return false;
        }
    }

    static void PrintDevice(const deviceStructure &deviceArg, bool previewArg = false, bool bottomPaddingArg = true) {
        std::string untrustedDeviceStyleHolder = "[GUID] \"" + deviceArg.ID + "\" {"
                                                 + (previewArg ? "" : ("\n\t[SerialNumber] " +
                                                                       deviceArg.SerialNumber))
                                                 + "\n\t[Name] " + deviceArg.Name
                                                 + "\n\t[Label] " + deviceArg.Label
                                                 + (previewArg ? "" : ("\n\t[FileSystem] " +
                                                                       deviceArg.FileSystem))
                                                 + "\n\t[DriveLetter] " + deviceArg.DriveLetter
                                                 + "\n\t[Capacity] " + IntManipulators::Convertors::ConvertByte(
                deviceArg.FreeSpace)
                                                 + " / "
                                                 + IntManipulators::Convertors::ConvertByte(
                deviceArg.Capacity)
                                                 + "\n}"
                                                 + (bottomPaddingArg ? "\n\n" : "");

        CLI::ConsoleOut(untrustedDeviceStyleHolder);
    }

    void AddNewDevice(void) {
        CLI::PrintTitle("AddNewDevice");
        Devices retrievedUntrustedDevices{DBMSReference.RetrieveUntrustedDevices()};
        bool isThereAnyDevice = ControlPanel::PrintDevices(retrievedUntrustedDevices, true);

        CLI::ConsoleOut("\n(0 to exit) - (Enter to refresh)");
        std::string selectedDeviceGUID{TUI::ReadOption(TUI::GetPanelPathHolder({"Devices", "AddNewDevice"}),
                                                       isThereAnyDevice ? "Enter a GUID: " : "")};
        if (selectedDeviceGUID == "0") {
            return;
        }
        while (!retrievedUntrustedDevices.contains(selectedDeviceGUID)) {
            CLI::NotFound(selectedDeviceGUID, true);

            CLI::PrintTitle("AddNewDevice");
            retrievedUntrustedDevices = DBMSReference.RetrieveUntrustedDevices();

            isThereAnyDevice = ControlPanel::PrintDevices(retrievedUntrustedDevices, true);

            CLI::ConsoleOut("\n(0 to exit) - (Enter to refresh)");
            selectedDeviceGUID = TUI::ReadOption(TUI::GetPanelPathHolder({"Devices", "AddNewDevice"}),
                                                 isThereAnyDevice ? "Enter a GUID: " : "");
            if (selectedDeviceGUID == "0") {
                return;
            }
        }

        deviceStructure selectedDevice = retrievedUntrustedDevices.at(selectedDeviceGUID);
        CLI::ConsoleOut("\n\n---------------------------------------------");
        PrintDevice(selectedDevice, true, false);
        CLI::ConsoleOut("---------------------------------------------\n\n");

        if (AuthenticateAnswer({"Devices", "AddNewDevice"})) {
            DBMSReference.InsertNewDevice(selectedDevice);
            CLI::ConsoleOut("New Device Added!");
        } else {
            CLI::ConsoleOut("Operation Aborted.");
        }

        if (AuthenticateAnswer({"Devices", "AddNewDevice"}, false, "Wanna add more?")) {
            AddNewDevice();
        }
    }

    void RemoveTrustedDevice(void) {
        CLI::PrintTitle("RemoveTrustedDevice");
        Devices retrievedTrustedDevices{DBMSReference.RetrieveTrustedDevices()};
        bool isThereAnyDevice = ControlPanel::PrintDevices(retrievedTrustedDevices, true);

        CLI::ConsoleOut("\n(0 to exit) - (Enter to refresh)");
        std::string selectedDeviceGUID{
                TUI::ReadOption(TUI::GetPanelPathHolder({"Devices", "RemoveTrustedDevice"}),
                                isThereAnyDevice ? "Enter a GUID: " : "")};
        if (selectedDeviceGUID == "0") {
            return;
        }
        while (!retrievedTrustedDevices.contains(selectedDeviceGUID)) {
            CLI::NotFound(selectedDeviceGUID, true);

            CLI::PrintTitle("RemoveTrustedDevice");
            retrievedTrustedDevices = DBMSReference.RetrieveTrustedDevices();

            isThereAnyDevice = ControlPanel::PrintDevices(retrievedTrustedDevices, true);

            CLI::ConsoleOut("\n(0 to exit) - (Enter to refresh)");
            selectedDeviceGUID = TUI::ReadOption(TUI::GetPanelPathHolder({"Devices", "RemoveTrustedDevice"}),
                                                 isThereAnyDevice ? "Enter a GUID: " : "");
            if (selectedDeviceGUID == "0") {
                return;
            }
        }

        deviceStructure selectedDevice = retrievedTrustedDevices.at(selectedDeviceGUID);
        CLI::ConsoleOut("\n\n---------------------------------------------");
        PrintDevice(selectedDevice, true, false);
        CLI::ConsoleOut("---------------------------------------------\n\n");

        if (AuthenticateAnswer({"Devices", "RemoveTrustedDevice",}, false)) {
            DBMSReference.RemoveTrustedDevice(selectedDevice.ID);
            CLI::ConsoleOut("Device removed from trusted list!");
        } else {
            CLI::ConsoleOut("Operation Aborted.");
        }
    }

    static bool AuthenticateAnswer(const std::vector<std::string> &pathsArg, bool defaultArg = true,
                                   const std::string &additionalInfoArg = "ARE YOU SURE?") {
        std::string addDeviceAuthentication;
        const std::array<const std::string, 10> acceptKeywords{"y", "yy", "ye", "yes", "ya", "yup", "yeah", "true",
                                                               "t"};
        const std::array<const std::string, 10> denyKeywords{"n", "nn", "na", "no", "nah", "nope", "false", "f"};

        while (true) {
            addDeviceAuthentication = TUI::ReadOption(TUI::GetPanelPathHolder(pathsArg),
                                                      additionalInfoArg + " (yes, no, default=" +
                                                      (defaultArg ? "yes" : "no") + ") ");
            if (addDeviceAuthentication.empty()) {
                return defaultArg;
            } else {
                if (STLManipulators::Contains(denyKeywords, addDeviceAuthentication)) {
                    return false;
                }
                if (STLManipulators::Contains(acceptKeywords, addDeviceAuthentication)) {
                    return true;
                }
            }
        }
    }

    static void HiddenDeviceFixer(const std::string &IDArg) {
        std::string ALPHABET{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

        for (char driveLetter: ALPHABET) {
            std::string commandHolder = "powershell -command \"$usbdev = gwmi win32_volume"
                                        " | where{$_.DeviceID -eq "
                                        "\'" + IDArg + "\'"
                                        + "};$usbdev.DriveLetter = "
                                        + "\'" + driveLetter + ":\'; $usbdev.Put()";
            std::string commandResult = System::ExecuteCommand(commandHolder);

            if (commandResult.contains("Exception calling")) {
                if (commandResult.contains("Access is denied")) {
                    CLI::ConsoleOut("Access Denied!\nRun the program as Administrator");
                    return;
                }
            } else {
                CLI::ConsoleOut("Your device has been fixed successfully.");
                return;
            }
        }
    }

    void FixHiddenDevice(void) {
        CLI::PrintTitle("FixHiddenDevice");
        Devices retrievedUntrustedDevices{DBMSReference.RetrieveUntrustedDevices()};
        bool isThereAnyDevice = ControlPanel::PrintDevices(retrievedUntrustedDevices, true);

        CLI::ConsoleOut("\n(0 to exit) - (Enter to refresh)");
        std::string selectedDeviceGUID{TUI::ReadOption(TUI::GetPanelPathHolder({"Devices", "FixHiddenDevice"}),
                                                       isThereAnyDevice ? "Enter a GUID: " : "")};
        if (selectedDeviceGUID == "0") {
            return;
        }
        while (!retrievedUntrustedDevices.contains(selectedDeviceGUID)) {
            CLI::NotFound(selectedDeviceGUID, true);

            CLI::PrintTitle("FixHiddenDevice");
            retrievedUntrustedDevices = DBMSReference.RetrieveUntrustedDevices();

            isThereAnyDevice = ControlPanel::PrintDevices(retrievedUntrustedDevices, true);

            CLI::ConsoleOut("\n(0 to exit) - (Enter to refresh)");
            selectedDeviceGUID = TUI::ReadOption(TUI::GetPanelPathHolder({"Devices", "FixHiddenDevice",}),
                                                 isThereAnyDevice ? "Enter a GUID: " : "");
            if (selectedDeviceGUID == "0") {
                return;
            }
        }

        deviceStructure selectedDevice = retrievedUntrustedDevices.at(selectedDeviceGUID);
        CLI::ConsoleOut("\n\n---------------------------------------------");
        PrintDevice(selectedDevice, true, false);
        CLI::ConsoleOut("---------------------------------------------\n\n");

        if (AuthenticateAnswer({"Devices", "FixHiddenDevice"})) {
            HiddenDeviceFixer(selectedDevice.ID);
        } else {
            CLI::ConsoleOut("Operation Aborted.");
        }

        if (AuthenticateAnswer({"Devices", "FixHiddenDevice"}, false, "Wanna fix more?")) {
            FixHiddenDevice();
        }
    }

    static void Login(void) {
        if (!ControlPanel::IsRegistered()) {
            ControlPanel::Register();
        } else {
            CLI::PrintTitle("LOGIN");
            const std::string passwordHolder{GetPassword()};

            uint16_t passwordEnteredWrong{0};
            while (passwordHolder != TUI::ReadOption("", "Password: ")) {
                SleepOnWrongPassword(++passwordEnteredWrong);
            }
            CLI::ConsoleOut("Successful Login!");
            System::Sleeps::SleepForMillisecond(400);
        }
    }

    static void Register(void) {
        if (!ControlPanel::IsRegistered()) {
            CLI::PrintTitle("REGISTRATION");
            CLI::ConsoleOut("Setup a password for your app");
            std::string passwordGetter{TUI::ReadOption("", "New Password: ")};
            while (passwordGetter.length() <= 4) {
                CLI::PrintTitle("REGISTRATION");
                CLI::ConsoleOut("Password should be more than 4 characters!\nSetup a password for your app");
                passwordGetter = TUI::ReadOption("", "New Password: ");
            }
            std::string passwordGetter2{TUI::ReadOption("", "Repeat the password: ")};
            if (passwordGetter2 != passwordGetter) {
                CLI::ConsoleOut("\nPasswords doesn't match!\nTry again.");
                CLI::Halts::PressEnterTo();
                ControlPanel::Register();
            } else {
                SetPassword(passwordGetter2);
                CLI::ConsoleOut("\nRegistered Successfully!");
                CLI::Halts::PressEnterTo("Login.");
                ControlPanel::Login();
            }
        }
    }

    static void ChangePassword(void) {
        CLI::PrintTitle("ChangePassword");
        const std::string passwordHolder{GetPassword()};

        CLI::ConsoleOut("Verify your current password.");
        uint16_t passwordEnteredWrong{0};
        while (passwordHolder != TUI::ReadOption("", "Current Password: ")) {
            SleepOnWrongPassword(++passwordEnteredWrong);
        }

        TryAgain:
        CLI::PrintTitle("ChangePassword");
        CLI::ConsoleOut("Setup a new password for your app");
        std::string passwordGetter{TUI::ReadOption("", "New Password: ")};
        while (passwordGetter.length() <= 4) {
            CLI::PrintTitle("REGISTRATION");
            CLI::ConsoleOut("Password should be more than 4 characters!\nSetup a new password for your app");
            passwordGetter = TUI::ReadOption("", "New Password: ");
        }
        std::string passwordGetter2{TUI::ReadOption("", "Repeat the password: ")};
        if (passwordGetter2 != passwordGetter) {
            CLI::ConsoleOut("\nPasswords doesn't match!\nTry again.");
            CLI::Halts::PressEnterTo();
            goto TryAgain;
        } else {
            SetPassword(passwordGetter2);
            CLI::ConsoleOut("\nPassword changed Successfully!");
            CLI::Halts::PressEnterTo();
        }
    }

    static void AppStateChanger(void) {
        bool appStateChangerLoop = true;
        while (appStateChangerLoop) {
            int32_t chosenTableItem_AppState{TUI::SimplePanel(
                    "Preferences",
                    {
                            "ON" + std::string(DataManager::GetAppState() ? " - Current" : ""),
                            "OFF" + std::string(DataManager::GetAppState() ? "" : " - Current"),
                    }, TUI::GetPanelPathHolder({"Preferences", "AppState"}), false, true, false)};

            switch (chosenTableItem_AppState) {
                case 1:
                    SetAppState(true);
                    break;
                case 2:
                    SetAppState(false);
                    break;
                case 3:
                    appStateChangerLoop = false;
                    break;
                default:
                    break;
            }
        }
    }

    static void DestructionStateChanger(void) {
        bool destructionStateChangerLoop = true;
        while (destructionStateChangerLoop) {
            int32_t chosenTableItem_DestructionState{TUI::SimplePanel(
                    "DestructionState",
                    {
                            "ON" + std::string(DataManager::GetDestructionState() ? " - Current" : ""),
                            "OFF" + std::string(DataManager::GetDestructionState() ? "" : " - Current"),
                    }, TUI::GetPanelPathHolder({"Preferences", "DestructionState"}), false, true, false)};

            switch (chosenTableItem_DestructionState) {
                case 1:
                    SetDestructionState(true);
                    break;
                case 2:
                    SetDestructionState(false);
                    break;
                case 3:
                    destructionStateChangerLoop = false;
                    break;
                default:
                    break;
            }
        }
    }

    static void DestructionModeChanger(void) {
        bool destructionModeChangerLoop = true;
        while (destructionModeChangerLoop) {
            int32_t chosenTableItem_DestructionMode{TUI::SimplePanel(
                    "DestructionMode",
                    {
                            "Irreversible" + std::string(DataManager::GetDestructionMode() ? "" : " - Current"),
                            "Reversible" + std::string(DataManager::GetDestructionMode() ? " - Current" : ""),
                    }, TUI::GetPanelPathHolder({"Preferences", "DestructionMode"}), false, true, false)};

            switch (chosenTableItem_DestructionMode) {
                case 1:
                    SetDestructionMode(false);
                    break;
                case 2:
                    SetDestructionMode(true);
                    break;
                case 3:
                    destructionModeChangerLoop = false;
                    break;
                default:
                    break;
            }
        }
    }
};


#endif //CONTROLPANEL_HPP
