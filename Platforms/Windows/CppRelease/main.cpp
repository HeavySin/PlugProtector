#include "ControlPanel.hpp"
#include <thread>

int main() {
    if (!ControlPanel::PlugProtectorInstalled()) {
        CLI::PrintTitle("WARN");
        CLI::ConsoleOut(Requirements::appName + " Isn't installed!\nFirst, install the " + Requirements::appName +
                        " via its launcher!");
        CLI::Halts::PressEnterTo("exit");
        return -1;
    } else {
        ControlPanel::Login();
    }

    DataManager DBMS(System::Paths::GetAppDataDirectory0() + "\\TrustedDevices");
    ControlPanel Controller(DBMS);

    bool mainLoop = true;
    while (mainLoop) {
        int32_t chosenTableItemMain{TUI::SimplePanel("PlugProtector", { "Devices", "Preferences", "Reload"})};


        if (chosenTableItemMain == 1) /* Going to Devices */ {
            bool devicesLoop = true;
            while (devicesLoop) {
                int32_t chosenTableItem_Devices{TUI::SimplePanel(
                        "Devices",
                        {
                                "TrustedDevices List",
                                "Add New Device to TrustedDevices",
                                "Remove Device from TrustedDevices",
                                "Fix a Hidden Device",
                                "LOGS (SOON...)",
                        }, TUI::GetPanelPathHolder({"Devices"}), false, true)};

                switch (chosenTableItem_Devices) {
                    case 1:
                        ControlPanel::PrintDevices(DBMS.RetrieveTrustedDevices());
                        CLI::Halts::PressEnterTo();
                        break;
                    case 2:
                        DataManager::ChangeAppState(false);
                        Controller.AddNewDevice();
                        CLI::Halts::PressEnterTo();
                        std::thread(&DataManager::ChangeAppState, true, 1400).detach();
                        break;
                    case 3:
                        Controller.RemoveTrustedDevice();
                        CLI::Halts::PressEnterTo();
                        break;
                    case 4:
                        Controller.FixHiddenDevice();
                        CLI::Halts::PressEnterTo();
                        break;
                    case 5:
                        // LOGS (SOON ...)
                        break;
                    case 6:
                        devicesLoop = false;
                        break;
                    default:
                        break;
                }
            }
        } else if (chosenTableItemMain == 2) /*  Going to Preferences */ {
            bool preferencesLoop = true;
            while (preferencesLoop) {
                int32_t chosenTableItem_Preferences{TUI::SimplePanel(
                        "Preferences",
                        {
                                "App State",
                                "Destructive State",
                                "Destructive Mode",
                                "Clear Data (SOON...)",
                                "Change Password",
                        }, TUI::GetPanelPathHolder({"Preferences"}), false, true)};

                switch (chosenTableItem_Preferences) {
                    case 1:
                        ControlPanel::AppStateChanger();
                        break;
                    case 2:
                        ControlPanel::DestructionStateChanger();
                        break;
                    case 3:;
                        ControlPanel::DestructionModeChanger();
                        break;
                    case 4:
                        // Clear Data (SOON...)
                        break;
                    case 5:
                        ControlPanel::ChangePassword();
                        break;
                    case 6:
                        preferencesLoop = false;
                        break;
                    default:
                        break;
                }
            }
        } else if (chosenTableItemMain == 0) /* Exiting the Main-Loop (Exiting the program) */ {
            mainLoop = false;
        }
    }
    return 0;
}
